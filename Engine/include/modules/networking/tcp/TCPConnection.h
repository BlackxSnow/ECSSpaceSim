#pragma once

#include "../../../utility/CCXEvent.h"
#include "../../../utility/CCXThread.h"
#include "../NetDefs.h"
#include "../Packet.h"
#include <queue>

namespace Thera::Net::tcp
{
	/// <summary>
	/// Established two way TCP connection.
	/// </summary>
	class Connection : public CCX::ManagedSharedResource<Connection>
	{
	public:
		/// <summary>
		/// Disconnect callback event.
		/// </summary>
		CCX::Event<std::shared_ptr<Connection>> Disconnected;
	private:
		std::mutex _Mutex;
		asio::io_context _Context;
		asio::ip::tcp::endpoint _Endpoint;
		asio::ip::tcp::socket _Socket;

		std::queue<std::shared_ptr<AggregatePacket>> _SendQueue;
		std::vector<byte> _ReceiveBuffer;

		bool _IsActive;
		bool _IsClosed;

		void Listen();

		void HandleReceive(const asio::error_code& error, size_t bytesTransferred);
		void HandleDisconnect();
	public:
		asio::ip::tcp::endpoint Endpoint() const { return _Endpoint; }
        asio::ip::tcp::endpoint LocalEndpoint() const { return _Socket.local_endpoint(); }

		/// <summary>
		/// If the connection was created with startActive = false, this function will cause the connection to go 'live'.
		/// </summary>
		void Activate();

		/// <summary>
		/// Queue a packet for send on this connection.
		/// </summary>
		/// <param name="packet"></param>
		void Send(Packet& packet);
		/// <summary>
		/// Send all queued packets through the connection.
		/// </summary>
		/// <param name="local"></param>
		/// <param name="remote"></param>
		/// <returns></returns>
		void Flush();
		
		/// <summary>
		/// Close this connection and its underlying socket.
		/// </summary>
		/// <param name="local"></param>
		/// <param name="address"></param>
		/// <param name="port"></param>
		/// <param name="error"></param>
		void Close()
		{
			if (_IsClosed) return;
			_Socket.cancel();
			_Context.stop();
			_Socket.close();
			_IsClosed = true;
		}

		/// <summary>
		/// Create a new connection from the local to the target address and port. On failure modifies provided error_code.
		/// </summary>
		static std::shared_ptr<Connection> Create(const asio::ip::tcp::endpoint& local, const asio::ip::address& address, const asio::ip::port_type port, asio::error_code& error);
		/// <summary>
		/// Create a new connection from an existing native socket.
		/// </summary>
		static std::shared_ptr<Connection> Create(asio::ip::tcp::socket& socket, bool startActive);

		/// <summary>
		/// Exposed constructor for internal usage. You probably want to use Connection::Create instead.
		/// </summary>
		template<typename ...Args>
		explicit Connection(CreationKey, Args&&... args) : Connection(std::forward<Args>(args)...) {}
	private:
		Connection(const asio::ip::tcp::endpoint& local, const asio::ip::address& address, const asio::ip::port_type port, asio::error_code& error);
		Connection(asio::ip::tcp::socket& socket, bool startActive);
		Connection(const Connection&) = delete;
		const Connection& operator =(const Connection&) = delete;
	public:
		~Connection()
		{
			Close();
		}
	};

	/// <summary>
	/// TCP Listener for accepting incoming socket requests.
	/// </summary>
	class Listener : public CCX::ManagedSharedResource<Listener>
	{		
	private:
		asio::io_context _Context;
		asio::ip::tcp::acceptor _Acceptor;
		std::function<void(std::shared_ptr<Connection>)> _OnAccept;
		bool _IsClosed = false;
		
		void HandleAccept(const asio::error_code& ec, asio::ip::tcp::socket socket)
		{
			if (ec)
			{
				if (ec.value() == asio::error::operation_aborted)
				{
					LogInfo((std::ostringstream() << "Aborting TCP Listen").str());
					//LogInfo((std::ostringstream() << "Aborting listen on port " << _Acceptor.local_endpoint().port()).str());
					return;
				}
				LogError(ec.message(), false);
				return;
			}
			
			_OnAccept(Connection::Create(socket, false));
			auto bind = std::bind(&Listener::HandleAccept, this, std::placeholders::_1, std::placeholders::_2);
			_Acceptor.async_accept(bind);
		}

		void Activate()
		{
			CCX::RunTask([&]() { _Context.run(); }, shared_from_this(), [&]() { Close(); });
		}

	public:
		/// <summary>
		/// Close the listener and the underlying socket.
		/// </summary>
		/// <param name="port"></param>
		/// <param name="onAccept"></param>
		void Close()
		{
			if (_IsClosed) return;
			_Acceptor.cancel();
			_Context.stop();
			_Acceptor.close();
			_IsClosed = true;
		}

		/// <summary>
		/// Create a new listener on the specified port. Calls onAccept on each new connection.
		/// </summary>
		inline static std::shared_ptr<Listener> Create(const asio::ip::port_type& port, const std::function<void(std::shared_ptr<Connection>)>& onAccept)
		{
			auto listener = std::make_shared<Listener>(CreationKey{}, port, onAccept);
			listener->Activate();
			return listener;
		}
		
		template<typename ...Args>
		explicit Listener(const CreationKey&, Args&&... args) : Listener(std::forward<Args>(args)...) {}
		
	private:
		Listener(const asio::ip::port_type& port, const std::function<void(std::shared_ptr<Connection>)>& onAccept) : _Acceptor(_Context), _OnAccept(onAccept)
		{
			asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), port);
			_Acceptor.open(endpoint.protocol());
			_Acceptor.set_option(asio::ip::tcp::no_delay(true));
			_Acceptor.bind(endpoint);
			_Acceptor.listen();
			_Acceptor.async_accept(std::bind(&Listener::HandleAccept, this, std::placeholders::_1, std::placeholders::_2));
		}
	public:
		~Listener()
		{
			Close();
		}
	};
}