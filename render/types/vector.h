#pragma once
#include "matrix.h"

namespace LightningGE
{
	namespace Render
	{
		template<typename _Scalar, int Dimension>
		class Vector : public Matrix<_Scalar, Dimension, 1>
		{
		public:
			template<typename Iterable>
			Vector(const Iterable& data, typename std::iterator_traits<decltype(std::cbegin(data))>::pointer=nullptr) : Matrix<_Scalar, Dimension, 1>(data, true){}
			Vector(const std::initializer_list<_Scalar>& data) : Matrix<_Scalar, Dimension, 1>(data, true){}
			_Scalar& operator[](const int comp) { return Matrix<_Scalar, Dimension, 1>(comp, 0); }
		};

		using Vector4f = Vector<float, 4>;
		using Vector3f = Vector<float, 3>;
		using Vector2f = Vector<float, 2>;
		using Vector4i = Vector<int, 4>;
		using Vector3i = Vector<int, 3>;
		using Vector2i = Vector<int, 2>;
		template<typename _Scalar, int Dimension>
		using VectorList = MatrixList<_Scalar, Dimension, 1>;
	}
}