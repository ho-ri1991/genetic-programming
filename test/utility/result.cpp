#define BOOST_TEST_NO_LIB
#define BOOST_TEST_MAIN
#include <gp/utility/result.hpp>
#include <vector>
#include <list>
#include <boost/test/unit_test.hpp>

using namespace gp;

BOOST_AUTO_TEST_SUITE(result)
    BOOST_AUTO_TEST_CASE(result_unwrap) {
        auto result1 = utility::result::ok(1);
        BOOST_CHECK(result1);
        BOOST_CHECK_EQUAL(result1.unwrap(), 1);
        const auto& cRefResult1 = result1;
        BOOST_CHECK(cRefResult1);
        BOOST_CHECK_EQUAL(cRefResult1.unwrap(), 1);
        BOOST_CHECK_EQUAL(std::move(result1).unwrap(), 1);

        auto result2 = utility::result::err<int>("err");
        BOOST_CHECK(!result2);
        BOOST_CHECK_EXCEPTION(result2.unwrap(), std::bad_variant_access, [](const std::bad_variant_access&){return true;});
        BOOST_CHECK_EQUAL(result2.errMessage(), "err");
        BOOST_CHECK_EXCEPTION(std::move(result2).unwrap(), std::bad_variant_access, [](const std::bad_variant_access&){return true;});
    }
    BOOST_AUTO_TEST_CASE(result_ok_or) {
        auto result1 = utility::result::ok(1);
        BOOST_CHECK_EQUAL(result1.ok_or([](){return -1;}), 1);
        BOOST_CHECK_EQUAL(std::move(result1).ok_or([](){return -1;}), 1);

        auto result2 = utility::result::err<int>("msg");
        BOOST_CHECK_EQUAL(result2.ok_or([](){return -1;}), -1);
        BOOST_CHECK_EQUAL(std::move(result2).ok_or([](){return -1;}), -1);
    }
    BOOST_AUTO_TEST_CASE(result_map) {
        auto result1 = utility::result::ok(1);
        auto mapRes1 = result1.map([](int x){return x == 1;});
        BOOST_CHECK(mapRes1);
        BOOST_CHECK_EQUAL(mapRes1.unwrap(), true);
        auto mapMoveRes1 = std::move(result1).map([](int x){return x == 1;});
        BOOST_CHECK(mapMoveRes1);
        BOOST_CHECK_EQUAL(mapMoveRes1.unwrap(), true);

        auto result2 = utility::result::err<int>("msg");
        auto mapRes2 = result2.map([](int x){return x == 1;});
        BOOST_CHECK(!mapRes2);
        auto mapMoveRes2 = std::move(result2).map([](int x){return x == 1;});
        BOOST_CHECK(!mapMoveRes2);
    }
    BOOST_AUTO_TEST_CASE(result_flat_map) {
        auto result1 = utility::result::ok(1);
        auto fmapRes1 = result1.flatMap([](int x){return utility::result::ok(x == 1);});
        BOOST_CHECK(fmapRes1);
        BOOST_CHECK_EQUAL(fmapRes1.unwrap(), true);
        auto fmapResErr1 = result1.flatMap([](int x){return utility::result::err<bool>("err");});
        BOOST_CHECK(!fmapResErr1);
        auto fmapMoveRes1 = std::move(result1).flatMap([](int x){return utility::result::ok(x == 1);});
        BOOST_CHECK(fmapMoveRes1);
        BOOST_CHECK_EQUAL(fmapMoveRes1.unwrap(), true);

        auto result2 = utility::result::err<int>("err");
        auto fmapRes2 = result2.flatMap([](int x){return utility::result::ok(x == 1);});
        BOOST_CHECK(!fmapRes2);
        auto fmapMoveRes2 = std::move(result2).flatMap([](int x){ return utility::result::ok(x == 1);});
        BOOST_CHECK(!fmapMoveRes2);
    }
    BOOST_AUTO_TEST_CASE(result_match) {
        auto result1 = utility::result::ok(1);
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

        auto result2 = utility::result::err<int>("err");
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
    BOOST_AUTO_TEST_CASE(result_from_optional) {
        std::optional<int> option1 = 1;
        auto result1 = utility::result::fromOptional(option1, "err");
        BOOST_CHECK(result1);
        BOOST_CHECK_EQUAL(result1.unwrap(), 1);
        auto resultMove1 = utility::result::fromOptional(std::move(option1), "err");
        BOOST_CHECK(resultMove1);
        BOOST_CHECK_EQUAL(resultMove1.unwrap(), 1);

        std::optional<int> option2;
        auto result2 = utility::result::fromOptional(option2, "err");
        BOOST_CHECK(!result2);
        auto msg = result2.match([](auto&& result){
            using T = std::decay_t<decltype(result)>;
            if constexpr (std::is_same_v<T, utility::Ok<int>>) {
                return std::to_string(result.data);
            } else {
                return result.message;
            }
        });
        BOOST_CHECK_EQUAL(msg, "err");
        auto resultMove2 = utility::result::fromOptional(std::move(option2), "err");
        BOOST_CHECK(!resultMove2);
        auto msg2 = resultMove2.match([](auto&& result){
            using T = std::decay_t<decltype(result)>;
            if constexpr (std::is_same_v<T, utility::Ok<int>>) {
                return std::to_string(result.data);
            } else {
                return result.message;
            }
        });
        BOOST_CHECK_EQUAL(msg2, "err");

        int x = 0;
        boost::optional<int&> option3 = x;
        auto result3 = utility::result::fromOptional(option3, "err");
        BOOST_CHECK(result3);
        int& y = result3.unwrap();
        y = 1;
        BOOST_CHECK_EQUAL(x, 1);
    }
    BOOST_AUTO_TEST_CASE(result_sequence) {
        auto result1 = utility::result::sequence(utility::result::ok(1),
                                                 utility::result::ok(2.5),
                                                 utility::result::ok(false));
        BOOST_CHECK(result1);
        auto& ans1 = result1.unwrap();
        BOOST_CHECK_EQUAL(std::get<0>(ans1), 1);
        BOOST_CHECK_EQUAL(std::get<1>(ans1), 2.5);
        BOOST_CHECK_EQUAL(std::get<2>(ans1), false);

        auto result2 = utility::result::sequence(utility::result::err<int>("err0"),
                                                 utility::result::ok(2.5),
                                                 utility::result::err<bool>("err2"));
        BOOST_CHECK(!result2);
        BOOST_CHECK_EQUAL(result2.errMessage(), "err0\nerr2");

        auto result3 = utility::result::sequence(std::vector{utility::result::ok(1),
                                                             utility::result::ok(2),
                                                             utility::result::ok(3)});
        std::vector<int> ans3 = {1, 2, 3};
        BOOST_CHECK(result3);
        BOOST_CHECK(result3.unwrap() == ans3);

        auto result4 = utility::result::sequence(std::vector{utility::result::ok(1),
                                                             utility::result::err<int>("err1"),
                                                             utility::result::err<int>("err2")});
        BOOST_CHECK(!result4);
        BOOST_CHECK_EQUAL(result4.errMessage(), "err1\nerr2");

        auto result5 = utility::result::sequence(std::list{utility::result::ok(1),
                                                           utility::result::ok(2),
                                                           utility::result::ok(3)});
        auto ans5 = std::list{1, 2, 3};
        BOOST_CHECK(result5);
        BOOST_CHECK(result5.unwrap() == ans5);

        auto result6 = utility::result::sequence(std::list{utility::result::err<int>("err0"),
                                                           utility::result::err<int>("err1"),
                                                           utility::result::ok(1)});
        BOOST_CHECK(!result6);
        BOOST_CHECK_EQUAL(result6.errMessage(), "err0\nerr1");
    }
    BOOST_AUTO_TEST_CASE(result_try){
        auto result1 = utility::result::tryFunction([](){return 1;}, "err");
        BOOST_CHECK(result1);
        BOOST_CHECK_EQUAL(result1.unwrap(), 1);

        auto result2 = utility::result::tryFunction([](){throw std::runtime_error(""); return 0;}, "err");
        BOOST_CHECK(!result2);
        BOOST_CHECK_EQUAL(result2.errMessage(), "err");

        auto result3 = utility::result::tryFunction([](){throw std::runtime_error(""); return 0;}, "err", std::runtime_error(""));
        BOOST_CHECK(!result3);
        BOOST_CHECK_EQUAL(result3.errMessage(), "err");

        BOOST_CHECK_EXCEPTION(utility::result::tryFunction([](){throw std::runtime_error(""); return 0;}, "err", std::invalid_argument("")),
                              std::runtime_error,
                              [](const std::runtime_error&){return true;});
    }
BOOST_AUTO_TEST_SUITE_END()
