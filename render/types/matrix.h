#pragma once
#include <iterator>
#include <vector>
#include <tuple>
#include <Eigen/Dense>
#include <Eigen/StdVector>

#define LIGHTNINGGE_ALIGNED_OPERATOR_NEW EIGEN_MAKE_ALIGNED_OPERATOR_NEW

namespace LightningGE
{
	namespace Render
	{
		template<typename _Scalar, int Rows, int Columns>
		class Matrix
		{
		public:
			EIGEN_MAKE_ALIGNED_OPERATOR_NEW
			Matrix();
			template<typename Iterable>
			Matrix(const Iterable& data, bool rowMajor=true, typename std::iterator_traits<decltype(std::cbegin(data))>::pointer=nullptr) 
			{ 
				static_assert(Rows > 0 && Columns > 0, "Rows and Columns must be possible integers!");
				Set(data, rowMajor); 
			}
			Matrix(const std::initializer_list<_Scalar>& data, bool rowMajor=true) 
			{ 
				static_assert(Rows > 0 && Columns > 0, "Rows and Columns must be possible integers!");
				Set(data, rowMajor); 
			}

			bool operator==(const Matrix<_Scalar, Rows, Columns>& m){return m_value == m.m_value;}
			bool operator!=(const Matrix<_Scalar, Rows, Columns>& m){return !(*this == m); }
			Matrix<_Scalar, Rows, Columns>& operator+=(const Matrix<_Scalar, Rows, Columns>& m)
			{
				m_value += m.m_value;
				return *this;
			}
			Matrix<_Scalar, Rows, Columns> operator+(const Matrix<_Scalar, Rows, Columns>& m)const
			{
				Matrix<_Scalar, Rows, Columns> result(*this);
				result += m;
				return result;
			}
			Matrix<_Scalar, Rows, Columns>& operator-=(const Matrix<_Scalar, Rows, Columns>& m)
			{
				m_value -= m.m_value;
				return *this;
			}
			Matrix<_Scalar, Rows, Columns> operator-(const Matrix<_Scalar, Rows, Columns>& m)const
			{
				Matrix<_Scalar, Rows, Columns> result(*this);
				result -= m;
				return result;
			}
			template<int Dimension>
			Matrix<_Scalar, Rows, Dimension>& operator*=(const Matrix<_Scalar, Columns, Dimension>& m)
			{
				static_assert(Dimension == Columns, "Only matrices that have the same dimension can be multiplied!");
				m_value *= m.m_value;
				return *this;
			}
			template<int Dimension>
			Matrix<_Scalar, Rows, Dimension> operator*(const Matrix<_Scalar, Columns, Dimension>& m)const
			{
				Matrix<_Scalar, Rows, Dimension> result;
				result.m_value = m_value * m.m_value;
				return result;
			}
			void TransposeInPlace() 
			{ 
				static_assert(Rows == Columns, "Only square matrices can transpose in place!");
				m_value.transposeInPlace();
			}
			template<int SubRows, int SubColumns> 
			Matrix<_Scalar, SubRows, SubColumns> SubMatrix(const int r, const int c)
			{
				static_assert(SubRows <= Rows && SubColumns <= Columns, "submatrix cannot have greater dimension than original matrix.");
				return Matrix<_Scalar, SubRows, SubColumns>(m_value.block<SubRows, SubColumns>(r, c));
			}
			Matrix<_Scalar, Columns, Rows> Transpose()const 
			{ 
				Matrix<_Scalar, Columns, Rows> result;
				result.m_value = m_value.transpose();
				return result;
			}
			template<typename Iterable>
			void Set(const Iterable& data, bool rowMajor=true);
			_Scalar& operator()(const int row, const int column) { return m_value(row, column); }
			_Scalar operator()(const int row, const int column)const { return m_value(row, column); }
			template<int _Rows = Rows>
			bool Invertible(typename std::enable_if<(_Rows <= 4), void*>::type = nullptr)const;
			template<int _Rows = Rows>
			bool Invertible(typename std::enable_if<(_Rows > 4), void*>::type = nullptr)const;
			Matrix<_Scalar, Rows, Columns> Inverse()const
			{
				static_assert(Rows == Columns, "Only square matrices are invertible!");
				return Matrix<_Scalar, Rows, Columns>(m_value.inverse());
			}
			static const Matrix<_Scalar, Rows, Columns> Identity()
			{
				static_assert(Rows == Columns, "Only square matrices have identity matrix!");
				return Matrix<_Scalar, Rows, Columns>(Eigen::Matrix<_Scalar, Rows, Columns>::Identity());
			}
		protected:
			template<typename _Scalar, int _Rows, int _Columns> friend class Matrix;
			Eigen::Matrix<_Scalar, Rows, Columns> m_value;
			Matrix(Eigen::Matrix<_Scalar, Rows, Columns>&& v):m_value(std::forward<Eigen::Matrix<_Scalar, Rows, Columns>>(v)){}
		};

		template<typename _Scalar, int Rows, int Columns>
		Matrix<_Scalar, Rows, Columns>::Matrix()
		{
			static_assert(Rows > 0 && Columns > 0, "Rows and Columns must be possible integers!");
			for (std::size_t i = 0;i < Rows;++i)
			{
				for (std::size_t j = 0; j < Columns; ++j)
				{
					m_value(i, j) = 0;
				}
			}
		}

		template<typename _Scalar, int Rows, int Columns>
		template<typename Iterable>
		void Matrix<_Scalar, Rows, Columns>::Set(const Iterable& data, bool rowMajor)
		{
			auto it = std::cbegin(data);
			if (rowMajor)
			{
				for (int i = 0;i < Rows;++i)
				{
					for (int j = 0;j < Columns;++j)
					{
						m_value(i, j) = *it++;
					}
				}
			}
			else
			{
				for (int i = 0; i < Columns; ++i)
				{
					for (int j = 0;j < Rows;++j)
					{
						m_value(j, i) = *it++;
					}
				}
			}
		}

		template<typename _Scalar, int Rows, int Columns>
		template<int _Rows>
		bool Matrix<_Scalar, Rows, Columns>::Invertible(typename std::enable_if<(_Rows <= 4), void*>::type)const
		{
			if (Rows != Columns)
				return false;

			Eigen::Matrix<_Scalar, Rows, Columns> invMatrix;
			_Scalar determinant;
			bool invertible{ false };
			m_value.computeInverseAndDetWithCheck(invMatrix, determinant, invertible);
			return invertible;
		}

		template<typename _Scalar, int Rows, int Columns>
		template<int _Rows>
		bool Matrix<_Scalar, Rows, Columns>::Invertible(typename std::enable_if < (_Rows > 4), void*>::type)const
		{
			if (Rows != Columns)
				return false;

			Eigen::FullPivLU<Eigen::Matrix<_Scalar, Rows, Columns>> lu(m_value);
			return lu.isInvertible();
		}

		using Matrix4x4f = Matrix<float, 4, 4>;
		using Matrix3x3f = Matrix<float, 3, 3>;
		using Matrix2x2f = Matrix<float, 2, 2>;

		template<typename _Scalar, int Rows, int Columns>
		using MatrixList = std::vector<Matrix<_Scalar, Rows, Columns>, Eigen::aligned_allocator<Matrix<_Scalar, Rows, Columns>>>;
	}
}
