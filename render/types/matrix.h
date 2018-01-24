#pragma once
#include <Eigen/Dense>

namespace LightningGE
{
	namespace Render
	{
		template<typename _Scalar, int Rows, int Columns, bool RowMajor=true>
		class Matrix
		{
		public:
			EIGEN_MAKE_ALIGNED_OPERATOR_NEW
			Matrix();
			void TransposeInPlace()
			{
				m_value.transposeInPlace();
			}
			Matrix<_Scalar, Columns, Rows> Transpose()const
			{
				return m_value.transpose();
			}
			void Set(const _Scalar* data);
			const _Scalar operator()(const int row, const int column)
			{
				return m_value(row, column);
			}
		private:
			Eigen::Matrix<_Scalar, Rows, Columns> m_value;
		};

		template<typename _Scalar, int Rows, int Columns, bool RowMajor>
		Matrix<_Scalar, Rows, Columns, RowMajor>::Matrix()
		{
			for (std::size_t i = 0;i < Rows;++i)
			{
				for (std::size_t j = 0; j < Columns; ++j)
				{
					m_value(i, j) = 0;
				}
			}
		}

		template<typename _Scalar, int Rows, int Columns, bool RowMajor>
		void Matrix<_Scalar, Rows, Columns, RowMajor>::Set(const _Scalar* data)
		{
			int current = 0;
			if (RowMajor)
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
