#define BOOST_TEST_MAIN
#include <gp/utility/variable.hpp>
#include <typeinfo>
#include <boost/test/included/unit_test.hpp>

using namespace gp;

BOOST_AUTO_TEST_SUITE(variable)
BOOST_AUTO_TEST_CASE(variable) {
        utility::Variable var1;
        BOOST_CHECK(!var1.hasValue());
        BOOST_CHECK(var1.getType() == typeid(void));

        var1.set(1);
        BOOST_CHECK(var1.getType() == typeid(int));
        BOOST_CHECK_EQUAL(var1.get<int>(), 1);
        var1.get<int&>() = 2;
        BOOST_CHECK_EQUAL(var1.get<int>(), 2);

        int x = 1;
        utility::Variable var2(x);
        BOOST_CHECK(var1.getType() == typeid(int));
        BOOST_CHECK_EQUAL(var2.get<int>(), 1);
        var2.get<int&>() = 2;
        BOOST_CHECK_EQUAL(var2.get<int>(), 2);
        var2.set(false);
        BOOST_CHECK(var2.getType() == typeid(bool));
        BOOST_CHECK_EQUAL(var2.get<bool>(), false);

        auto copyVar2 = var2;
        BOOST_CHECK(copyVar2.getType() == typeid(bool));
        BOOST_CHECK_EQUAL(copyVar2.get<bool>(), false);
        var2.set(true);
        BOOST_CHECK_EQUAL(copyVar2.get<bool>(), false);

//        utility::Variable pVar(&x);

}
BOOST_AUTO_TEST_SUITE_END()
