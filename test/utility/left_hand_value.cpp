#define BOOST_TEST_NO_LIB
#define BOOST_TEST_MAIN
#include <gp/utility/left_hand_value.hpp>
#include <boost/test/unit_test.hpp>

using namespace gp;

BOOST_AUTO_TEST_SUITE(lvalue)
    BOOST_AUTO_TEST_CASE(lvalue) {
        utility::LeftHandValue<int> lvalue1;
        BOOST_CHECK(!lvalue1);
        utility::Variable var1(1);
        lvalue1.setVariable(var1);
        BOOST_CHECK(lvalue1);
        BOOST_CHECK_EQUAL(lvalue1.getRef(), 1);
        lvalue1.getRef() = 2;
        BOOST_CHECK_EQUAL(lvalue1.getRef(), 2);
        BOOST_CHECK_EQUAL(var1.get<int>(), 2);

        utility::Variable var2(1);
        utility::LeftHandValue<int> lvalue2(var2);
        BOOST_CHECK(lvalue2);
        BOOST_CHECK_EQUAL(lvalue2.getRef(), 1);
        lvalue2.getRef() = 2;
        BOOST_CHECK_EQUAL(lvalue2.getRef(), 2);
        BOOST_CHECK_EQUAL(var2.get<int>(), 2);

        int x = 0;
        utility::LeftHandValue<int> lvalue3(x);
        BOOST_CHECK(lvalue3);
        BOOST_CHECK_EQUAL(lvalue3.getRef(), 0);
        lvalue3.getRef() = 1;
        BOOST_CHECK_EQUAL(x, 1);
    }
BOOST_AUTO_TEST_SUITE_END()
