#pragma once

#include "NetDefs.h"
#include "Packet.h"
#include <queue>
#include <utility/CCXThread.h>
#include <utility/CCXMemory.h>
#include <utility/CCXEvent.h>

#ifdef _WIN32
#include <libloaderapi.h>
#include <IPExport.h>
#include <inaddr.h>
#include <IcmpAPI.h>
#include <iostream>
#endif

namespace Thera::Net
{
	class Connection : public CCX::ManagedSharedResource<Connection>
	{
	private:
		asio::io_context _Context;
		asio::ip::udp::socket _Socket;
		asio::ip::udp::endpoint _Endpoint;
		size_t _MTU;
		bool _IsActive = false;
		bool _IsClosed = false;

		std::queue<std::shared_ptr<AggregatePacket>> _SendQueue;
		std::vector<byte> _ReceiveBuffer;
	public:
		CCX::Event<std::shared_ptr<Connection>> Disconnected;
	private:
		void HandleReceive(const asio::error_code& error, size_t bytesTransferred);
		void HandleDisconnect();

		void Listen();

	public:
		void Send(Packet& packet);
		void Flush();
		size_t MTU() { return _MTU; }
		void MTU(size_t value) { _MTU = value; }

		void Close()
		{
			if (_IsClosed) return;
			_Socket.cancel();
			_Context.stop();
			_Socket.close();
			_IsClosed = true;
		}

		void Activate()
		{
			if (_IsActive) return;
			CCX::RunTask([&]() { _Context.run(); }, shared_from_this(), [&]() { Close(); });
			_IsActive = true;
		}

		static inline std::shared_ptr<Connection> Create(const asio::ip::udp::endpoint& local, const asio::ip::address& address, const asio::ip::port_type& port)
		{
			auto connection = std::make_shared<Connection>(CreationKey{}, local, address, port);
			connection->Activate();
			return connection;
		}
		static inline std::shared_ptr<Connection> Create(const asio::ip::udp::endpoint& local, const asio::ip::udp::endpoint& endpoint)
		{
			auto connection = std::make_shared<Connection>(CreationKey{}, local, endpoint);
			connection->Activate();
			return connection;
		}
		static inline std::shared_ptr<Connection> Create(const asio::ip::port_type& port)
		{
			auto connection = std::make_shared<Connection>(CreationKey{}, port);
			connection->Activate();
			return connection;
		}

		template<typename ...Args>
		explicit Connection(CreationKey, Args&& ...args) : Connection(std::forward<Args>(args)...) {}
	private:
		Connection(const asio::ip::udp::endpoint& local, const asio::ip::address& address, const asio::ip::port_type& port);
		Connection(const asio::ip::udp::endpoint& local, const asio::ip::udp::endpoint& endpoint);
		Connection(const asio::ip::port_type& port);
	public:
		~Connection();
	};


	int DiscoverMTU(asio::ip::address_v4& address);
}