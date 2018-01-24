#include <cstdlib>
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

		auto m3 = m2 - m1;
		std::cout << "m3 = m2 - m1" << std::endl;
		std::cout << m3 << std::endl;

		std::cout << "m1 is " << std::endl;
		std::cout << m1 << std::endl;
		std::cout << "m2 is " << std::endl;
		std::cout << m2 << std::endl;
		std::cout << "m1 * m2 is " << std::endl;
		std::cout << m1 * m2 << std::endl;

		m.TransposeInPlace();

		std::cout << "After transposeInPlace, the matrix becomes:" << std::endl;
		std::cout << m << std::endl;
		system("pause");
	}
}