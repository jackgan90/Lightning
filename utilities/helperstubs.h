#pragma once
#include "container.h"

namespace helper
{
	using Lightning::Foundation::container;
	template<typename... T>
	constexpr auto make_array(T&&... elements)->container::array<typename std::decay<typename std::common_type<T...>::type>::type, sizeof...(elements)>
	{
		return container::array<typename std::decay<typename std::common_type<T...>::type>::type, sizeof...(elements)>{std::forward<T>(elements)...};
	}
}
