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
			Vector(const Matrix<_Scalar, 1, Dimension>& m): Matrix<_Scalar, Dimension, 1>(m.m_value){}
			Vector(Matrix<_Scalar, 1, Dimension>&& m): Matrix<_Scalar, Dimension, 1>(std::move(m.m_value)){}
			template<typename S, int Rows, int Columns>
			Vector(const Matrix<S, Rows, Columns>& v) : Matrix<_Scalar, Dimension, 1>(v){}
			template<typename S, int Rows, int Columns>
			Vector(Matrix<S, Rows, Columns>&& v) : Matrix<_Scalar, Dimension, 1>(std::move(v)){}
			template<typename ScalarPointerType, typename SizeType, typename = 
				typename std::enable_if<std::is_integral<typename std::decay<SizeType>::type>::value	//ensure the second argument is an integral type
				&& std::is_pointer<typename std::decay<ScalarPointerType>::type>::value	//ensure the first argument is a pointer
				&& std::is_convertible<typename std::decay<typename std::remove_pointer<ScalarPointerType>::type>::type, //ensure data pointed to by first argument can be converted to _Scalar
				typename std::decay<_Scalar>::type>::value
				>::type
			>
			Vector(const ScalarPointerType arr, const SizeType size):Matrix<_Scalar, Dimension, 1>(arr, size){}
			bool operator==(const Matrix<_Scalar, 1, Dimension>& mv)const
			{
				return m_value.isApprox(mv.m_value.transpose());
			}
			bool operator!=(const Matrix<_Scalar, 1, Dimension>& mv)const
			{
				return !(*this == mv);
			}
			bool operator==(const Vector<_Scalar, Dimension>& v)const
			{
				return m_value.isApprox(v.m_value);
			}
			bool operator!=(const Vector<_Scalar, Dimension>& v)const
			{
				return !(*this == mv);
			}
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