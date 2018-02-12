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
			Vector() : Matrix < _Scalar, Dimension, 1>({0, 0, 0}) {}
			Vector(const std::initializer_list<_Scalar>& data) : Matrix<_Scalar, Dimension, 1>(data){}
			Vector(const Vector<_Scalar, Dimension>& v) = default;
			Vector(Vector<_Scalar, Dimension>&& v) = default;
			Vector& operator=(const Vector<_Scalar, Dimension>& v) = default;
			Vector& operator=(Vector<_Scalar, Dimension>&& v) = default;
			~Vector() = default;
			_Scalar& operator[](const int comp) { return Matrix<_Scalar, Dimension, 1>::operator()(comp, 0); }
			_Scalar operator[](const int comp)const { return Matrix<_Scalar, Dimension, 1>::operator()(comp, 0); }
			_Scalar Dot(const Vector<_Scalar, Dimension>& v)const { return m_value.dot(v.m_value); }
			Vector<_Scalar, Dimension> Cross(const Vector<_Scalar, Dimension>& v)const { return Vector(m_value.cross(v.m_value)); }
			_Scalar Length(){ return m_value.norm(); }
			void Normalize() { m_value.normalize(); }
			Vector<_Scalar, Dimension> Normalized()const { return Vector<_Scalar, Dimension>(m_value.normalized()); }
			static const Vector<_Scalar, Dimension> Zero()
			{
				static const Vector<_Scalar, Dimension> v;
				return v;
			}
		protected:
			Vector(Eigen::Matrix<_Scalar, Dimension, 1>&& v):Matrix<_Scalar, Dimension, 1>(std::move(v)){}
			Vector(const Eigen::Matrix<_Scalar, Dimension, 1>& v):Matrix<_Scalar, Dimension, 1>(v){}
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