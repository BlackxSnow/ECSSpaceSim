#pragma once

#include <string>
#include "CCXType.h"
#include "CCXDefs.h"

namespace CCX
{
	template<typename T>
	T Deserialise(void* data, OUT size_t* size)
	{
		static_assert(CCX::false_type<T>, "Deserialisation of type is unhandled.");
	}
	
	template<>
	inline std::string Deserialise<std::string>(void* data, OUT size_t* size)
	{
		*size = *reinterpret_cast<uint32_t*>(data);
		data = (char*)data + sizeof(uint32_t);
		return std::string((char*)data, *size);
	}

	template<typename T, typename Writable, if_not_t<has_write<Writable>::value> = 0>
	void Serialise(const T obj, Writable& writable)
	{
		static_assert(CCX::false_type<T>(), "Given type is not writable.");
	}

	template<typename T, typename Writable, if_t<has_write<Writable>::value> = 0>
	inline void Serialise(const T obj, Writable& writable)
	{
		static_assert(CCX::false_type<T>(), "Serialisation of type is unhandled.");
	}

	template<typename T, typename Writable, if_t<has_write<Writable>::value> = 0>
	inline void Serialise(const std::string obj, Writable& writable)
	{
		uint32_t length = obj.length();
		writable.Write(&length, sizeof(uint32_t));
		writable.Write(obj.c_str(), length);
	}
}