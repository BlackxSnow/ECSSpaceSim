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

namespace Thera::Net
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
		size_t MTU() { return _MTU; }
		void MTU(size_t value) { _MTU = value; }

		Connection(const asio::ip::udp::endpoint& local, const asio::ip::address& address, const asio::ip::port_type port);
		Connection(const asio::ip::udp::endpoint& local, const asio::ip::udp::endpoint& endpoint);
		Connection(const asio::ip::port_type& port);
	};


	int DiscoverMTU(asio::ip::address_v4& address);
}