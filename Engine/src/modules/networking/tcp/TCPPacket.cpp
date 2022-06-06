#include <modules/networking/tcp/TCPPacket.h>

namespace Thera::Net::tcp
{
	std::unordered_map<PacketID, PacketHandler> _PacketHandlers;
	
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