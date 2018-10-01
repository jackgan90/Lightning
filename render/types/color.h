#pragma once
#include <cstdint>
#include "plainobject.h"

namespace Lightning
{
	namespace Render
	{
		template<typename T>
		struct Color : Foundation::PlainObject<Color<T>>
		{
			Color() : r(0), g(0), b(0), a(0){}
			Color(T _r, T _g, T _b, T _a) : r(_r), g(_g), b(_b), a(_a){}
			T r, g, b, a;
		};

		using ColorF = Color<float>;
		using Color32 = Color<std::uint8_t>;
	}
}