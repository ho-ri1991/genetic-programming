#define BOOST_TEST_NO_LIB
#define BOOST_TEST_MAIN
#include <gp/utility/variable.hpp>
#include <typeinfo>
#include <boost/test/unit_test.hpp>

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

        utility::Variable pVar(&x);
        BOOST_CHECK(pVar.getType() == typeid(int));
        BOOST_CHECK_EQUAL(pVar.get<int>(), 1);
        BOOST_CHECK_EQUAL(pVar.get<int*>(), &x);
        pVar.get<int&>() = 2;
        BOOST_CHECK_EQUAL(pVar.get<int>(), 2);
        BOOST_CHECK_EQUAL(x, 2);
        *pVar.get<int*>() = 3;
        BOOST_CHECK_EQUAL(pVar.get<int&>(), 3);
        BOOST_CHECK_EQUAL(x, 3);

        BOOST_CHECK_EQUAL(pVar.get<const int>(), 3);
        BOOST_CHECK_EQUAL(pVar.get<const int&>(), 3);
        BOOST_CHECK_EQUAL(*pVar.get<const int*>(), 3);
        BOOST_CHECK_EQUAL(*pVar.get<int* const>(), 3);
        BOOST_CHECK_EQUAL(*pVar.get<const int* const>(), 3);


        utility::Variable var3(1);
        BOOST_CHECK_EQUAL(var3.get<int&>(), 1);
        BOOST_CHECK_EQUAL(var3.get<const int&>(), 1);
        BOOST_CHECK_EQUAL(*var3.get<const int*>(), 1);
        BOOST_CHECK_EQUAL(*var3.get<int* const>(), 1);
        BOOST_CHECK_EQUAL(*var3.get<const int* const>(), 1);

    }
BOOST_AUTO_TEST_SUITE_END()
