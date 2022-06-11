#pragma once
#include "../Packet.h"
#include "TCPConnection.h"

namespace Thera::Net::tcp
{
	using PacketHandler = std::function<void(Connection&, Packet*)>;

	/// <summary>
	/// Register a packet handler for a specific packet type.
	/// </summary>
	/// <param name="id"></param>
	/// <param name="handler"></param>
	void RegisterPacket(PacketID id, PacketHandler handler);
	template<typename Enum>
	void RegisterPacket(Enum idEnum, PacketHandler handler)
	{
		static_assert(CCX::is_explicitly_convertible<Enum, PacketID>::value, "Given type must be convertible to PacketID.");
		RegisterPacket(static_cast<PacketID>(idEnum), handler);
	}

	/// <summary>
	/// Attempt to retrieve an existing packet handler.
	/// </summary>
	/// <param name="id"></param>
	/// <param name="handler"></param>
	/// <returns></returns>
	bool TryGetPacketHandler(PacketID id, PacketHandler*& handler);
}