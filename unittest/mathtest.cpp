#include <cstdlib>
#include <vector>
#include <list>
#include <set>
#include <array>
#include <iostream>
#include "catch.hpp"
#include "matrix.h"
#include "helperstubs.h"
#include "vector.h"
#include "rect.h"
#include "color.h"

namespace
{
	using LightningGE::Render::Matrix;
	using LightningGE::Render::Matrix4x4f;
	using LightningGE::Render::Matrix3x3f;
	using LightningGE::Render::Matrix2x2f;
	using LightningGE::Render::Matrix4x4i;
	using LightningGE::Render::Matrix3x3i;
	using LightningGE::Render::Matrix2x2i;
	using LightningGE::Render::VectorList;
	using LightningGE::Render::Vector4f;
	using LightningGE::Render::Vector4i;
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

	TEST_CASE("Vector test", "[Vector test]")
	{
		Vector3f v1{ 1.0, 2.0, 3.0};
		Vector3f v2{ 5.0, 6.0, 7.0};
		auto dotProduct = v1.Dot(v2);
		REQUIRE(dotProduct == Approx(38.0));
		auto v3 = v1.Cross(v2);
		REQUIRE(v3[0] == Approx(-4.0));
		REQUIRE(v3[1] == Approx(8.0));
		REQUIRE(v3[2] == Approx(-4.0));

		Vector3f zero = Vector3f::Zero();
		Vector3f zero1{ 0.0f, 0.0f, 0.0f };
		REQUIRE(zero == zero1);

		REQUIRE(zero.Length() == Approx(0.0f));

		Vector3f v4{0.0f, 3.0f, 4.0f};
		REQUIRE(v4.Length() == Approx(5.0f));

		v4.Normalize();
		REQUIRE(v4.Length() == Approx(1.0f));

		Vector3f v5{ 1.0f, 2.0f, 3.0f };
		auto v6 = v5.Normalized();
		REQUIRE(v6.Length() == Approx(1.0f));
		REQUIRE(v6.Cross(v5).Length() == Approx(0.0f));
	}

	TEST_CASE("Matrix test", "[Matrix test]")
	{
		auto m = Matrix4x4f::Identity();
		REQUIRE(m(0, 0) == 1);
		REQUIRE(m(0, 0) == m(1, 1));
		REQUIRE(m(1, 1) == m(2, 2));
		REQUIRE(m(2, 2) == m(3, 3));
		for (std::size_t i = 0;i < 4;++i)
		{
			for (std::size_t j = 0; j < 4; ++j)
			{
				if (i == j)
					continue;
				REQUIRE(m(i, j) == 0);
			}
		}
		m(3, 3) = 10.0;
		REQUIRE(m(3, 3) == Approx(10.0));
		Matrix4x4f m1{ 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0 };
		auto subm1 = m1.SubMatrix<2, 2>(1, 1);
		auto subm1_type_correct = std::is_same<decltype(subm1), Matrix<float, 2, 2>>::value;
		REQUIRE(subm1_type_correct);
		REQUIRE(m1(0, 0) == Approx(1.0));
		REQUIRE(m1(1, 0) == Approx(2.0));
		REQUIRE(m1(0, 1) == Approx(5.0));
		REQUIRE(m1(3, 2) == Approx(12.0));
		REQUIRE(subm1(0, 0) == Approx(6.0));
		REQUIRE(subm1(0, 1) == Approx(10.0));
		REQUIRE(subm1(1, 0) == Approx(7.0));
		REQUIRE(subm1(1, 1) == Approx(11.0));
		
		Vector4i col0{ 2, 3, 4, 5 };
		Vector4i col1{ 1, 3, 5, 7 };
		Vector4i col2{ 10, 20, 30, 40 };
		Vector4i col3{ -1, -2, -3, -4 };
		
		
		Matrix4x4i m2(helper::make_array(col0, col1, col2, col3));
		REQUIRE(m2(0, 0) == 2);
		REQUIRE(m2(1, 0) == 3);
		REQUIRE(m2(2, 0) == 4);
		REQUIRE(m2(3, 0) == 5);
		REQUIRE(m2(0, 1) == 1);
		REQUIRE(m2(1, 1) == 3);
		REQUIRE(m2(2, 1) == 5);
		REQUIRE(m2(3, 1) == 7);
		REQUIRE(m2(0, 2) == 10);
		REQUIRE(m2(1, 2) == 20);
		REQUIRE(m2(2, 2) == 30);
		REQUIRE(m2(3, 2) == 40);
		REQUIRE(m2(0, 3) == -1);
		REQUIRE(m2(1, 3) == -2);
		REQUIRE(m2(2, 3) == -3);
		REQUIRE(m2(3, 3) == -4);
		
		Matrix4x4i opm1 = { 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31 };
		Matrix4x4i opm2 = { 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32 };

		auto add_m = opm1 + opm2;
		auto minus_m = opm1 - opm2;
		auto mul_m = opm1 * opm2;

		REQUIRE(add_m(0, 0) == 3);
		REQUIRE(add_m(1, 0) == 7);
		REQUIRE(add_m(2, 0) == 11);
		REQUIRE(add_m(3, 0) == 15);
		REQUIRE(add_m(0, 1) == 19);
		REQUIRE(add_m(1, 1) == 23);
		REQUIRE(add_m(2, 1) == 27);
		REQUIRE(add_m(3, 1) == 31);
		REQUIRE(add_m(0, 2) == 35);
		REQUIRE(add_m(1, 2) == 39);
		REQUIRE(add_m(2, 2) == 43);
		REQUIRE(add_m(3, 2) == 47);
		REQUIRE(add_m(0, 3) == 51);
		REQUIRE(add_m(1, 3) == 55);
		REQUIRE(add_m(2, 3) == 59);
		REQUIRE(add_m(3, 3) == 63);

		REQUIRE(minus_m(0, 0) == -1);
		REQUIRE(minus_m(1, 0) == -1);
		REQUIRE(minus_m(2, 0) == -1);
		REQUIRE(minus_m(3, 0) == -1);
		REQUIRE(minus_m(0, 1) == -1);
		REQUIRE(minus_m(1, 1) == -1);
		REQUIRE(minus_m(2, 1) == -1);
		REQUIRE(minus_m(3, 1) == -1);
		REQUIRE(minus_m(0, 2) == -1);
		REQUIRE(minus_m(1, 2) == -1);
		REQUIRE(minus_m(2, 2) == -1);
		REQUIRE(minus_m(3, 2) == -1);
		REQUIRE(minus_m(0, 3) == -1);
		REQUIRE(minus_m(1, 3) == -1);
		REQUIRE(minus_m(2, 3) == -1);
		REQUIRE(minus_m(3, 3) == -1);

		REQUIRE(mul_m(0, 0) == 340);
		REQUIRE(mul_m(1, 0) == 380);
		REQUIRE(mul_m(2, 0) == 420);
		REQUIRE(mul_m(3, 0) == 460);
		REQUIRE(mul_m(0, 1) == 756);
		REQUIRE(mul_m(1, 1) == 860);
		REQUIRE(mul_m(2, 1) == 964);
		REQUIRE(mul_m(3, 1) == 1068);
		REQUIRE(mul_m(0, 2) == 1172);
		REQUIRE(mul_m(1, 2) == 1340);
		REQUIRE(mul_m(2, 2) == 1508);
		REQUIRE(mul_m(3, 2) == 1676);
		REQUIRE(mul_m(0, 3) == 1588);
		REQUIRE(mul_m(1, 3) == 1820);
		REQUIRE(mul_m(2, 3) == 2052);
		REQUIRE(mul_m(3, 3) == 2284);

		auto opm3(opm1);
		REQUIRE(opm1 == opm3);
		REQUIRE(opm1 != opm2);

		Vector4i row0{ 7, 7, 7, 7 };
		Vector4i row1{ 8, 8, 8, 8 };
		auto updatedRows = helper::make_array(row0, row1);
		auto opm4(opm3);
		opm3.SetRows(1, row0, row1);
		for (std::size_t i = 1;i < 3;++i)
		{
			for (std::size_t j = 0;j < 4;++j)
			{
				opm4(i, j) = updatedRows[i - 1][j];
			}
		}
		REQUIRE(opm3 == opm4);

		auto opm5(opm4);
		opm4.SetColumns(0, col0, col1);
		auto updatedColumns = helper::make_array(col0, col1);
		for (std::size_t i = 0;i < 2;++i)
		{
			for (std::size_t j = 0; j < 4; ++j)
			{
				opm5(j, i) = updatedColumns[i][j];
			}
		}
		REQUIRE(opm4 == opm5);

		Matrix2x2f fm2{ 7, -6, -4, 3 };
		REQUIRE(fm2.Invertible());

		Matrix3x3f fm3{ 1, 2, -2, -1, 1, -2, 3, 2, 1 };
		REQUIRE(fm3.Invertible());

		Matrix4x4f fm4{ 5, 0, -1, 1, 4, 1, -1, 1, 2, -1, 3, -1, 1, -1, 0, 2 };
		REQUIRE(fm4.Invertible());

		auto inv_fm2 = fm2.Inverse();
		REQUIRE(inv_fm2 * fm2 == Matrix2x2f::Identity());

		auto inv_fm3 = fm3.Inverse();
		REQUIRE(inv_fm3 * fm3 == Matrix3x3f::Identity());

		auto inv_fm4 = fm4.Inverse();
		REQUIRE(inv_fm4 * fm4 == Matrix4x4f::Identity());

		auto transposed_m(m2.Transpose());
		for (std::size_t i = 0;i < 4;++i)
		{
			for (std::size_t j = 0;j < 4;++j)
			{
				REQUIRE(m2(i, j) == transposed_m(j, i));
			}
		}
		REQUIRE(m2 != transposed_m);
		m2.TransposeInPlace();
		REQUIRE(m2 == transposed_m);
		
		VectorList<float, 4> vl;
		Vector<float, 4> reference_v{ 2,3,4,5 };
		for (std::size_t i = 0;i < 10;++i)
		{
			vl.emplace_back(std::initializer_list<float>{ 2, 3, 4, 5 });
			REQUIRE(vl.back() == reference_v);
		}

		MatrixList<float, 4, 4> ml;
		Matrix<float, 4, 4> reference_m{ 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597 };
		for (std::size_t i = 0;i < 10;++i)
		{
			ml.emplace_back(std::initializer_list<float>{ 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597 });
			REQUIRE(ml.back() == reference_m);
		}

		Matrix4x4f fm5;
		std::vector<float> v_ctr{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
		fm5.Set(v_ctr);
		REQUIRE(fm5(0, 0) == Approx(1.0f));
		REQUIRE(fm5(1, 0) == Approx(2.0f));
		REQUIRE(fm5(2, 0) == Approx(3.0f));
		REQUIRE(fm5(3, 0) == Approx(4.0f));
		REQUIRE(fm5(0, 1) == Approx(5.0f));

		std::list<float> l_ctr{ 6.0f, 7.0f, 8.0f, 9.0f, 10.0f };
		fm5.Set(l_ctr);
		REQUIRE(fm5(0, 0) == Approx(6.0f));
		REQUIRE(fm5(1, 0) == Approx(7.0f));
		REQUIRE(fm5(2, 0) == Approx(8.0f));
		REQUIRE(fm5(3, 0) == Approx(9.0f));
		REQUIRE(fm5(0, 1) == Approx(10.0f));

		std::set<float> s_ctr{ 11.0f, 12.0f, 13.0f, 14.0f, 15.0f };
		fm5.Set(s_ctr);
		REQUIRE(fm5(0, 0) == Approx(11.0f));
		REQUIRE(fm5(1, 0) == Approx(12.0f));
		REQUIRE(fm5(2, 0) == Approx(13.0f));
		REQUIRE(fm5(3, 0) == Approx(14.0f));
		REQUIRE(fm5(0, 1) == Approx(15.0f));

		std::multiset<float> ms_ctr{ 16.0f, 17.0f, 18.0f, 19.0f, 20.0f };
		fm5.Set(ms_ctr);
		REQUIRE(fm5(0, 0) == Approx(16.0f));
		REQUIRE(fm5(1, 0) == Approx(17.0f));
		REQUIRE(fm5(2, 0) == Approx(18.0f));
		REQUIRE(fm5(3, 0) == Approx(19.0f));
		REQUIRE(fm5(0, 1) == Approx(20.0f));

		const float a_ctr[] = { 21.0f, 22.0f, 23.0f, 24.0f, 25.0f };
		fm5.Set(a_ctr);
		REQUIRE(fm5(0, 0) == Approx(21.0f));
		REQUIRE(fm5(1, 0) == Approx(22.0f));
		REQUIRE(fm5(2, 0) == Approx(23.0f));
		REQUIRE(fm5(3, 0) == Approx(24.0f));
		REQUIRE(fm5(0, 1) == Approx(25.0f));

		std::array<float, 5> std_a_ctr = { 26.0f, 27.0f, 28.0f, 29.0f, 30.0f };
		fm5.Set(std_a_ctr);
		REQUIRE(fm5(0, 0) == Approx(26.0f));
		REQUIRE(fm5(1, 0) == Approx(27.0f));
		REQUIRE(fm5(2, 0) == Approx(28.0f));
		REQUIRE(fm5(3, 0) == Approx(29.0f));
		REQUIRE(fm5(0, 1) == Approx(30.0f));
	}

}