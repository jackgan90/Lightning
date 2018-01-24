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
		out << std::endl;
		return out;
	}
	TEST_CASE("Matrix function test", "[Matrix Test]")
	{
		Matrix<float, 4, 4> m;
		std::cout << "Original matrix is :" << std::endl;
		std::cout << m;
		float data[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
		m.Set(data);

		std::cout << "After Set the matrix becomes :" << std::endl;
		std::cout << m;

		m.TransposeInPlace();

		std::cout << "After transposeInPlace, the matrix becomes:" << std::endl;
		std::cout << m;
		system("pause");
	}
}