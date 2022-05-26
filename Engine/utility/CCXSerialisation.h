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
}