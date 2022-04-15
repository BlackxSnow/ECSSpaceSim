#pragma once

namespace CCX
{
	template<typename T>
	struct false_type : std::false_type {};

	// Template conditionals
	template<bool V, typename T = int>
	using if_t = typename std::enable_if<V, T>::type;
	
	template<bool V>
	using if_not_t = if_t <false == V>;
}