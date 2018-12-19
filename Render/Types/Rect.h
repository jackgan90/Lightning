#pragma once
#include <type_traits>

namespace Lightning
{
	namespace Render
	{
		template<typename T>
		struct Rect
		{
			static_assert(std::is_pod<T>::value, "T is not a POD type.");
			T right()const { return left + width; }
			T bottom()const { return top + height; }
			T left, top, width, height;
		};

		using RectI = Rect<int>;
		using RectF = Rect<float>;
		static_assert(std::is_pod<RectI>::value, "RectI is not a POD type.");
		static_assert(std::is_pod<RectF>::value, "RectF is not a POD type.");
	}
}