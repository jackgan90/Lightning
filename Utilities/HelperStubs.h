#pragma once
#include "Container.h"

namespace helper
{
	using Lightning::Foundation::Container;
	template<typename... T>
	constexpr auto make_array(T&&... elements)->Container::Array<typename std::decay<typename std::common_type<T...>::type>::type, sizeof...(elements)>
	{
		return Container::Array<typename std::decay<typename std::common_type<T...>::type>::type, sizeof...(elements)>{std::forward<T>(elements)...};
	}
}
