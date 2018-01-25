#include <cstdlib>
#include <vector>
#include <iostream>
#include "catch.hpp"
#include "matrix.h"

namespace
{
	using LightningGE::Render::Matrix;
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
	TEST_CASE("Matrix function test", "[Matrix Test]")
	{
		
		Matrix<float, 4, 4> m;
		std::cout << "Original matrix is :" << std::endl;
		std::cout << m;

		std::cout << "m.Invertible() == " << m.Invertible() << std::endl;
		//std::cout << m.Inverse() << std::endl;
		
		float data[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
		m.Set(data, false);

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
			
	}
}