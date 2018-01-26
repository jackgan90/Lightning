#pragma once
#include "vector.h"

namespace LightningGE
{
	namespace Render
	{
		template<typename _Scalar>
		class Rect : public Vector<_Scalar, 4>
		{
		public:
			Rect(const _Scalar left, const _Scalar top, const _Scalar width, const _Scalar height):Vector<_Scalar, 4>({left, top, width, height}){}
			template<typename Iterable>
			Rect(const Iterable& data, typename std::iterator_traits<decltype(std::cbegin(data))>::pointer=nullptr) : Vector<_Scalar, 4>(data){}
			Rect(const std::initializer_list<_Scalar>& data) : Vector<_Scalar, 4>(data){}
			_Scalar& left() { return Vector<_Scalar, 4>::operator [](0); }
			_Scalar& top() { return Vector<_Scalar, 4>::operator [](1); }
			_Scalar& width() { return Vector<_Scalar, 4>::operator [](2); }
			_Scalar& height() { return Vector<_Scalar, 4>::operator [](3); }
			const _Scalar right() { return left() + width(); }
			const _Scalar bottom() { return top() + height(); }
		};

		using RectI = Rect<int>;
		using RectF = Rect<float>;
	}
}