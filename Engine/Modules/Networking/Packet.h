#pragma once
#include <stdint.h>
#include <vector>
#include <unordered_map>
#include <functional>

#include "NetDefs.h"
#include "../../utility/CCXType.h"

namespace ecse::Networking
{
	typedef uint32_t PacketID;

	enum class PacketType : PacketID
	{
		Connect = 0,
		Disconnect,
		Heartbeat
	};

	inline PacketID FirstUserID = static_cast<PacketID>(PacketType::Heartbeat) + 1;

	class PacketHeader
	{
	public:
		static const uint32_t HEADER_SIZE = sizeof(PacketID) + sizeof(uint32_t);
		PacketID ID;
		uint32_t Size;

		PacketHeader(PacketID id) : ID(id), Size(0) {}
	};

	class Packet
	{
	private:
		PacketHeader _Header;
		std::vector<unsigned char> _Data;
		
	public:
		PacketID ID();
		uint32_t Size();
		size_t FullSize();

		Packet(PacketID id) : _Header(id) {}
		template<typename Enum>
		Packet(Enum idEnum) : _Header(static_cast<PacketID>(idEnum))
		{
			static_assert(CCX::is_explicitly_convertible<Enum, PacketID>::value, "Type must be convertible to PacketID");
		}
		Packet(char* data, size_t dataSize);

		std::array<asio::const_buffer, 2> GetBuffer()
		{
			return std::array<asio::const_buffer, 2> {
				asio::buffer(this, PacketHeader::HEADER_SIZE),
					asio::buffer(_Data.data(), _Data.size())
			};
		}

		void Write(void* data, size_t dataSize);

		template<typename T>
		friend Packet& operator<<(Packet& packet, T& data)
		{
			static_assert(std::is_standard_layout_v<T>, "Data is not trivial.");

			size_t startOffset = packet._Data.size();
			packet._Data.resize(startOffset + sizeof(T));
			memcpy(&packet._Data[startOffset], &data, sizeof(T));
			
			packet._Header.Size = packet._Data.size();
			return packet;
		}
		
		template<typename T>
		friend Packet& operator>>(Packet& packet, T& data)
		{
			static_assert(std::is_standard_layout_v<T>, "Data is not trivial.");

			size_t startOffset = packet._Data.size() - sizeof(T);
			memcpy(&data, &packet._Data[startOffset], sizeof(T));
			packet._Data.resize(startOffset);

			packet._Header.Size = packet._Data.size();
			return packet;
		}

	};

	class ReceivedPacket
	{
	public:
		asio::ip::udp::endpoint Source;
		Packet packet;

		ReceivedPacket(asio::ip::udp::endpoint& source, char* data, size_t size) : Source(source), packet(data, size) {}
	};

	using PacketHandler = std::function<void(ReceivedPacket*)>;

	void RegisterPacket(PacketID id, PacketHandler handler);
	template<typename Enum>
	void RegisterPacket(Enum idEnum, PacketHandler handler)
	{
		static_assert(CCX::is_explicitly_convertible<Enum, PacketID>::value, "Given type must be convertible to PacketID.");
		RegisterPacket(static_cast<PacketID>(idEnum), handler);
	}

	bool TryGetPacketHandler(PacketID id, PacketHandler*& handler);
}