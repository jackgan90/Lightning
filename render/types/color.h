#pragma once
#include <cstdint>
#include "vector.h"

namespace LightningGE
{
	namespace Render
	{
		template<typename _Scalar>
		class Color : public Vector<_Scalar, 4>
		{
		public:
			Color(const _Scalar r, const _Scalar g, const _Scalar b, const _Scalar a):Vector<_Scalar, 4>({r, g, b, a}){}
			template<typename Iterable>
			Color(const Iterable& data, typename std::iterator_traits<decltype(std::cbegin(data))>::pointer=nullptr) : Vector<_Scalar, 4>(data){}
			Color(const std::initializer_list<_Scalar>& data) : Vector<_Scalar, 4>(data){}
			_Scalar& r() { return Vector<_Scalar, 4>::operator [](0); }
			_Scalar r()const { return Vector<_Scalar, 4>::operator [](0); }
			_Scalar& g() { return Vector<_Scalar, 4>::operator [](1); }
			_Scalar g()const { return Vector<_Scalar, 4>::operator [](1); }
			_Scalar& b() { return Vector<_Scalar, 4>::operator [](2); }
			_Scalar b()const { return Vector<_Scalar, 4>::operator [](2); }
			_Scalar& a() { return Vector<_Scalar, 4>::operator [](3); }
			_Scalar a()const { return Vector<_Scalar, 4>::operator [](3); }
		};

		using ColorF = Color<float>;
		using Color32 = Color<std::uint32_t>;
	}
}