#include <modules/networking/tcp/TCPConnection.h>
#include <modules/networking/tcp/TCPPacket.h>

namespace Thera::Net::tcp
{
	void Connection::Listen()
	{
		_Socket.async_receive(asio::buffer(_ReceiveBuffer), std::bind(&Connection::HandleReceive, this, std::placeholders::_1, std::placeholders::_2));
	}

	void Connection::HandleReceive(const asio::error_code& error, size_t bytesTransferred)
	{
		if (error)
		{
			if (error.value() == asio::error::operation_aborted)
			{
				LogInfo((std::ostringstream() << "Aborted receive from connection " << _Endpoint).str());
				HandleDisconnect();
				return;
			}
			if (error.value() == asio::error::eof)
			{
				LogWarning((std::ostringstream() << "Connection " << _Endpoint << " closed connection (EOF).").str());
				HandleDisconnect();
				return;
			}
			if (error.value() == asio::error::connection_reset)
			{
				LogWarning((std::ostringstream() << "Connection " << _Endpoint << " closed connection (Reset).").str());
				HandleDisconnect();
				return;
			}
			if (error.value() == asio::error::connection_aborted)
			{
				LogInfo((std::ostringstream() << "Connection " << _Endpoint << " aborted by host machine.").str());
				HandleDisconnect();
				return;
			}
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
		while (reader.Remaining() > 0)
		{
			if (reader.Remaining() < PacketHeader::HEADER_SIZE)
			{
				LogError("Received extra data too short for another packet. Remainder: " + std::to_string(reader.Remaining()), false);
				Listen();
				return;
			}

			PacketSize packetCount = reader.Read<PacketSize>();
			PacketSize aggregatePacketSize = reader.Read<PacketSize>();

			if (aggregatePacketSize > reader.Remaining())
			{
				LogError("Received partial aggregate packet of " + std::to_string(reader.Remaining()) + " bytes. Expected " + std::to_string(aggregatePacketSize), false);
				Listen();
				return;
			}

			PacketHandler* handler;
			for (int i = 0; i < packetCount; i++)
			{
				Packet packet(reader);
				if (!TryGetPacketHandler(packet.ID(), handler))
				{
					LogWarning((std::ostringstream() << "Received unhandled packet id '" << std::to_string(packet.ID()) << "' from " << _Endpoint).str());
					continue;
				}
				(*handler)(*this, &packet);
			}
		}

		Listen();
	}

	void Connection::HandleDisconnect()
	{
		Close();
		Disconnected.Invoke(shared_from_this());
	}

	const size_t MAX_PACKET_COUNT = (asio::detail::max_iov_len / 2) - 1;
	void Connection::Activate()
	{
		if (!_IsActive)
		{
			_IsActive = true;
			Listen();
			CCX::RunTask([&]() { _Context.run(); }, shared_from_this(), [&]() { Close(); });
		}
	}
	void Connection::Send(Packet& packet)
	{
		std::lock_guard lock(_Mutex);
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
		std::lock_guard lock(_Mutex);
		if (_SendQueue.empty())
			return;

		while (!_SendQueue.empty())
		{
			auto& aggregate = _SendQueue.front();
			auto buffer = aggregate->GetBuffer();
			auto size = asio::buffer_size(buffer);
			size_t sent = _Socket.send(buffer);
			if (sent != size)
			{
				LogError("Sent " + std::to_string(sent) + " of " + std::to_string(size) + " bytes for aggregate with count " + std::to_string(aggregate->Count), false);
			}
			_SendQueue.pop();
		}
	}

	std::shared_ptr<Connection> Connection::Create(const asio::ip::tcp::endpoint& local, const asio::ip::address& address, const asio::ip::port_type port, asio::error_code& error)
	{
		auto connection = std::make_shared<Connection>(CreationKey{}, local, address, port, error);
		connection->Activate();
		return connection;
	}
	std::shared_ptr<Connection> Connection::Create(asio::ip::tcp::socket& socket, bool startActive)
	{
		auto connection = std::make_shared<Connection>(CreationKey{}, socket, startActive);
		if (startActive)
		{
			connection->Activate();
		}
		return connection;
	}

	Connection::Connection(const asio::ip::tcp::endpoint& local, const asio::ip::address& address, const asio::ip::port_type port, asio::error_code& error)
		: _Socket(_Context), _ReceiveBuffer(1024), _SendQueue(), _IsActive(false), _IsClosed(false)
	{
		asio::ip::tcp::endpoint endpoint = *asio::ip::tcp::resolver(_Context).resolve(asio::ip::tcp::endpoint(address, port)).begin();

		_Socket.open(endpoint.protocol(), error);
		if (error) return;
		_Socket.set_option(asio::ip::tcp::no_delay(true));
		_Socket.bind(local, error);
		if (error) return;
		_Socket.connect(endpoint, error);
		if (error) return;

		_Endpoint = _Socket.remote_endpoint(error);
		if (error) return;
	}

	Connection::Connection(asio::ip::tcp::socket& socket, bool startActive) : _Socket(_Context), _ReceiveBuffer(1024), _SendQueue(), _IsActive(false), _IsClosed(false)
	{
		auto native = socket.native_handle();
		auto handle = socket.release();
		_Socket.assign(asio::ip::tcp::v4(), native);
		_Socket.set_option(asio::ip::tcp::no_delay(true));

		_Endpoint = _Socket.remote_endpoint();
	};
}