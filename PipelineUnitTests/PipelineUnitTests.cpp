// PipelineUnitTests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define BOOST_TEST_MODULE MyTest
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(my_test)
{
	BOOST_CHECK(2 + 2 == 4);
	BOOST_CHECK(2 + 2 < 4);
	BOOST_REQUIRE(2 + 2 > 4);
	BOOST_CHECK(2 + 3 == 5);

}


