#pragma once

#include <type_traits>

namespace CCX
{
	template<typename T>
	struct false_type : std::false_type {};

	// Template conditionals
	template<bool V, typename T = int>
	using if_t = typename std::enable_if<V, T>::type;
	
	template<bool V>
	using if_not_t = if_t <false == V>;

    /// <summary>
    /// Whether 'From' can be explicitly cast to 'To'
    /// </summary>
    /// <typeparam name="From"></typeparam>
    /// <typeparam name="To"></typeparam>
    template<typename From, typename To>
    struct is_explicitly_convertible
    {
        template<typename T>
        static void f(T);

        template<typename F, typename T>
        static constexpr auto test(int) ->
            decltype(f(static_cast<T>(std::declval<F>())), true) {
            return true;
        }

        template<typename F, typename T>
        static constexpr auto test(...) -> bool {
            return false;
        }

        static bool const value = test<From, To>(0);
    };

	template<typename, typename = void>
    struct has_write : std::false_type {};

	/// <summary>
	/// Whether 'T' has a 'Write' method taking void* and size_t parameters.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<typename T> 
    struct has_write<T, decltype(std::declval<T>().Write(std::declval<void*>(), std::declval<size_t>()))> : std::true_type {};

    //template<typename T>
    //using has_write_v = has_write<T>::value;
}