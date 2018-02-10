#include <cstdlib>
#include <vector>
#include <iostream>
#include "catch.hpp"
#include "matrix.h"
#include "vector.h"
#include "rect.h"
#include "color.h"

namespace
{
	using LightningGE::Render::Matrix;
	using LightningGE::Render::Matrix4x4f;
	using LightningGE::Render::VectorList;
	using LightningGE::Render::Vector4f;
	using LightningGE::Render::Vector3f;
	using LightningGE::Render::Vector;
	using LightningGE::Render::RectF;
	using LightningGE::Render::ColorF;
	using LightningGE::Render::MatrixList;
	template<typename _Scalar, int Rows, int Columns>
	std::ostream& operator<<(std::ostream& out, Matrix<_Scalar, Rows, Columns>& m)
	{
		for (int i = 0;i < Rows;++i)
		{
			for (int j = 0;j < Columns;++j)
			{
				out << m(i, j) << ' ';
			}
			out << std::endl;
		}
		return out;
	}

	template<typename _Scalar, int Dimension>
	std::ostream& operator<<(std::ostream& out, Vector<_Scalar, Dimension>& v)
	{
		out << "[";
		for (int i = 0;i < Dimension;++i)
		{
			out << v[i];
			if (i < Dimension - 1)
			{
				out << ',';
			}
		}
		out << "]";
		return out;
	}

	TEST_CASE("Matrix function test", "[Matrix Test]")
	{
		//Vector3f v1{ 1.0, 2.0, 3.0};
		//Vector3f v2{ 5.0, 6.0, 7.0};
		//std::cout << "Dot product of v1 and v2 is " << std::endl;
		//std::cout << v1.Dot(v2) << std::endl;
		//std::cout << "vector is " << v << std::endl;
		//std::cout << "v[0] is " << v[0] << std::endl;
		//std::cout << "4 rank identity matrix is " << std::endl;
		//auto m = Matrix4x4f::Identity();
		//std::cout << m << std::endl;
		//std::cout << "Inverse matrix of identity matrix is" << std::endl;
		//std::cout << m.Inverse() << std::endl;
		//std::cout << "v1.Cross(v2) is " << std::endl;
		//auto v3 = v1.Cross(v2);
		//std::cout << "v1 is " << v1 << std::endl;
		//std::cout << "v2 is " << v2 << std::endl;
		//std::cout << "The cross product of v1 and v2 (v3) is " << v3 << std::endl;
		//
		//std::cout << "v1.Dot(v3) is " << v1.Dot(v3) << std::endl;
		//std::cout << "v2.Dot(v3) is " << v2.Dot(v3) << std::endl;


		//Matrix4x4f m{ 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0 };
		//std::cout << "Original matrix is " << std::endl;
		//std::cout << m << std::endl;
		//std::cout << "top left 3x3 submatrix is " << std::endl;
		//std::cout << m.SubMatrix<3, 3>(0, 0) << std::endl;

		//RectF rect(1.0f, 2.0f, 3.0f, 4.0f);
		//std::cout << "left is " << rect.left() << std::endl;
		//std::cout << "right is " << rect.right() << std::endl;
		//std::cout << "top is " << rect.top() << std::endl;
		/*
		ColorF c(0.2f, 0.3f, 0.4f, 0.5f);
		std::cout << "r:" << c.r() << std::endl;
		std::cout << "g:" << c.g() << std::endl;
		std::cout << "b:" << c.b() << std::endl;
		std::cout << "a:" << c.a() << std::endl;
		system("pause");
		*/
		
		Vector4f v0{ 1.0, 2.0, 3.0, 4.0 };
		Vector4f v1{ 5.0, 6.0, 7.0, 8.0 };
		Vector4f v2{ 9.0,10.0,11.0,12.0 };
		Vector4f v3{ 13.0, 14.0, 15.0,16.0 };
		
		Matrix4x4f m(v0, v1, v2, v3);
		std::cout << "matrix constructed from four vectors" << std::endl;
		std::cout << m << std::endl;
		
		system("pause");
		return;
		/*
		Matrix<float, 4, 4> m;
		std::cout << "Original matrix is :" << std::endl;
		std::cout << m;

		std::cout << "m.Invertible() == " << m.Invertible() << std::endl;
		//std::cout << m.Inverse() << std::endl;
		
		MatrixList<float, 4, 4> ml;
		for (int i = 0; i < 10; ++i)
		{
			Matrix4x4f em{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
			ml.push_back(em);
			std::cout << "push_back a new matrix4x4f to matrix list" << std::endl;
		}

		VectorList<float, 4> vl;
		for (int i = 0; i < 5; ++i)
		{
			Vector4f v{ 2.0f, 3.0f, 1.0f, 0.0f };
			vl.push_back(v);
			std::cout << "push_back a new vector4f to vector list" << std::endl;
		}

		Vector<float, 4> v{ 1.0, 2.0, 3.0, 4.0 };

		std::cout << "matrix is " << std::endl;
		std::cout << m << std::endl;
		std::cout << "vector is " << std::endl;
		std::cout << v << std::endl;

		std::cout << "matrix multiply vector is " << std::endl;
		std::cout << m * v << std::endl;
		system("pause");
		
		Matrix<float, 4, 4> m1 = m;

		std::cout << "After Set the matrix becomes :" << std::endl;
		std::cout << m << std::endl;

		std::cout << "Constructed from copy constructor:" << std::endl;
		std::cout << m1 << std::endl;

		std::cout << "m == m1:" << (m == m1) << std::endl;
		std::cout << "m != m1:" << (m != m1) << std::endl;

		Matrix<float, 4, 4> m2 = m + m1;
		std::cout << "m2 is " << std::endl;
		std::cout << m2 << std::endl;
		m2(0, 1) = 1024;
		std::cout << "operator () assignment make m2 to " << std::endl;
		std::cout << m2 << std::endl;

		m2 = m1;
		std::cout << "After assignment m2 is " << std::endl;
		std::cout << m2 << std::endl;

		std::cout << "m1 += m2" << std::endl;
		m1 += m2;
		std::cout << m1 << std::endl;

		auto m3 = m2 - m1;
		std::cout << "m3 = m2 - m1" << std::endl;
		std::cout << m3 << std::endl;

		std::cout << "m1 is " << std::endl;
		std::cout << m1 << std::endl;
		std::cout << "m2 is " << std::endl;
		std::cout << m2 << std::endl;
		std::cout << "m1 * m2(mul_m) is " << std::endl;
		auto mul_m = m1 * m2;
		std::cout << mul_m << std::endl;

		std::cout << "TransposeInPlace mul_m is " << std::endl;
		mul_m.TransposeInPlace();
		std::cout << mul_m << std::endl;
		system("pause");
		/*
		std::vector<float> v{ 137, 259, 111, 398, 520, 1133 };
		Matrix<float, 2, 3> m_construct_from_vector(v);
		Matrix<float, 3, 2> m_construct_from_initializer_list({ 11,22,33,44,55,66 });

		std::cout << "matrix constructed from vector:" << std::endl;
		std::cout << m_construct_from_vector << std::endl;

		std::cout << "matrix constructed from initializer list:" << std::endl;
		std::cout << m_construct_from_initializer_list << std::endl;

		std::cout << "Transposed matrix of matrix_constructed_from_vector is " << std::endl;
		std::cout << m_construct_from_vector.Transpose() << std::endl;


		Matrix<float, 4, 4> translationMatrix;
		const float tdata[] = {1, 0, 0, 1, 0, 1, 0, 3, 0, 0, 1, 4, 0, 0, 0, 1};
		translationMatrix.Set(tdata);
		Matrix<float, 4, 1> pos;
		const float pdata[] = { 10.0f, 20.0f, 30.0f, 1.0f };
		pos.Set(pdata);
		std::cout << "translation matrix is " << std::endl;
		std::cout << translationMatrix << std::endl;
		std::cout << "position is " << std::endl;
		std::cout << pos << std::endl;

		std::cout << "The transformed pos is " << std::endl;
		std::cout << translationMatrix * pos << std::endl;


		std::cout << "After transposeInPlace, the matrix becomes:" << std::endl;
		std::cout << m << std::endl;
		system("pause");
		*/		

	}
}