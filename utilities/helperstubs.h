#pragma once
#include <array>

namespace helper
{
	template<typename... T>
	constexpr auto make_array(T&&... elements)->std::array<typename std::decay<typename std::common_type<T...>::type>::type, sizeof...(elements)>
	{
		return std::array<typename std::decay<typename std::common_type<T...>::type>::type, sizeof...(elements)>{std::forward<T>(elements)...};
	}
}
