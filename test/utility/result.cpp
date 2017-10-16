#define BOOST_TEST_NO_LIB
#define BOOST_TEST_MAIN
#include <gp/utility/result.hpp>
#include <boost/test/unit_test.hpp>

using namespace gp;

BOOST_AUTO_TEST_SUITE(result)
    BOOST_AUTO_TEST_CASE(result_unwrap) {
        auto result1 = utility::Result<int>::ok(1);
        BOOST_CHECK(result1);
        BOOST_CHECK_EQUAL(result1.unwrap(), 1);
        const auto& cRefResult1 = result1;
        BOOST_CHECK(cRefResult1);
        BOOST_CHECK_EQUAL(cRefResult1.unwrap(), 1);
        BOOST_CHECK_EQUAL(std::move(result1).unwrap(), 1);

        auto result2 = utility::Result<int>::err("err");
        BOOST_CHECK(!result2);
        BOOST_CHECK_EXCEPTION(result2.unwrap(), std::bad_variant_access, [](const std::bad_variant_access&){return true;});
        BOOST_CHECK_EXCEPTION(std::move(result2).unwrap(), std::bad_variant_access, [](const std::bad_variant_access&){return true;});
    }
    BOOST_AUTO_TEST_CASE(result_ok_or) {
        auto result1 = utility::Result<int>::ok(1);
        BOOST_CHECK_EQUAL(result1.ok_or([](){return -1;}), 1);
        BOOST_CHECK_EQUAL(std::move(result1).ok_or([](){return -1;}), 1);

        auto result2 = utility::Result<int>::err("msg");
        BOOST_CHECK_EQUAL(result2.ok_or([](){return -1;}), -1);
        BOOST_CHECK_EQUAL(std::move(result2).ok_or([](){return -1;}), -1);
    }
    BOOST_AUTO_TEST_CASE(result_map) {
        auto result1 = utility::Result<int>::ok(1);
        auto mapRes1 = result1.map([](int x){return x == 1;});
        BOOST_CHECK(mapRes1);
        BOOST_CHECK_EQUAL(mapRes1.unwrap(), true);
        auto mapMoveRes1 = std::move(result1).map([](int x){return x == 1;});
        BOOST_CHECK(mapMoveRes1);
        BOOST_CHECK_EQUAL(mapMoveRes1.unwrap(), true);

        auto result2 = utility::Result<int>::err("msg");
        auto mapRes2 = result2.map([](int x){return x == 1;});
        BOOST_CHECK(!mapRes2);
        auto mapMoveRes2 = std::move(result2).map([](int x){return x == 1;});
        BOOST_CHECK(!mapMoveRes2);
    }
    BOOST_AUTO_TEST_CASE(result_flat_map) {
        auto result1 = utility::Result<int>::ok(1);
        auto fmapRes1 = result1.flatMap([](int x){return utility::Result<bool>::ok(x == 1);});
        BOOST_CHECK(fmapRes1);
        BOOST_CHECK_EQUAL(fmapRes1.unwrap(), true);
        auto fmapResErr1 = result1.flatMap([](int x){return utility::Result<bool>::err("err");});
        BOOST_CHECK(!fmapResErr1);
        auto fmapMoveRes1 = std::move(result1).flatMap([](int x){return utility::Result<bool>::ok(x == 1);});
        BOOST_CHECK(fmapMoveRes1);
        BOOST_CHECK_EQUAL(fmapMoveRes1.unwrap(), true);

        auto result2 = utility::Result<int>::err("err");
        auto fmapRes2 = result2.flatMap([](int x){return utility::Result<bool>::ok(x == 1);});
        BOOST_CHECK(!fmapRes2);
        auto fmapMoveRes2 = std::move(result2).flatMap([](int x){ return utility::Result<bool>::ok(x == 1);});
        BOOST_CHECK(!fmapMoveRes2);
    }
    BOOST_AUTO_TEST_CASE(result_match) {
        auto result1 = utility::Result<int>::ok(1);
        int ans1;
        result1.match([&ans1](auto&& result){
            using T = std::decay_t<decltype(result)>;
            if constexpr(std::is_same_v<utility::Ok<int>, T>) {
                ans1 = result.data;
            } else {
                ans1 = -1;
            }
        });
        BOOST_CHECK_EQUAL(ans1, 1);
        auto msg1 = std::move(result1).match([](auto&& result) {
            using T = std::decay_t<decltype(result)>;
            if constexpr (std::is_same_v<utility::Ok<int>, T>) {
                return std::to_string(result.data);
            } else {
                return result.message;
            }
        });
        BOOST_CHECK_EQUAL(msg1, "1");

        auto result2 = utility::Result<int>::err("err");
        int ans2;
        result2.match([&ans2](auto&& result) {
            using T = std::decay_t<decltype(result)>;
            if constexpr (std::is_same_v<utility::Ok<int>, T>){
                ans2 = result.data;
            } else {
                ans2 = -1;
            }
        });
        BOOST_CHECK_EQUAL(ans2, -1);

        auto msg2 = std::move(result2).match([](auto&& result) {
            using T = std::decay_t<decltype(result)>;
            if constexpr (std::is_same_v<utility::Ok<int>, T>) {
                return std::to_string(result.data);
            } else {
                return result.message;
            }
        });
        BOOST_CHECK_EQUAL(msg2, "err");
    }
BOOST_AUTO_TEST_SUITE_END()
