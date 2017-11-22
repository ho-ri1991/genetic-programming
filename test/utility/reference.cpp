#define BOOST_TEST_NO_LIB
#define BOOST_TEST_MAIN
#include <gp/utility/reference.hpp>
#include <boost/test/unit_test.hpp>

using namespace gp;

BOOST_AUTO_TEST_SUITE(reference)
    BOOST_AUTO_TEST_CASE(reference) {
        utility::Reference<int> ref1;
        BOOST_CHECK(!ref1);
        utility::Variable var1(1);
        ref1.setVariable(var1);
        BOOST_CHECK(ref1);
        BOOST_CHECK_EQUAL(ref1.getRef(), 1);
        ref1.getRef() = 2;
        BOOST_CHECK_EQUAL(ref1.getRef(), 2);
        BOOST_CHECK_EQUAL(var1.get<int>(), 2);

        utility::Variable var2(1);
        utility::Reference<int> ref2(var2);
        utility::Reference<int> copyRef2 = ref2;
        BOOST_CHECK(ref2);
        BOOST_CHECK(copyRef2);
        BOOST_CHECK_EQUAL(ref2.getRef(), 1);
        BOOST_CHECK_EQUAL(copyRef2.getRef(), 1);
        ref2.getRef() = 2;
        BOOST_CHECK_EQUAL(ref2.getRef(), 2);
        BOOST_CHECK_EQUAL(copyRef2.getRef(), 2);
        BOOST_CHECK_EQUAL(var2.get<int>(), 2);

        int x = 0;
        utility::Reference<int> ref3(x);
        BOOST_CHECK(ref3);
        BOOST_CHECK_EQUAL(ref3.getRef(), 0);
        ref3.getRef() = 1;
        BOOST_CHECK_EQUAL(ref3.getRef(), 1);
        BOOST_CHECK_EQUAL(x, 1);
    }
BOOST_AUTO_TEST_SUITE_END()
