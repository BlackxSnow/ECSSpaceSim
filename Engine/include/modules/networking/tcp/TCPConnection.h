#pragma once

#include "../../../utility/CCXEvent.h"
#include "../../../utility/CCXThread.h"
#include "../NetDefs.h"
#include "../Packet.h"
#include <queue>

namespace Thera::Net::tcp
{
	class Connection : public CCX::ManagedSharedResource<Connection>
	{
	public:
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
		const asio::ip::tcp::endpoint& Endpoint() { return _Endpoint; }
		const asio::ip::tcp::endpoint& LocalEndpoint() { return _Socket.local_endpoint(); }

		/// <summary>
		/// If the connection was created with isListening = false, this function will cause the connection to go 'live'.
		/// </summary>
		void Activate();

		void Send(Packet& packet);
		void Flush();
		
		void Close()
		{
			if (_IsClosed) return;
			_Socket.cancel();
			_Context.stop();
			_Socket.close();
			_IsClosed = true;
		}

		static std::shared_ptr<Connection> Create(const asio::ip::tcp::endpoint& local, const asio::ip::address& address, const asio::ip::port_type port, asio::error_code& error);
		static std::shared_ptr<Connection> Create(asio::ip::tcp::socket& socket, bool startActive);

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
		void Close()
		{
			if (_IsClosed) return;
			_Acceptor.cancel();
			_Context.stop();
			_Acceptor.close();
			_IsClosed = true;
		}

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