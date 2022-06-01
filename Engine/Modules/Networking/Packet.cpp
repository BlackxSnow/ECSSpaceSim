#include "Packet.h"
#include "../../utility/ConsoleLogging.h"

namespace Thera::Net
{
	std::unordered_map<PacketID, PacketHandler> _PacketHandlers;

	PacketID Packet::ID()
	{
		return _Header.ID;
	}

	PacketSize Packet::Size()
	{
		return _Header.Size;
	}

	size_t Packet::FullSize()
	{
		return static_cast<size_t>(Size()) + PacketHeader::HEADER_SIZE;
	}

	Packet::Packet(char* data, size_t dataSize) : _Header(*reinterpret_cast<PacketID*>(data))
	{
		data += sizeof(PacketID);
		dataSize -= sizeof(PacketID);

		PacketSize packetSize = *reinterpret_cast<PacketSize*>(data);
		data += sizeof(PacketSize);
		dataSize -= sizeof(PacketSize);

		if (dataSize != packetSize)
		{
			LogError("Data and packet size mismatch: Packet reports " + std::to_string(packetSize) + " but received: " + std::to_string(dataSize) + " after header.", false);
			return;
		}

		Write(data, dataSize);
	}
	Packet::Packet(CCX::MemoryReader& reader) : _Header(reader.Read<PacketID>())
	{
		PacketSize dataSize = reader.Read<PacketSize>();
		size_t remaining = reader.Remaining();
		if (dataSize > remaining)
		{
			LogError("Packet size mismatch: Packet reports " + std::to_string(dataSize) + " but received: " + std::to_string(reader.Remaining()) + " after header.", false);
			return;
		}
		
		Write(reader.Current(), dataSize);
		reader.Skip(dataSize);
	}
	std::array<asio::const_buffer, 2> Packet::GetBuffer()
	{
		return std::array<asio::const_buffer, 2> {
			asio::buffer(this, PacketHeader::HEADER_SIZE),
				asio::buffer(_Data.data(), _Data.size())
		};
	}
	void Packet::GetBuffer(OUT std::vector<asio::const_buffer>& addTo)
	{
		addTo.emplace_back(this, PacketHeader::HEADER_SIZE);
		if (_Data.size() > 0)
		{
			addTo.emplace_back(_Data.data(), _Data.size());
		}
	}

	void Packet::Write(void* data, size_t dataSize)
	{
		if (dataSize <= 0) return;
		size_t startOffset = _Data.size();
		_Data.resize(startOffset + dataSize);
		memcpy(&_Data[startOffset], data, dataSize);
		_Header.Size = _Data.size();
	}
	
	void RegisterPacket(PacketID id, PacketHandler handler)
	{
		if (_PacketHandlers.find(id) != _PacketHandlers.end())
		{
			LogWarning("Packet ID " + std::to_string(id) + " re-registered and overwritten.");
		}

		_PacketHandlers[id] = handler;
	}

	bool TryGetPacketHandler(PacketID id, PacketHandler*& handler)
	{
		auto result = _PacketHandlers.find(id);

		if (result == _PacketHandlers.end())
		{
			handler = nullptr;
			return false;
		}

		handler = &result->second;
		return true;
	}
	void AggregatePacket::AddPacket(Packet& packet)
	{
		Packets.push_back(packet);
		Count++;
		Size += packet.FullSize();
	}
	void AggregatePacket::AddPacket(char* data, size_t dataSize)
	{
		Packets.emplace_back(data, dataSize);
		Count++;
		Size += dataSize;
	}
	void AggregatePacket::AddPacket(CCX::MemoryReader& reader)
	{
		Packets.emplace_back(reader);
		Count++;
		Size += (Packets.end() - 1)->Size();
	}

	std::vector<asio::const_buffer> AggregatePacket::GetBuffer()
	{
		std::vector<asio::const_buffer> buffer;
		buffer.reserve(Count * 2 + 1);
		buffer.emplace_back(this, HEADER_SIZE);
		for (int i = 0; i < Count; i++)
		{
			Packets[i].GetBuffer(buffer);
		}
		return buffer;
	}
}
