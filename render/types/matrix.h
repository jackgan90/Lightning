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
			//Don't declare any of the five member control methods here,just let the compiler auto generated them.since this class is highly unlikely has many
			//child classes and even if there's child class of Matrix,subclasses shouldn't add too complex structures
			using VectorBaseType = Matrix<_Scalar, Rows, 1>;
			Matrix():m_value(InternalMatrixType::Identity()){}
			//construct with an initializer list
			Matrix(const std::initializer_list<_Scalar>& data) 
			{ 
				Set(data); 
			}
			template<typename ScalarPointerType, typename SizeType, typename = 
				typename std::enable_if<std::is_integral<typename std::decay<SizeType>::type>::value	//ensure the second argument is an integral type
				&& std::is_pointer<typename std::decay<ScalarPointerType>::type>::value	//ensure the first argument is a pointer
				&& std::is_convertible<typename std::decay<typename std::remove_pointer<ScalarPointerType>::type>::type, //ensure data pointed to by first argument can be converted to _Scalar
				typename std::decay<_Scalar>::type>::value
				>::type
			>
			Matrix(const ScalarPointerType arr, const SizeType size)
			{
				Set(arr, size);
			}
			template<typename S, int _Rows, int _Columns>
			Matrix(const Matrix<S, _Rows, _Columns>& m)
			{
				SetInternal<S, _Rows, _Columns>(m);
			}
			template<typename S, int _Rows, int _Columns>
			Matrix(Matrix<S, _Rows, _Columns>&& m)
			{
				SetInternal<S, _Rows, _Columns>(std::move(m));
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

			Matrix<_Scalar, 1, Columns> GetRow(const int i)const { return Matrix<_Scalar, 1, Columns>(m_value.row(i)); }
			Matrix<_Scalar, Rows, 1> GetColumn(const int i)const { return Matrix<_Scalar, Rows, 1>(m_value.col(i)); }

			bool operator==(const Matrix<_Scalar, Rows, Columns>& m)const {return m_value.isApprox(m.m_value);}
			template<int _Rows, int _Columns>
			bool operator==(const Matrix<_Scalar, _Rows, _Columns>& m)const
			{
				if (Rows != _Columns || Columns != _Rows || (Rows != 1 && Columns != 1))
					return false;
				return m_value.isApprox(m.m_value.transpose());
			}
			bool operator!=(const Matrix<_Scalar, Rows, Columns>& m)const{return !(*this == m); }
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
			Matrix<_Scalar, Rows, Columns> operator-()const
			{
				return Matrix<_Scalar, Rows, Columns>(-m_value);
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
			void SetZero()
			{
				m_value.setZero();
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
				typename std::enable_if<
				std::is_convertible<typename std::decay<decltype(*std::cbegin(data))>::type, typename std::decay<_Scalar>::type>::value
				&& std::is_convertible<typename std::decay<decltype(*std::cend(data))>::type, typename std::decay<_Scalar>::type>::value
				>::type
			{
				int idx = 0;
				for (auto it = std::cbegin(data); it != std::cend(data);++it)
				{
					m_value(idx % Rows, idx / Rows) = *it;
					++idx;
				}
			}
			template<typename ScalarPointerType, typename SizeType>
			auto Set(const ScalarPointerType arr, const SizeType size) ->
				typename std::enable_if<std::is_integral<typename std::decay<SizeType>::type>::value	//ensure the second argument is an integral type
				&& std::is_pointer<typename std::decay<ScalarPointerType>::type>::value	//ensure the first argument is a pointer
				&& std::is_convertible<typename std::decay<typename std::remove_pointer<ScalarPointerType>::type>::type, //ensure data pointed to by first argument can be converted to _Scalar
				typename std::decay<_Scalar>::type>::value
				>::type
			{
				int idx = 0;
				for (typename std::decay<SizeType>::type i = 0;i < size;++i)
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
				static const Matrix<_Scalar, Rows, Columns> m(InternalMatrixType::Identity());
				return m;
			}
			const _Scalar* GetData()const { return m_value.data(); }
		protected:
			static constexpr int InternalAlignmentOptions = Eigen::DontAlign | 
				((Rows == 1 && Columns != 1) ? Eigen::RowMajor : 
				(Columns == 1 && Rows != 1) ? Eigen::ColMajor : Eigen::ColMajor);
			using InternalMatrixType = Eigen::Matrix<_Scalar, Rows, Columns, InternalAlignmentOptions>;
			template<int Dimension>
			using InternalVectorType = Eigen::Matrix<_Scalar, Dimension, 1, InternalAlignmentOptions>;
			template<typename _Scalar, int _Rows, int _Columns> friend class Matrix;
			template<typename _Scalar, int Dimension> friend class Vector;
			InternalMatrixType m_value;
			Matrix(const InternalMatrixType& m):m_value(m){}
			Matrix(InternalMatrixType&& m):m_value(std::move(m)){}
			template<typename S, int _Rows, int _Columns, typename T>
			auto SetInternal(T&& m) ->
				typename std::enable_if<std::is_convertible<
					typename std::decay<T>::type,
					Matrix<typename std::decay<S>::type, _Rows, Columns>>::value
				>::type
			{
				SetInternalImpl<S, _Rows, _Columns>(std::forward<T>(m));
			}

			template<typename S, int _Rows, int _Columns>
			auto SetInternalImpl(Matrix<S, _Rows, _Columns>&& m) ->
				typename std::enable_if<!(_Rows == Rows && _Columns == Columns) && (_Rows <= Rows && _Columns <= Columns)>::type
			{
				m_value.block<_Rows, _Columns>(0, 0) = std::move(m.m_value);
			}
			template<typename S, int _Rows, int _Columns>
			auto SetInternalImpl(const Matrix<S, _Rows, _Columns>& m) ->
				typename std::enable_if<!(_Rows == Rows && _Columns == Columns) && (_Rows <= Rows && _Columns <= Columns)>::type
			{
				m_value.block<_Rows, _Columns>(0, 0) = m.m_value;
			}
			template<typename S, int _Rows, int _Columns>
			auto SetInternalImpl(Matrix<S, _Rows, _Columns>&& m) ->
				typename std::enable_if<!(_Rows == Rows && _Columns == Columns) && (_Rows >= Rows && _Columns >= Columns)>::type
			{
				m_value = std::move(m.m_value.block<Rows, Columns>(0, 0));
			}
			template<typename S, int _Rows, int _Columns>
			auto SetInternalImpl(const Matrix<S, _Rows, _Columns>& m) ->
				typename std::enable_if<(!(_Rows == Rows && _Columns == Columns) && _Rows >= Rows && _Columns >= Columns)>::type
			{
				m_value = m.m_value.block<Rows, Columns>(0, 0);
			}
			template<typename S, int _Rows, int _Columns>
			auto SetInternalImpl(const Matrix<S, _Rows, _Columns>& m) ->
				typename std::enable_if<(_Rows == Rows && _Columns == Columns)>::type
			{
				m_value = m.m_value;
			}
			template<typename S, int _Rows, int _Columns>
			auto SetInternalImpl(Matrix<S, _Rows, _Columns>&& m) ->
				typename std::enable_if<(_Rows == Rows && _Columns == Columns)>::type
			{
				m_value = std::move(m.m_value);
			}
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

			InternalMatrixType invMatrix;
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

			Eigen::FullPivLU<InternalMatrixType> lu(m_value);
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
