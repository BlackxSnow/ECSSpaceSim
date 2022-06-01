#include "Connection.h"
#include <iostream>

namespace ecse::Networking
{

	void Connection::HandleReceive(const asio::error_code& error, size_t bytesTransferred)
	{
		if (error)
		{
			LogError(error.message(), false);
			Listen();
			return;
		}
		if (bytesTransferred < PacketHeader::HEADER_SIZE)
		{
			LogError("Received packet with invalid size " + std::to_string(bytesTransferred), false);
			Listen();
			return;
		}

		CCX::MemoryReader reader(_ReceiveBuffer.data(), bytesTransferred);
		PacketSize packetCount = reader.Read<PacketSize>();
		PacketSize packetSize = reader.Read<PacketSize>();
		if (packetSize + AggregatePacket::HEADER_SIZE > bytesTransferred)
		{
			LogError("Received partial aggregate packet of " + std::to_string(bytesTransferred) + " bytes. Expected " + std::to_string(packetSize + AggregatePacket::HEADER_SIZE), false);
			Listen();
			return;
		}

		PacketHandler* handler;
		for (int i = 0; i < packetCount; i++)
		{
			Packet packet(reader);
			if (!TryGetPacketHandler(packet.ID(), handler))
			{
				LogWarning("Received unhandled packet id '" + std::to_string(packet.ID()) + "' from " + (std::ostringstream() << _Endpoint).str(), false);
				continue;
			}
			(*handler)(_Endpoint, &packet);
		}

		std::cout << "Received " << bytesTransferred << " bytes from " << _Endpoint << std::endl;
		Listen();
	}
	
	void Connection::Listen()
	{
		_Socket.async_receive_from(asio::buffer(_ReceiveBuffer), _Endpoint,
			std::bind(&Connection::HandleReceive, this, std::placeholders::_1, std::placeholders::_2));
	}

	const size_t MAX_PACKET_COUNT = (asio::detail::max_iov_len / 2) - 1;
	void Connection::Send(Packet& packet)
	{
		if (_SendQueue.empty())
		{
			auto& aggregate = _SendQueue.emplace(std::make_shared<AggregatePacket>(4));
			aggregate->AddPacket(packet);
			return;
		}
		auto& lastAggregate = _SendQueue.back();
		if (lastAggregate->Count >= MAX_PACKET_COUNT)
		{
			auto& aggregate = _SendQueue.emplace(std::make_shared<AggregatePacket>(4));
			aggregate->AddPacket(packet);
			return;
		}
		
		lastAggregate->AddPacket(packet);
	}

	void Connection::Flush()
	{
		if (_SendQueue.empty())
			return;

		while (!_SendQueue.empty())
		{
			auto& aggregate = _SendQueue.front();
			auto buffer = aggregate->GetBuffer();
			auto size = asio::buffer_size(buffer);
			std::cout << "sending on " << _Endpoint << std::endl;
			size_t sent = _Socket.send_to(buffer, _Endpoint);
			if (sent != size)
			{
				LogError("Sent " + std::to_string(sent) + " of " + std::to_string(size) + " bytes for aggregate with count " + std::to_string(aggregate->Count), false);
			}
			_SendQueue.pop();
		}
	}

	Connection::Connection(const asio::ip::udp::endpoint& local, const asio::ip::address& address, const asio::ip::port_type port) : _Socket(_Context), _ReceiveBuffer(1024)
	{
		auto endpoints = asio::ip::udp::resolver(_Context).resolve(address.to_string(), std::to_string(port));
		_Endpoint = *endpoints.begin();
		
		std::cout << "Results begin" << std::endl;
		for (auto& endpoint : endpoints)
		{
			std::cout << endpoint.endpoint() << std::endl;
		}
		std::cout << "Results end" << std::endl;

		_Socket.open(asio::ip::udp::v4());
		_Socket.set_option(asio::ip::udp::socket::reuse_address(true));
		_Socket.bind(local);
		Listen();
		_Thread = new std::thread([&]() { _Context.run(); });
	}

	Connection::Connection(const asio::ip::udp::endpoint& local, const asio::ip::udp::endpoint& endpoint) : _Socket(_Context), _Endpoint(endpoint), _ReceiveBuffer(1024)
	{
		_Socket.open(asio::ip::udp::v4());
		_Socket.set_option(asio::ip::udp::socket::reuse_address(true));
		_Socket.bind(local);
		Listen();
		_Thread = new std::thread([&]() { _Context.run(); });
	}

	Connection::Connection(const asio::ip::port_type& port) : _Socket(_Context, asio::ip::udp::endpoint(asio::ip::udp::v4(), port)), _ReceiveBuffer(1024), _Thread(nullptr)
	{
		Listen();
		_Thread = new std::thread([&]() { _Context.run(); });
	}

#ifdef _WIN32
	inline bool TestMTUOnce(int size, int& minMTU, int& maxMTU, HANDLE icmpHandle, IPAddr target, void* data, PIP_OPTION_INFORMATION options)
	{
		auto replyCount = IcmpSendEcho(icmpHandle, target, data, size, options, data, 65536, 1000);
		bool gotReply = replyCount > 0;
		if (gotReply) minMTU = size;
		else maxMTU = size;

		return gotReply;
	}
	int DiscoverMTU(asio::ip::address_v4& target)
	{
		auto icmpHandle = IcmpCreateFile();
		if (icmpHandle == INVALID_HANDLE_VALUE)
		{
			auto error = GetLastError();
			LogError("ICMP handle creation failed: " + std::to_string(error), false);
			return 576;
		}

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
			gotReply = TestMTUOnce(current, minMTU, maxMTU, icmpHandle, target.to_uint(), data, options);
		}

		while (maxMTU - minMTU > 1)
		{
			int current = minMTU + (maxMTU - minMTU) / 2.0f;
			TestMTUOnce(current, minMTU, maxMTU, icmpHandle, target.to_uint(), data, options);
		}

		LogInfo("Found MTU of " + std::to_string(minMTU));
		return minMTU;
	}
#endif
}