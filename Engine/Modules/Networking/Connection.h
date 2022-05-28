#pragma once

#include "NetDefs.h"
#include "Packet.h"
#include <queue>

#ifdef _WIN32
#include <libloaderapi.h>
#include <IPExport.h>
#include <inaddr.h>
#include <IcmpAPI.h>
#include <iostream>
#endif

namespace ecse::Networking
{
	class Connection
	{
	private:
		asio::io_context _Context;
		std::thread* _Thread;
		asio::ip::udp::socket _Socket;
		asio::ip::udp::endpoint _Endpoint;
		size_t _MTU;

		std::queue<std::shared_ptr<AggregatePacket>> _SendQueue;
		std::vector<byte> _ReceiveBuffer;
		
		void HandleReceive(const asio::error_code& error, size_t bytesTransferred);

		void Listen();

	public:
		void Send(Packet& packet);
		void Flush();

		Connection(const asio::ip::udp::endpoint& local, const asio::ip::address& address, const asio::ip::port_type port);
		Connection(const asio::ip::udp::endpoint& local, const asio::ip::udp::endpoint& endpoint);
		Connection(const asio::ip::port_type& port);
	};


#ifdef _WIN32
	//inline HMODULE icmpLib = LoadLibraryA("Iphlpapi.dll");

	inline bool TestMTUOnce(int size, int& minMTU, int& maxMTU, HANDLE icmpHandle, IPAddr target, void* data, PIP_OPTION_INFORMATION options)
	{
		auto replyCount = IcmpSendEcho(icmpHandle, target, data, size, options, data, 65536, 1000);
		bool gotReply = replyCount > 0;
		if (gotReply) minMTU = size;
		else maxMTU = size;

		return gotReply;
	}

	inline void DiscoverMTU(byte addr1, byte addr2, byte addr3, byte addr4)
	{
		auto icmpHandle = IcmpCreateFile();
		if (icmpHandle == INVALID_HANDLE_VALUE)
		{
			auto error = GetLastError();
			LogError("ICMP handle creation failed: " + std::to_string(error), false);
			return;
		}
		
		auto ipAddr = in_addr();
		ipAddr.S_un.S_un_b.s_b1 = addr1;
		ipAddr.S_un.S_un_b.s_b2 = addr2;
		ipAddr.S_un.S_un_b.s_b3 = addr3;
		ipAddr.S_un.S_un_b.s_b4 = addr4;

		char* data = new char[65536];
		auto options32 = IP_OPTION_INFORMATION32();
		options32.Ttl = 255;
		options32.Flags = IP_FLAG_DF;
		options32.OptionsSize = 0;
		options32.OptionsData = nullptr;
		auto options = reinterpret_cast<IP_OPTION_INFORMATION*>(&options32);

		int minMTU = 548; // (UDP min MTU) 576 - (ICMP Header) 28
		int maxMTU = 65536;

		bool gotReply = true;
		while (gotReply)
		{
			int current = minMTU * 2;
			gotReply = TestMTUOnce(current, minMTU, maxMTU, icmpHandle, ipAddr.S_un.S_addr, data, options);
		}

		while (maxMTU - minMTU > 1)
		{
			int current = minMTU + (maxMTU - minMTU) / 2.0f;
			TestMTUOnce(current, minMTU, maxMTU, icmpHandle, ipAddr.S_un.S_addr, data, options);
		}

		LogInfo("Found MTU of " + std::to_string(minMTU));
	}
#endif
}