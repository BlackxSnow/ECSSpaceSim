#include "Packet.h"
#include "../../utility/ConsoleLogging.h"

namespace ecse::Networking
{
	std::unordered_map<PacketID, PacketHandler> _PacketHandlers;

	PacketID Packet::ID()
	{
		return _Header.ID;
	}

	uint32_t Packet::Size()
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

		uint32_t packetSize = *reinterpret_cast<uint32_t*>(data);
		data += sizeof(uint32_t);
		dataSize -= sizeof(uint32_t);

		if (dataSize != packetSize)
		{
			LogError("Data and packet size mismatch: Packet reports " + std::to_string(packetSize) + " but received: " + std::to_string(dataSize) + " after header.", false);
			return;
		}

		Write(data, dataSize);
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
}
