#pragma once

#include "CCXType.h"
#include "CCXDefs.h"
#include "CCXSerialisation.h"
#include "ConsoleLogging.h"

namespace CCX
{
	/// <summary>
	/// Wrapper for raw memory allowing reading as if it were a stream.
	/// </summary>
	class MemoryReader
	{
	private:
		byte* _Data;
		size_t _Size;
		size_t _Position;

	public:
		MemoryReader(byte* data, size_t size) : _Data(data), _Size(size), _Position(0) {}

		byte* Current() { return _Data + _Position; }
		size_t Remaining() { return _Size - _Position; }
		void Skip(size_t bytes) { _Position += bytes; }

		template<typename T, if_not_t<std::is_standard_layout<T>::value> = 0>
		T Read()
		{
			size_t* size = nullptr;
			T value = CCX::Deserialise<T>(Current(), size);
			_Position += *size;
			return value;
		}

		template<typename T, if_t<std::is_standard_layout<T>::value> = 0>
		T Read()
		{
			if (sizeof(T) > Remaining())
			{
				LogError("Not enough data to read.", true);
			}
			T value{};
			memcpy(&value, _Data + _Position, sizeof(T));
			_Position += sizeof(T);
			return value;
		}
	};
}