#pragma once
#include <Eigen/Dense>

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
			Matrix(const _Scalar* data, bool rowMajor) { Set(data, rowMajor); }
			bool operator==(const Matrix<_Scalar, Rows, Columns>& m){return m_value == m.m_value;}
			bool operator!=(const Matrix<_Scalar, Rows, Columns>& m){return !(*this == m); }
			Matrix<_Scalar, Rows, Columns> operator+(const Matrix<_Scalar, Rows, Columns>& m)const
			{
				Matrix<_Scalar, Rows, Columns> result(*this);
				result.m_value += m.m_value;
				return result;
			}
			Matrix<_Scalar, Rows, Columns> operator-(const Matrix<_Scalar, Rows, Columns>& m)const
			{
				Matrix<_Scalar, Rows, Columns> result(*this);
				result.m_value -= m.m_value;
				return result;
			}
			template<int Dimension>
			Matrix<_Scalar, Rows, Dimension> operator*(const Matrix<_Scalar, Columns, Dimension>& m)const
			{
				Matrix<_Scalar, Rows, Dimension> result(*this);
				result.m_value *= m.m_value;
				return result;
			}
			void TransposeInPlace() { m_value.transposeInPlace();}
			Matrix<_Scalar, Columns, Rows> Transpose()const { return m_value.transpose(); }
			void Set(const _Scalar* data, bool rowMajor);
			const _Scalar operator()(const int row, const int column) { return m_value(row, column); }
		private:
			Eigen::Matrix<_Scalar, Rows, Columns> m_value;
		};

		template<typename _Scalar, int Rows, int Columns>
		Matrix<_Scalar, Rows, Columns>::Matrix()
		{
			for (std::size_t i = 0;i < Rows;++i)
			{
				for (std::size_t j = 0; j < Columns; ++j)
				{
					m_value(i, j) = 0;
				}
			}
		}

		template<typename _Scalar, int Rows, int Columns>
		void Matrix<_Scalar, Rows, Columns>::Set(const _Scalar* data, bool rowMajor)
		{
			int current = 0;
			if (rowMajor)
			{
				for (int i = 0;i < Rows;++i)
				{
					for (int j = 0;j < Columns;++j)
					{
						m_value(i, j) = data[current++];
					}
				}
			}
			else
			{
				for (int i = 0; i < Columns; ++i)
				{
					for (int j = 0;j < Rows;++j)
					{
						m_value(j, i) = data[current++];
					}
				}
			}
		}
	}
}
