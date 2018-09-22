#pragma once
#include <type_traits>
#include "vector.h"

namespace Lightning
{
	namespace Foundation
	{
		namespace Math
		{
			//column major matrix
			template<typename Derived, typename T>
			struct MatrixBase : PlainObject<Derived>
			{
				void SetZero()
				{
					std::memset(this, 0, sizeof(Derived));
				}

				void SetIdentity()
				{
					std::memset(this, 0, sizeof(Derived));
					Derived* const pDerived = reinterpret_cast<Derived* const>(this);
					for (int i = 0; i < Derived::Order; ++i)
						pDerived->m[CELL_INDEX(i, i)] = 1;
				}

				Derived operator*(const Derived& other)const
				{
					Derived mat;
					const Derived *const pDerived = reinterpret_cast<const Derived* const>(this);
					for (int i = 0;i < Derived::Order;++i)
					{
						for (int j = 0; j < Derived::Order; ++j)
						{
							unsigned idx = CELL_INDEX(i, j);
							mat.m[idx] = 0;
							for (int k = 0; k < Derived::Order; ++k)
								mat.m[idx] += pDerived->m[CELL_INDEX(i, k)] * other.m[CELL_INDEX(k, j)];
						}
					}
					return mat;
				}
				
				static inline unsigned CELL_INDEX(unsigned row, unsigned col) { return col * Derived::Order + row; }
			};

			template<typename T>
			struct Matrix3 : MatrixBase<Matrix3<T>, T>
			{
				static constexpr unsigned Order = 3;
				void SetRow(const Vector3<T>& v, unsigned row) { SetMatrixRow(*this, v, row); }
				void SetColumn(const Vector3<T>& v, unsigned col) { SetMatrixColumn(*this, v, col); }
				T m[Order * Order];
			};

			template<typename T>
			struct Matrix4 : MatrixBase<Matrix4<T>, T>
			{
				static constexpr unsigned Order = 4;
				void SetRow(const Vector4<T>& v, unsigned row) { SetMatrixRow(*this, v, row); }
				void SetColumn(const Vector4<T>& v, unsigned col) { SetMatrixColumn(*this, v, col); }
				T m[Order * Order];
			};

			template<typename MatrixType, typename VectorType>
			void SetMatrixColumn(MatrixType& mat, const VectorType& v, unsigned col)
			{
				static_assert(MatrixType::Order == VectorType::Order, "vector order must match matrix order!");
				if (col >= 0 && col < MatrixType::Order)
				{
					for (int i = 0; i < MatrixType::Order; ++i)
					{
						mat.m[MatrixType::CELL_INDEX(i, col)] = v[i];
					}
				}
			}

			template<typename MatrixType, typename VectorType>
			void SetMatrixRow(MatrixType& mat, const VectorType& v, unsigned row)
			{
				static_assert(MatrixType::Order == VectorType::Order, "vector order must match matrix order!");
				if (row >= 0 && row < MatrixType::Order)
				{
					for (int i = 0; i < MatrixType::Order; ++i)
					{
						mat.m[MatrixType::CELL_INDEX(row, i)] = v[i];
					}
				}
			}

			using Matrix4f = Matrix4<float>;
			using Matrix4i = Matrix4<int>;
			using Matrix3f = Matrix3<float>;
			using Matrix3i = Matrix3<int>;
		}

	}
}
