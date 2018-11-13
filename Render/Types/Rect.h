#pragma once
#include "Container.h"
#include "PlainObject.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::Container;
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
		using RectList = Container::Vector<Rect<T>>;
		using RectIList = RectList<int>;
		using RectFList = RectList<float>;
	}
}