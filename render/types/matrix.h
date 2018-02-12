#pragma once
#include <iterator>
#include <vector>
#include <tuple>
#include <array>
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
			static_assert(Rows > 0 && Columns > 0, "Rows and Columns must be positive integers!");
		public:
			EIGEN_MAKE_ALIGNED_OPERATOR_NEW
			using VectorBaseType = Matrix<_Scalar, Rows, 1>;
			Matrix():m_value(Eigen::Matrix<_Scalar, Rows, Columns>::Identity()){}
			//construct with an initializer list
			Matrix(const std::initializer_list<_Scalar>& data) 
			{ 
				Set(data); 
			}
			Matrix(const _Scalar* arr, std::size_t size)
			{
				Set(arr, size);
			}
			template<typename Derived>
			Matrix(const std::array<Derived, Columns>& arr)
			{
				static_assert(std::is_base_of<VectorBaseType, Derived>::value, "Only derived classes of Matrix(vector etc> can be used to init");
				for (std::size_t i = 0;i < arr.size();++i)
				{
					SetColumns(i, arr[i]);
				}
			}

			template<typename S, int R, template<typename, int> typename _Vector1, template<typename, int> typename... _Vector2>
			typename std::enable_if<std::is_base_of<VectorBaseType, typename std::decay<_Vector1<S, R>>::type>::value>::type
			SetColumns(int i, const _Vector1<S, R>& col, const _Vector2<S, R>&... columns)
			{
				m_value.col(i) = col.m_value;
				SetColumns(i + 1, columns...);
			}

			template<typename S, int R, template<typename, int> typename _Vector1, template<typename, int> typename... _Vector2>
			typename std::enable_if<std::is_base_of<VectorBaseType, typename std::decay<_Vector1<S, R>>::type>::value>::type
			SetRows(int i, const _Vector1<S, R>& col, const _Vector2<S, R>&... rows)
			{
				m_value.row(i) = col.m_value;
				SetRows(i + 1, rows...);
			}

			bool operator==(const Matrix<_Scalar, Rows, Columns>& m){return m_value.isApprox(m.m_value);}
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
			auto Set(const Iterable& data) ->
				typename std::enable_if<std::is_same<typename std::decay<decltype(*std::cbegin(data))>::type, typename std::decay<_Scalar>::type>::value
				&& std::is_same<typename std::decay<decltype(*std::cend(data))>::type, typename std::decay<_Scalar>::type>::value>::type
			{
				int idx = 0;
				for (auto it = std::cbegin(data); it != std::cend(data);++it)
				{
					m_value(idx % Rows, idx / Rows) = *it;
					++idx;
				}
			}
			void Set(const _Scalar* arr, std::size_t size)
			{
				int idx = 0;
				for (std::size_t i = 0;i < size;++i)
				{
					m_value(idx % Rows, idx / Rows) = arr[i];
					++idx;
				}
			}
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
				static const Matrix<_Scalar, Rows, Columns> m(Eigen::Matrix<_Scalar, Rows, Columns>::Identity());
				return m;
			}
		protected:
			template<typename _Scalar, int _Rows, int _Columns> friend class Matrix;
			Eigen::Matrix<_Scalar, Rows, Columns> m_value;
			Matrix(const Eigen::Matrix<_Scalar, Rows, Columns>& m):m_value(m){}
			Matrix(Eigen::Matrix<_Scalar, Rows, Columns>&& m):m_value(std::move(m)){}
		private:
			void SetColumns(int i) {} //Only for execute internally
			void SetRows(int i) {} //Only for execute internally
		};

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
		using Matrix4x4i = Matrix<int, 4, 4>;
		using Matrix3x3f = Matrix<float, 3, 3>;
		using Matrix3x3i = Matrix<int, 3, 3>;
		using Matrix2x2f = Matrix<float, 2, 2>;
		using Matrix2x2i = Matrix<int, 2, 2>;

		template<typename _Scalar, int Rows, int Columns>
		using MatrixList = std::vector<Matrix<_Scalar, Rows, Columns>, Eigen::aligned_allocator<Matrix<_Scalar, Rows, Columns>>>;
	}
}
