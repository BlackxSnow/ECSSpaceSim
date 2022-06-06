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
				return;
			}
			if (error.value() == asio::error::eof)
			{
				LogWarning((std::ostringstream() << "Connection " << _Endpoint << " closed connection (EOF).").str());
				return;
			}
			if (error.value() == asio::error::connection_reset)
			{
				LogWarning((std::ostringstream() << "Connection " << _Endpoint << " closed connection (Reset).").str());
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
				LogWarning((std::ostringstream() << "Received unhandled packet id '" << std::to_string(packet.ID()) << "' from " << _Endpoint).str(), false);
				continue;
			}
			(*handler)(*this, &packet);
		}
		LogInfo((std::ostringstream() << "Received " << bytesTransferred << " bytes from " << _Endpoint).str());
		Listen();
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
			LogInfo((std::ostringstream() << "Sending packets to: " << _Socket.remote_endpoint()).str());
			size_t sent = _Socket.send(buffer);
			if (sent != size)
			{
				LogError("Sent " + std::to_string(sent) + " of " + std::to_string(size) + " bytes for aggregate with count " + std::to_string(aggregate->Count), false);
			}
			_SendQueue.pop();
		}
	}
}