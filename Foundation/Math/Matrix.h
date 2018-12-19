#pragma once
#include <type_traits>
#include "Vector.h"

namespace Lightning
{
	namespace Foundation
	{
		namespace Math
		{
			template<typename T>
			struct Matrix4
			{
				static_assert(std::is_pod<T>::value, "T is not a POD!");
				void SetZero()
				{
					std::memset(m, 0, sizeof(m));
				}

				void SetIdentity()
				{
					SetZero();
					m[0] = m[5] = m[10] = m[15] = T(1);
				}

				void SetCell(unsigned row, unsigned column, T value)
				{
					m[CELL_INDEX(row, column)] = value;
				}

				T GetCell(unsigned row, unsigned column)const
				{
					return m[CELL_INDEX(row, column)];
				}

				void MultMatrix(const Matrix4<T>& other, Matrix4<T>& result)const
				{
					for (int i = 0;i < 4;++i)
					{
						for (int j = 0; j < 4; ++j)
						{
							auto idx = CELL_INDEX(i, j);
							result.m[idx] = T(0);
							for (int k = 0; k < 4; ++k)
								result.m[idx] += m[CELL_INDEX(i, k)] * other.m[CELL_INDEX(k, j)];
						}
					}
				}

				Matrix4<T> operator*(const Matrix4<T>& other)const
				{
					Matrix4<T> res;
					MultMatrix(other, res);
					return res;
				}

				Matrix4<T>& operator*=(const Matrix4<T>& other)
				{
					*this = (*this) * other;
					return *this;
				}

				void SetRow(unsigned row, const Vector4<T>& v) 
				{ 
					m[CELL_INDEX(row, 0)] = v.x;
					m[CELL_INDEX(row, 1)] = v.y;
					m[CELL_INDEX(row, 2)] = v.z;
					m[CELL_INDEX(row, 3)] = v.w;
				}

				void SetColumn(unsigned col, const Vector4<T>& v) 
				{ 
					m[CELL_INDEX(0, col)] = v.x;
					m[CELL_INDEX(1, col)] = v.y;
					m[CELL_INDEX(2, col)] = v.z;
					m[CELL_INDEX(3, col)] = v.w;
				}

				static inline constexpr std::size_t CELL_INDEX(std::size_t row, std::size_t col) { return col * 4 + row; }
				T m[16];
			};

			template<typename T>
			Vector4<T> operator*(const Matrix4<T>& mat, const Vector4<T>& v)
			{
				Vector4<T> res;
				res.x = mat.GetCell(0, 0) * v.x + mat.GetCell(0, 1) * v.y + \
					mat.GetCell(0, 2) * v.z + mat.GetCell(0, 3) * v.w;

				res.y = mat.GetCell(1, 0) * v.x + mat.GetCell(1, 1) * v.y + \
					mat.GetCell(1, 2) * v.z + mat.GetCell(1, 3) * v.w;

				res.z = mat.GetCell(2, 0) * v.x + mat.GetCell(2, 1) * v.y + \
					mat.GetCell(2, 2) * v.z + mat.GetCell(2, 3) * v.w;

				res.w = mat.GetCell(3, 0) * v.x + mat.GetCell(3, 1) * v.y + \
					mat.GetCell(3, 2) * v.z + mat.GetCell(3, 3) * v.w;

				return res;
			}

			template<typename T>
			Vector4<T> operator*(const Vector4<T>& v, const Matrix4<T>& mat)
			{
				return operator*(mat, v);
			}

			using Matrix4f = Matrix4<float>;
			using Matrix4i = Matrix4<int>;
			static_assert(std::is_pod<Matrix4f>::value, "Matrix4f is not a POD");
			static_assert(std::is_pod<Matrix4i>::value, "Matrix4i is not a POD!");

			/*
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

				void SetCell(unsigned row, unsigned col, const T& value)
				{
					Derived* const pDerived = reinterpret_cast<Derived* const>(this);
					pDerived->m[CELL_INDEX(row, col)] = value;
				}

				void MultMatrix(const Derived& other, Derived& mat)const
				{
					const Derived *const pDerived = reinterpret_cast<const Derived* const>(this);
					for (int i = 0;i < Derived::Order;++i)
					{
						for (int j = 0; j < Derived::Order; ++j)
						{
							auto idx = CELL_INDEX(i, j);
							mat.m[idx] = 0;
							for (int k = 0; k < Derived::Order; ++k)
								mat.m[idx] += pDerived->m[CELL_INDEX(i, k)] * other.m[CELL_INDEX(k, j)];
						}
					}
				}

				Derived operator*(const Derived& other)const
				{
					Derived mat;
					MultMatrix(other, mat);
					return mat;
				}

				Derived& operator*=(const Derived& other)
				{
					auto pDerived = reinterpret_cast<Derived*>(this);
					*pDerived = *pDerived * other;
					return *pDerived;
				}

				template<typename V>
				std::enable_if_t<std::is_base_of<VectorBase<V, T>, V>::value, V> operator*(const V& v)const
				{
					static_assert(Derived::Order == V::Order, "matrix multiply vector requires the same dimension!");
					V res;
					const Derived* const pDerived = reinterpret_cast<const Derived* const>(this);
					auto& m = pDerived->m;
					for (int i = 0;i < Derived::Order;++i)
					{
						T s{ 0 };
						for (int j = 0; j < Derived::Order;++j)
						{
							s += m[CELL_INDEX(i, j)] * v[j];
						}
						res[i] = s;
					}
					return res;
				}
				
				static inline std::size_t CELL_INDEX(std::size_t row, std::size_t col) { return col * Derived::Order + row; }
			};

			template<typename T>
			struct Matrix3 : MatrixBase<Matrix3<T>, T>
			{
				static constexpr unsigned Order = 3;
				void SetRow(unsigned row, const Vector3<T>& v) { SetMatrixRow(*this, row, v); }
				void SetColumn(unsigned col, const Vector3<T>& v) { SetMatrixColumn(*this, col, v); }
				T m[Order * Order];
			};

			template<typename T>
			struct Matrix4 : MatrixBase<Matrix4<T>, T>
			{
				static constexpr unsigned Order = 4;
				void SetRow(unsigned row, const Vector4<T>& v) { SetMatrixRow(*this, row, v); }
				void SetColumn(unsigned col, const Vector4<T>& v) { SetMatrixColumn(*this, col, v); }
				T m[Order * Order];
			};

			template<typename MatrixType, typename VectorType>
			void SetMatrixColumn(MatrixType& mat, unsigned col, const VectorType& v)
			{
				//static_assert(MatrixType::Order == VectorType::Order, "vector order must match matrix order!");
				if (col >= 0 && col < MatrixType::Order)
				{
					for (int i = 0; i < MatrixType::Order; ++i)
					{
						mat.m[MatrixType::CELL_INDEX(i, col)] = v[i];
					}
				}
			}

			template<typename MatrixType, typename VectorType>
			void SetMatrixRow(MatrixType& mat, unsigned row, const VectorType& v)
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

			using Matrix3f = Matrix3<float>;
			using Matrix3i = Matrix3<int>;
			*/
		}

	}
}
