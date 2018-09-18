#pragma once
#include "plainobject.h"

namespace Lightning
{
	namespace Render
	{
		template<typename T>
		struct Rect : Foundation::PlainObject<Rect<T>>
		{
		public:
			Rect() : 
			Rect(T l, T t, T w, T h) : left(l), top(t), width(w), height(h){}
			T right()const { return left + width; }
			T bottom()const { return top + height; }
			T left, top, width, height;
		};

		using RectI = Rect<int>;
		using RectF = Rect<float>;
		template<typename T>
		using RectList = std::vector<Rect<T>>;
		using RectIList = RectList<int>;
		using RectFList = RectList<float>;
	}
}