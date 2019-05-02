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
		}

	}
}
