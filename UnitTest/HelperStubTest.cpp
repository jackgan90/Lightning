#include "catch.hpp"
#include "HelperStubs.h"

namespace
{
	TEST_CASE("Make array", "[make_array test]")
	{
		auto intArray = helper::make_array(1, 2, 3, 4, 5);
		REQUIRE(intArray.size() == 5);
		REQUIRE(intArray[0] == 1);

		int a = 10;
		auto res = std::is_same<decltype(a), int>::value == true;
		REQUIRE(res);
	}
}