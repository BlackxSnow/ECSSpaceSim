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
}