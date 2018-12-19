#pragma once
#include <cstdint>
#include "PlainObject.h"

namespace Lightning
{
	namespace Render
	{
		template<typename T>
		struct Color
		{
			static_assert(std::is_pod<T>::value, "T is not a POD type.");
			T r, g, b, a;
		};

		using ColorF = Color<float>;
		using Color32 = Color<std::uint8_t>;
		static_assert(std::is_pod<ColorF>::value, "ColorF is not a POD type.");
		static_assert(std::is_pod<Color32>::value, "Color32 is not a POD type.");
	}
}