#define BOOST_TEST_NO_LIB
#define BOOST_TEST_MAIN
#include <fstream>
#include <gp/tree/tree.hpp>
#include <gp/node/node.hpp>
#include <gp/tree/io.hpp>
#include <boost/test/unit_test.hpp>
using namespace gp;

constexpr std::size_t MAX_SUBROUTINE_ARG_NUM = 3;

using TypeList = std::tuple<int>;

BOOST_AUTO_TEST_SUITE(io_test)
    BOOST_AUTO_TEST_CASE(io_test) {
        utility::StringToType stringToType;
        stringToType.setTypeNamePair<int>("int");
        stringToType.setTypeNamePair<utility::LeftHandValue<int>>("lvalue[int]");
        stringToType.setTypeNamePair<utility::Reference<int>>("ref[int]");
        stringToType.setTypeNamePair<bool>("bool");

        tree::TreeIO<MAX_SUBROUTINE_ARG_NUM, TypeList> treeIO;

        treeIO.registerNode(std::make_unique<node::PrognNode<int,2>>());
        treeIO.registerNode(std::make_unique<node::AddNode<int>>());
        treeIO.registerNode(std::make_unique<node::SubstitutionNode<int>>());
        treeIO.registerNode(std::make_unique<node::LocalVariableNode<int>>());
        treeIO.registerNode(std::make_unique<node::LocalVariableNode<utility::LeftHandValue<int>>>());
        treeIO.registerNode(std::make_unique<node::ArgumentNode<int,0>>());
        treeIO.registerNode(std::make_unique<node::ArgumentNode<int,1>>());
        treeIO.registerNode(std::make_unique<node::ArgumentNode<utility::Reference<int>, 0>>());
        treeIO.registerNode(std::make_unique<node::ArgumentNode<utility::LeftHandValue<int>, 0>>());
        treeIO.registerNode(std::make_unique<node::ConstNode<int>>(0));
        treeIO.registerNode(std::make_unique<node::GreaterNode<int>>());
        treeIO.registerNode(std::make_unique<node::GreaterEqNode<int>>());
        treeIO.registerNode(std::make_unique<node::LessThanEqNode<int>>());
        treeIO.registerNode(std::make_unique<node::LessThanNode<int>>());
        treeIO.registerNode(std::make_unique<node::EqualNode<int>>());
        treeIO.registerNode(std::make_unique<node::NotEqualNode<int>>());
        treeIO.registerNode(std::make_unique<node::IfNode<int>>());
        treeIO.registerNode(std::make_unique<node::MultiplyNode<int>>());
        treeIO.registerNode(std::make_unique<node::SubtractNode<int>>());
        treeIO.registerNode(std::make_unique<node::ConstNode<bool>>(false));
        treeIO.registerNode(std::make_unique<node::NopNode<int>>());
        treeIO.registerNode(std::make_unique<node::NopNode<bool>>());

        auto var0 = node::NodeInterface::createInstance<node::LocalVariableNode<int>>(0);
        auto arg1 = node::NodeInterface::createInstance<node::ArgumentNode<int, 1>>();
        auto add1 = node::NodeInterface::createInstance<node::AddNode<int>>();

        add1->setChild(0, std::move(arg1));
        add1->setChild(1, std::move(var0));

        auto c1 = node::NodeInterface::createInstance<node::ConstNode<int>>(10);
        auto add2 = node::NodeInterface::createInstance<node::AddNode<int>>();

        add2->setChild(0, std::move(c1));
        add2->setChild(1, std::move(add1));

        auto lvar0 = node::NodeInterface::createInstance<node::LocalVariableNode<utility::LeftHandValue<int>>>(0);
        auto arg0 = node::NodeInterface::createInstance<node::ArgumentNode<int, 0>>();
        auto subst = node::NodeInterface::createInstance<node::SubstitutionNode<int>>();

        subst->setChild(0, std::move(lvar0));
        subst->setChild(1, std::move(arg0));

        auto progn = node::NodeInterface::createInstance<node::PrognNode<int, 2>>();

        progn->setChild(0, std::move(subst));
        progn->setChild(1, std::move(add2));

        auto treeProperty = tree::TreeProperty{&utility::typeInfo<int>(),
                                               {&utility::typeInfo<int>(), &utility::typeInfo<int>()},
                                               {&utility::typeInfo<int>()},
                                               "TestTree"};

        tree::Tree tree(std::move(treeProperty), std::move(progn));

        for(int i = -10; i < 10; ++i) {
            int a0 = i;
            int a1 = 2 * i;
            auto ans = tree.evaluate(std::make_tuple(a0, a1));
            BOOST_CHECK_EQUAL(static_cast<int>(ans.getEvaluationStatus()), static_cast<int>(utility::EvaluationStatus::ValueReturned));
            BOOST_CHECK_EQUAL(std::any_cast<int>(ans.getReturnValue()), 10 + a0 + a1);
        }

        std::ofstream fout("TestTree.xml");
        treeIO.writeTree(tree, fout);
        fout.close();

        std::ifstream fin("TestTree.xml");
        auto inTreeResult = treeIO.readTree(fin, stringToType);
        BOOST_CHECK(inTreeResult);
        auto& inTree = inTreeResult.unwrap();
        BOOST_CHECK_EQUAL(inTree.getName(), tree.getName());
        BOOST_CHECK_EQUAL(inTree.getArgumentNum(), tree.getArgumentNum());
        BOOST_CHECK_EQUAL(inTree.getLocalVariableNum(), tree.getLocalVariableNum());
        BOOST_CHECK(tree.getReturnType() == inTree.getReturnType());
        for(int i = 0; i < tree.getArgumentNum(); ++i) {
            BOOST_CHECK(tree.getArgumentType(i) == inTree.getArgumentType(i));
        }
        for(int i = 0; i < tree.getLocalVariableNum(); ++i) {
            BOOST_CHECK(tree.getLocalVariableType(i) == inTree.getLocalVariableType(i));
        }

        for(int i = -10; i < 10; ++i) {
            int a0 = i;
            int a1 = 2 * i;
            auto ans = inTree.evaluate(std::make_tuple(a0, a1));
            BOOST_CHECK_EQUAL(static_cast<int>(ans.getEvaluationStatus()), static_cast<int>(utility::EvaluationStatus::ValueReturned));
            BOOST_CHECK_EQUAL(std::any_cast<int>(ans.getReturnValue()), 10 + a0 + a1);
        }
    }
    BOOST_AUTO_TEST_CASE(recursion_test) {
        utility::StringToType stringToType;
        stringToType.setTypeNamePair<int>("int");
        stringToType.setTypeNamePair<utility::LeftHandValue<int>>("lvalue[int]");
        stringToType.setTypeNamePair<utility::Reference<int>>("ref[int]");
        stringToType.setTypeNamePair<bool>("bool");

        tree::TreeIO<MAX_SUBROUTINE_ARG_NUM, TypeList> treeIO;

        treeIO.registerNode(std::make_unique<node::PrognNode<int,2>>());
        treeIO.registerNode(std::make_unique<node::AddNode<int>>());
        treeIO.registerNode(std::make_unique<node::SubstitutionNode<int>>());
        treeIO.registerNode(std::make_unique<node::LocalVariableNode<int>>());
        treeIO.registerNode(std::make_unique<node::LocalVariableNode<utility::LeftHandValue<int>>>());
        treeIO.registerNode(std::make_unique<node::ArgumentNode<int,0>>());
        treeIO.registerNode(std::make_unique<node::ArgumentNode<int,1>>());
        treeIO.registerNode(std::make_unique<node::ArgumentNode<utility::Reference<int>, 0>>());
        treeIO.registerNode(std::make_unique<node::ArgumentNode<utility::LeftHandValue<int>, 0>>());
        treeIO.registerNode(std::make_unique<node::ConstNode<int>>(0));
        treeIO.registerNode(std::make_unique<node::GreaterNode<int>>());
        treeIO.registerNode(std::make_unique<node::GreaterEqNode<int>>());
        treeIO.registerNode(std::make_unique<node::LessThanEqNode<int>>());
        treeIO.registerNode(std::make_unique<node::LessThanNode<int>>());
        treeIO.registerNode(std::make_unique<node::EqualNode<int>>());
        treeIO.registerNode(std::make_unique<node::NotEqualNode<int>>());
        treeIO.registerNode(std::make_unique<node::IfNode<int>>());
        treeIO.registerNode(std::make_unique<node::MultiplyNode<int>>());
        treeIO.registerNode(std::make_unique<node::SubtractNode<int>>());
        treeIO.registerNode(std::make_unique<node::ConstNode<bool>>(false));
        treeIO.registerNode(std::make_unique<node::NopNode<int>>());
        treeIO.registerNode(std::make_unique<node::NopNode<bool>>());

        std::stringstream recursionSubTreeStr(std::string("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
                + "<tree>\n"
                + "    <name>Fact[int]</name>\n"
                + "    <return_type>int</return_type>\n"
                + "    <arguments>\n"
                + "        <type>int</type>\n"
                + "    </arguments>\n"
                + "    <local_variables>\n"
                + "    </local_variables>\n"
                + "    <tree_entity>\n"
                + "        +--If[int]\n"
                + "            |\n"
                + "            +--Greater[int]\n"
                + "            |   |\n"
                + "            |   +--Argument[int,0]\n"
                + "            |   |\n"
                + "            |   +--Const[int,1]\n"
                + "            |\n"
                + "            +--Mult[int]\n"
                + "            |   |\n"
                + "            |   +--Argument[int,0]\n"
                + "            |   |\n"
                + "            |   +--Fact[int]\n"
                + "            |       |\n"
                + "            |       +--Sub[int]\n"
                + "            |           |\n"
                + "            |           +--Argument[int,0]\n"
                + "            |           |\n"
                + "            |           +--Const[int,1]\n"
                + "            |\n"
                + "            +--Const[int,1]\n"
                + "    </tree_entity>\n"
                + "</tree>");

        auto subroutineResult = treeIO.loadSubroutine(recursionSubTreeStr, stringToType);
        BOOST_CHECK(subroutineResult);
        auto [factNode, factSubroutineProperty] = std::move(subroutineResult).unwrap();
        BOOST_CHECK(*factSubroutineProperty.returnType == utility::typeInfo<int>());
        BOOST_CHECK_EQUAL(factSubroutineProperty.argumentTypes.size(), 1);
        BOOST_CHECK(*factSubroutineProperty.argumentTypes[0] == utility::typeInfo<int>());
        BOOST_CHECK_EQUAL(factSubroutineProperty.localVariableTypes.size(), 0);
        BOOST_CHECK_EQUAL(factSubroutineProperty.name, "Fact[int]");

        factNode->setChild(0, node::NodeInterface::createInstance<node::ArgumentNode<int, 0>>());
        tree::Tree tree(std::move(factSubroutineProperty), std::move(factNode));

        int acc = 1;
        for(int i = 0; i < 10; ++i) {
            auto ans = tree.evaluate(std::make_tuple(i));
            BOOST_CHECK(ans.getEvaluationStatus() == utility::EvaluationStatus::ValueReturned);
            BOOST_CHECK_EQUAL(std::any_cast<int>(ans.getReturnValue()), acc);
            acc *= i + 1;
        }
    }
    BOOST_AUTO_TEST_CASE(reference_test) {
        utility::StringToType stringToType;
        stringToType.setTypeNamePair<int>("int");
        stringToType.setTypeNamePair<utility::LeftHandValue<int>>("lvalue[int]");
        stringToType.setTypeNamePair<utility::Reference<int>>("ref[int]");
        stringToType.setTypeNamePair<bool>("bool");

        tree::TreeIO<MAX_SUBROUTINE_ARG_NUM, TypeList> treeIO;

        treeIO.registerNode(std::make_unique<node::PrognNode<int,2>>());
        treeIO.registerNode(std::make_unique<node::AddNode<int>>());
        treeIO.registerNode(std::make_unique<node::SubstitutionNode<int>>());
        treeIO.registerNode(std::make_unique<node::LocalVariableNode<int>>());
        treeIO.registerNode(std::make_unique<node::LocalVariableNode<utility::LeftHandValue<int>>>());
        treeIO.registerNode(std::make_unique<node::ArgumentNode<int,0>>());
        treeIO.registerNode(std::make_unique<node::ArgumentNode<int,1>>());
        treeIO.registerNode(std::make_unique<node::ArgumentNode<utility::Reference<int>, 0>>());
        treeIO.registerNode(std::make_unique<node::ArgumentNode<utility::LeftHandValue<int>, 0>>());
        treeIO.registerNode(std::make_unique<node::ConstNode<int>>(0));
        treeIO.registerNode(std::make_unique<node::GreaterNode<int>>());
        treeIO.registerNode(std::make_unique<node::GreaterEqNode<int>>());
        treeIO.registerNode(std::make_unique<node::LessThanEqNode<int>>());
        treeIO.registerNode(std::make_unique<node::LessThanNode<int>>());
        treeIO.registerNode(std::make_unique<node::EqualNode<int>>());
        treeIO.registerNode(std::make_unique<node::NotEqualNode<int>>());
        treeIO.registerNode(std::make_unique<node::IfNode<int>>());
        treeIO.registerNode(std::make_unique<node::MultiplyNode<int>>());
        treeIO.registerNode(std::make_unique<node::SubtractNode<int>>());
        treeIO.registerNode(std::make_unique<node::ConstNode<bool>>(false));
        treeIO.registerNode(std::make_unique<node::NopNode<int>>());
        treeIO.registerNode(std::make_unique<node::NopNode<bool>>());
        treeIO.registerNode(std::make_unique<node::NopNode<utility::Reference<int>>>());

        std::stringstream referenceTestSubStr(std::string("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
                                                      + "<tree>\n"
                                                      + "    <name>ReferenceTestSub</name>\n"
                                                      + "    <return_type>int</return_type>\n"
                                                      + "    <arguments>\n"
                                                      + "        <type>ref[int]</type>\n"
                                                      + "    </arguments>\n"
                                                      + "    <local_variables>\n"
                                                      + "    </local_variables>\n"
                                                      + "    <tree_entity>\n"
                                                      + "        +--Substitute[int]\n"
                                                      + "             |\n"
                                                      + "             +--Argument[lvalue[int],0]\n"
                                                      + "             |\n"
                                                      + "             +--Add[int]\n"
                                                      + "                 |\n"
                                                      + "                 +--Const[int,10]\n"
                                                      + "                 |\n"
                                                      + "                 +--Argument[int,0]\n"
                                                      + "    </tree_entity>\n"
                                                      + "</tree>");

        auto subroutineResult = treeIO.loadSubroutine(referenceTestSubStr, stringToType);
        BOOST_CHECK(subroutineResult);
        auto [refereneTestSub, referenceTestSubProperty] = std::move(subroutineResult).unwrap();
        BOOST_CHECK(*referenceTestSubProperty.returnType == utility::typeInfo<int>());
        BOOST_CHECK_EQUAL(referenceTestSubProperty.argumentTypes.size(), 1);
        BOOST_CHECK(*referenceTestSubProperty.argumentTypes[0] == utility::typeInfo<utility::Reference<int>>());
        BOOST_CHECK_EQUAL(referenceTestSubProperty.localVariableTypes.size(), 0);
        BOOST_CHECK_EQUAL(referenceTestSubProperty.name, "ReferenceTestSub");

        {
            std::stringstream referenceTestStr(std::string("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
                                               + "<tree>\n"
                                               + "    <name>ReferenceTest</name>\n"
                                               + "    <return_type>int</return_type>\n"
                                               + "    <arguments>\n"
                                               + "        <type>int</type>\n"
                                               + "    </arguments>\n"
                                               + "    <local_variables>\n"
                                               + "    </local_variables>\n"
                                               + "    <tree_entity>\n"
                                               + "    +--Progn[int,2]\n"
                                               + "        |\n"
                                               + "        +--ReferenceTestSub\n"
                                               + "        |   |\n"
                                               + "        |   +--Argument[ref[int],0]\n"
                                               + "        |\n"
                                               + "        +--Argument[int,0]\n"
                                               + "    </tree_entity>\n"
                                               + "</tree>");

            auto treeResult = treeIO.readTree(referenceTestStr, stringToType);
            BOOST_CHECK(treeResult);
            auto& tree = treeResult.unwrap();

            for (int i = 0; i < 10; ++i) {
                auto ans = tree.evaluate(std::make_tuple(i));
                BOOST_CHECK(ans.getEvaluationStatus() == utility::EvaluationStatus::ValueReturned);
                BOOST_CHECK_EQUAL(std::any_cast<int>(ans.getReturnValue()), i + 10);
            }
        }
        {//illeagal reference is passed to the subroutine node
            std::stringstream referenceTestStr(std::string("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
                                               + "<tree>\n"
                                               + "    <name>ReferenceTest</name>\n"
                                               + "    <return_type>int</return_type>\n"
                                               + "    <arguments>\n"
                                               + "        <type>int</type>\n"
                                               + "    </arguments>\n"
                                               + "    <local_variables>\n"
                                               + "    </local_variables>\n"
                                               + "    <tree_entity>\n"
                                               + "    +--Progn[int,2]\n"
                                               + "        |\n"
                                               + "        +--ReferenceTestSub\n"
                                               + "        |   |\n"
                                               + "        |   +--Nop[ref[int]]\n"
                                               + "        |\n"
                                               + "        +--Argument[int,0]\n"
                                               + "    </tree_entity>\n"
                                               + "</tree>");

            auto treeResult = treeIO.readTree(referenceTestStr, stringToType);
            BOOST_CHECK(treeResult);
            auto& tree = treeResult.unwrap();

            for (int i = 0; i < 10; ++i) {
                auto ans = tree.evaluate(std::make_tuple(i));
                BOOST_CHECK(ans.getEvaluationStatus() == utility::EvaluationStatus::InvalidValue);
            }
        }
    }
BOOST_AUTO_TEST_SUITE_END()
