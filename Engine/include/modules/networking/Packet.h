#pragma once
#include <stdint.h>
#include <vector>
#include <unordered_map>
#include <functional>

#include "NetDefs.h"
#include "../../utility/CCXType.h"
#include "../../utility/CCXSerialisation.h"
#include "../../utility/CCXMemory.h"

namespace Thera::Net
{
	typedef uint16_t PacketID;
	typedef uint16_t PacketSize;

	/// <summary>
	/// Deprecated. To be removed when all references to it are replaced.
	/// </summary>
	enum class PacketType : PacketID
	{
		Connect = 0,
		Disconnect,
		Heartbeat
	};

	inline PacketID FirstUserID = static_cast<PacketID>(PacketType::Heartbeat) + 1;

	/// <summary>
	/// Packet defining data found before all packet data.
	/// </summary>
	class PacketHeader
	{
	public:
		static const size_t HEADER_SIZE = sizeof(PacketID) + sizeof(PacketSize);
		PacketID ID;
		PacketSize Size;

		PacketHeader(PacketID id) : ID(id), Size(0) {}
	};

	class Packet
	{
	private:
		PacketHeader _Header;
		std::vector<unsigned char> _Data;
		
	public:
		PacketID ID();
		/// <summary>
		/// Get the data size within this packet.
		/// </summary>
		/// <returns></returns>
		PacketSize Size();
		/// <summary>
		/// Get the size of this packet including the header.
		/// </summary>
		/// <returns></returns>
		size_t FullSize();

		Packet(PacketID id) : _Header(id) {}
		template<typename Enum>
		Packet(Enum idEnum) : _Header(static_cast<PacketID>(idEnum))
		{
			static_assert(CCX::is_explicitly_convertible<Enum, PacketID>::value, "Type must be convertible to PacketID");
		}
		/// <summary>
		/// Create packet from raw data including ID and size.
		/// </summary>
		/// <param name="data"></param>
		/// <param name="dataSize"></param>
		Packet(char* data, size_t dataSize);
		/// <summary>
		/// Create a packet from a reader, assuming the reader is at the start of the packet header.
		/// </summary>
		Packet(CCX::MemoryReader& reader);

		/// <summary>
		/// Get the buffer sequence for this packet.
		/// </summary>
		/// <returns></returns>
		std::array<asio::const_buffer, 2> GetBuffer();
		void GetBuffer(OUT std::vector<asio::const_buffer>& addTo);

		/// <summary>
		/// Write to this packet's internal data.
		/// </summary>
		/// <param name="data"></param>
		/// <param name="dataSize"></param>
		void Write(const void* data, size_t dataSize);
		template<typename T, CCX::if_not_t<std::is_standard_layout_v<T>> = 0>
		void Write(const T data)
		{
			CCX::Serialise<T, Packet>(data, *this);
		}
		template<typename T, CCX::if_t<std::is_standard_layout_v<T>> = 0>
		void Write(const T data)
		{
			*this << data;
		}
		/// <summary>
		/// Get a reader for this packet's data.
		/// </summary>
		/// <returns></returns>
		CCX::MemoryReader GetReader();

		template<typename T>
		friend Packet& operator<<(Packet& packet, const T& data)
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
	/// <summary>
	/// A group of unique packets bundled together.
	/// </summary>
	class AggregatePacket
	{
	public:
		static const size_t HEADER_SIZE = sizeof(PacketSize) * 2;
		PacketSize Count;
		PacketSize Size;
		std::vector<Packet> Packets;

		/// <summary>
		/// Create a new aggregate with a reserved size.
		/// </summary>
		/// <param name="expected"></param>
		AggregatePacket(size_t expected) : Count(0), Size(0)
		{
			Packets.reserve(expected);
		}

		void AddPacket(Packet& packet);
		/// <summary>
		/// Add a packet from raw data including ID and size.
		/// </summary>
		/// <param name="data"></param>
		/// <param name="dataSize"></param>
		void AddPacket(char* data, size_t dataSize);
		/// <summary>
		/// Add a packet from a memory reader, assuming the reader is at the start of the packet header.
		/// </summary>
		/// <param name="reader"></param>
		void AddPacket(CCX::MemoryReader& reader);

		/// <summary>
		/// Get the buffer sequence for this aggregate, including all its contained packets.
		/// </summary>
		/// <returns></returns>
		std::vector<asio::const_buffer> GetBuffer();
	};

	class Connection;

	using PacketHandler = std::function<void(Connection&, Packet*)>;

	/// <summary>
	/// Register a packet handler for the specified ID.
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