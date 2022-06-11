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
	/// <summary>
	/// UDP connection wrapper.
	/// </summary>
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
		/// <summary>
		/// Callback on disconnect (Generally a local error for UDP).
		/// </summary>
		CCX::Event<std::shared_ptr<Connection>> Disconnected;
	private:
		void HandleReceive(const asio::error_code& error, size_t bytesTransferred);
		void HandleDisconnect();

		void Listen();

	public:
		/// <summary>
		/// Queue a packet for sending on this connection.
		/// </summary>
		/// <param name="packet"></param>
		void Send(Packet& packet);
		/// <summary>
		/// Send all queued packets.
		/// </summary>
		void Flush();
		/// <summary>
		/// Retrieve the MTU of this connection.
		/// </summary>
		/// <returns></returns>
		size_t MTU() { return _MTU; }
		/// <summary>
		/// Set the MTU of this connection.
		/// </summary>
		/// <param name="value"></param>
		void MTU(size_t value) { _MTU = value; }
		const asio::ip::udp::endpoint& Endpoint() { return _Endpoint; }

		/// <summary>
		/// Close this connection and its underlying socket.
		/// </summary>
		void Close()
		{
			if (_IsClosed) return;
			_Socket.cancel();
			_Context.stop();
			_Socket.close();
			_IsClosed = true;
		}

		/// <summary>
		/// Cause this connection to go 'live' and begin listening for data.
		/// </summary>
		void Activate()
		{
			if (_IsActive) return;
			CCX::RunTask([&]() { _Context.run(); }, shared_from_this(), [&]() { Close(); });
			_IsActive = true;
		}

		/// <summary>
		/// Create a new connection from 'local' to the endpoint defined by 'address' and 'port'.
		/// </summary>
		/// <param name="local"></param>
		/// <param name="address"></param>
		/// <param name="port"></param>
		/// <returns></returns>
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
		/// <summary>
		/// Create a new connection that listens for any incoming data on a port.
		/// </summary>
		/// <param name="port"></param>
		/// <returns></returns>
		static inline std::shared_ptr<Connection> Create(const asio::ip::port_type& port)
		{
			auto connection = std::make_shared<Connection>(CreationKey{}, port);
			connection->Activate();
			return connection;
		}

		/// <summary>
		/// Exposed constructor for internal usage. Use the static Create() methods instead.
		/// </summary>
		/// <typeparam name="...Args"></typeparam>
		/// <param name=""></param>
		/// <param name="...args"></param>
		template<typename ...Args>
		explicit Connection(CreationKey, Args&& ...args) : Connection(std::forward<Args>(args)...) {}
	private:
		Connection(const asio::ip::udp::endpoint& local, const asio::ip::address& address, const asio::ip::port_type& port);
		Connection(const asio::ip::udp::endpoint& local, const asio::ip::udp::endpoint& endpoint);
		Connection(const asio::ip::port_type& port);
	public:
		~Connection();
	};

	/// <summary>
	/// Determine the MTU to an address via ICMP.
	/// </summary>
	/// <param name="address"></param>
	/// <returns></returns>
	int DiscoverMTU(asio::ip::address_v4& address);
}