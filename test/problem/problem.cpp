#define BOOST_TEST_NO_LIB
#define BOOST_TEST_MAIN
#include <gp/problem/problem.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>
#include <boost/test/unit_test.hpp>

using namespace gp;

BOOST_AUTO_TEST_SUITE(problem_test)
    BOOST_AUTO_TEST_CASE(problem_test) {
        utility::StringToType stringToType;
        stringToType.setTypeNamePair<int>("int");
        stringToType.setTypeNamePair<utility::LeftHandValue<int>>("lvalue[int]");
        stringToType.setTypeNamePair<utility::Reference<int>>("ref[int]");
        stringToType.setTypeNamePair<bool>("bool");
        stringToType.setTypeNamePair<double>("double");

        auto stringToValues = std::make_tuple(
                std::function<int(const std::string&)>([](const std::string& str){return boost::lexical_cast<int>(str);}),
                std::function<bool(const std::string&)>([](const std::string& str){return boost::lexical_cast<bool>(str);}),
                std::function<double(const std::string&)>([](const std::string& str){return boost::lexical_cast<double>(str);})
        );

        std::stringstream sstream(
                std::string("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
                            + "<problem>\n"
                            + "    <name>test</name>\n"
                            + "    <return_type>int</return_type>\n"
                            + "    <arguments>\n"
                            + "        <type>int</type>\n"
                            + "        <type>bool</type>\n"
                            + "        <type>double</type>\n"
                            + "    </arguments>\n"
                            + "    <teacher_data_set>\n"
                            + "        <data>\n"
                            + "            <argument idx = \"0\">1</argument>\n"
                            + "            <argument idx = \"1\">1</argument>\n"
                            + "            <argument idx = \"2\">5.5</argument>\n"
                            + "            <answer>2</answer>\n"
                            + "        </data>\n"
                            + "        <data>\n"
                            + "            <argument idx = \"0\">1</argument>\n"
                            + "            <argument idx = \"1\">0</argument>\n"
                            + "            <argument idx = \"2\">0</argument>\n"
                            + "            <answer>0</answer>\n"
                            + "        </data>\n"
                            + "        <data>\n"
                            + "            <argument idx = \"0\">10</argument>\n"
                            + "            <argument idx = \"1\">1</argument>\n"
                            + "            <argument idx = \"2\">0.5</argument>\n"
                            + "            <answer>15</answer>\n"
                            + "        </data>\n"
                            + "        <data>\n"
                            + "            <argument idx = \"0\">-10</argument>\n"
                            + "            <argument idx = \"1\">0</argument>\n"
                            + "            <argument idx = \"2\">-0.05</argument>\n"
                            + "            <answer>-5</answer>\n"
                            + "        </data>\n"
                            + "    </teacher_data_set>\n"
                            + "</problem>"
        );

        auto problemResult = problem::load(sstream, stringToType, stringToValues);
        if(!problemResult)std::cout<<problemResult.errMessage()<<std::endl;
        BOOST_CHECK(problemResult);
        auto& problem1 = problemResult.unwrap();

        BOOST_CHECK_EQUAL(problem1.name, "test");
        BOOST_CHECK(*problem1.returnType == utility::typeInfo<int>());
        BOOST_CHECK_EQUAL(std::size(problem1.argumentTypes), 3);
        BOOST_CHECK(*problem1.argumentTypes[0] == utility::typeInfo<int>());
        BOOST_CHECK(*problem1.argumentTypes[1] == utility::typeInfo<bool>());
        BOOST_CHECK(*problem1.argumentTypes[2] == utility::typeInfo<double>());
        BOOST_CHECK_EQUAL(std::size(problem1.ansArgList), 4);
        {
            auto & [ans, args] = problem1.ansArgList[0];
            BOOST_CHECK_EQUAL(ans.get<int>(), 2);
            BOOST_CHECK_EQUAL(std::size(args), 3);
            BOOST_CHECK_EQUAL(args[0].get<int>(), 1);
            BOOST_CHECK_EQUAL(args[1].get<bool>(), true);
            BOOST_CHECK_EQUAL(args[2].get<double>(), 5.5);
        }
        {
            auto & [ans, args] = problem1.ansArgList[1];
            BOOST_CHECK_EQUAL(ans.get<int>(), 0);
            BOOST_CHECK_EQUAL(std::size(args), 3);
            BOOST_CHECK_EQUAL(args[0].get<int>(), 1);
            BOOST_CHECK_EQUAL(args[1].get<bool>(), false);
            BOOST_CHECK_EQUAL(args[2].get<double>(), 0);
        }
        {
            auto & [ans, args] = problem1.ansArgList[2];
            BOOST_CHECK_EQUAL(ans.get<int>(), 15);
            BOOST_CHECK_EQUAL(std::size(args), 3);
            BOOST_CHECK_EQUAL(args[0].get<int>(), 10);
            BOOST_CHECK_EQUAL(args[1].get<bool>(), true);
            BOOST_CHECK_EQUAL(args[2].get<double>(), 0.5);
        }
        {
            auto & [ans, args] = problem1.ansArgList[3];
            BOOST_CHECK_EQUAL(ans.get<int>(), -5);
            BOOST_CHECK_EQUAL(std::size(args), 3);
            BOOST_CHECK_EQUAL(args[0].get<int>(), -10);
            BOOST_CHECK_EQUAL(args[1].get<bool>(), false);
            BOOST_CHECK_EQUAL(args[2].get<double>(), -0.05);
        }
    }
BOOST_AUTO_TEST_SUITE_END()
