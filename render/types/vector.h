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
			Vector(const Vector<_Scalar, Dimension>& v) = default;
			Vector(Vector<_Scalar, Dimension>&& v) = default;
			Vector& operator=(const Vector<_Scalar, Dimension>& v) = default;
			Vector& operator=(Vector<_Scalar, Dimension>&& v) = default;
			~Vector() = default;
			_Scalar& operator[](const int comp) { return Matrix<_Scalar, Dimension, 1>::operator()(comp, 0); }
			_Scalar operator[](const int comp)const { return Matrix<_Scalar, Dimension, 1>::operator()(comp, 0); }
			_Scalar Dot(const Vector<_Scalar, Dimension>& v)const { return m_value.dot(v.m_value); }
			Vector<_Scalar, Dimension> Cross(const Vector<_Scalar, Dimension>& v)const { return Vector(m_value.cross(v.m_value)); }
		protected:
			Vector(Eigen::Matrix<_Scalar, Dimension, 1>&& v):Matrix<_Scalar, Dimension, 1>(std::forward<Eigen::Matrix<_Scalar, Dimension, 1>>(v)){}
		};

		using Vector4f = Vector<float, 4>;
		using Vector3f = Vector<float, 3>;
		using Vector2f = Vector<float, 2>;
		using Vector4i = Vector<int, 4>;
		using Vector3i = Vector<int, 3>;
		using Vector2i = Vector<int, 2>;
		template<typename _Scalar, int Dimension>
		using VectorList = std::vector<Vector<_Scalar, Dimension>, Eigen::aligned_allocator<Vector<_Scalar, Dimension>>>;
	}
}