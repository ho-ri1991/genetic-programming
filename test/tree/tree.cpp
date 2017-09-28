#define BOOST_TEST_NO_LIB
#define BOOST_TEST_MAIN

#include <gp/node/node.hpp>
#include <gp/tree/tree.hpp>
#include <boost/test/unit_test.hpp>
using namespace gp;

BOOST_AUTO_TEST_SUITE(gplib)
    BOOST_AUTO_TEST_CASE(gplib) {
        auto var0 = node::NodeInterface::createInstance<node::LocalVariableNode<int, 0>>();
        auto arg1 = node::NodeInterface::createInstance<node::ArgumentNode<int, 1>>();
        auto add1 = node::NodeInterface::createInstance<node::AddNode<int>>();

        add1->setChild(0, std::move(arg1));
        add1->setChild(1, std::move(var0));

        auto c1 = node::NodeInterface::createInstance<node::ConstNode<int>>(10);
        auto add2 = node::NodeInterface::createInstance<node::AddNode<int>>();

        add2->setChild(0, std::move(c1));
        add2->setChild(1, std::move(add1));

        auto lvar0 = node::NodeInterface::createInstance<node::LocalVariableNode<utility::LeftHandValue<int>, 0>>();
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

        tree::Tree copyTree = tree;

        BOOST_CHECK_EQUAL(copyTree.getArgumentNum(), tree.getArgumentNum());
        BOOST_CHECK_EQUAL(copyTree.getLocalVariableNum(), tree.getLocalVariableNum());
        BOOST_CHECK(tree.getReturnType() == copyTree.getReturnType());
        for(int i = 0; i < tree.getArgumentNum(); ++i) {
            BOOST_CHECK(tree.getArgumentType(i) == copyTree.getArgumentType(i));
        }
        for(int i = 0; i < tree.getLocalVariableNum(); ++i) {
            BOOST_CHECK(tree.getLocalVariableType(i) == copyTree.getLocalVariableType(i));
        }

        for(int i = -10; i < 10; ++i) {
            int a0 = i;
            int a1 = 2 * i;
            auto ans = copyTree.evaluate(std::make_tuple(a0, a1));
            BOOST_CHECK_EQUAL(static_cast<int>(ans.getEvaluationStatus()), static_cast<int>(utility::EvaluationStatus::ValueReturned));
            BOOST_CHECK_EQUAL(std::any_cast<int>(ans.getReturnValue()), 10 + a0 + a1);
        }

        copyTree.getRootNode().getChild(1).getChild(0).setNodePropertyByAny(5);
        for(int i = -10; i < 10; ++i) {
            int a0 = i;
            int a1 = 2 * i;
            auto ans = tree.evaluate(std::make_tuple(a0, a1));
            BOOST_CHECK_EQUAL(static_cast<int>(ans.getEvaluationStatus()), static_cast<int>(utility::EvaluationStatus::ValueReturned));
            BOOST_CHECK_EQUAL(std::any_cast<int>(ans.getReturnValue()), 10 + a0 + a1);
        }
        for(int i = -10; i < 10; ++i) {
            int a0 = i;
            int a1 = 2 * i;
            auto ans = copyTree.evaluate(std::make_tuple(a0, a1));
            BOOST_CHECK_EQUAL(static_cast<int>(ans.getEvaluationStatus()), static_cast<int>(utility::EvaluationStatus::ValueReturned));
            BOOST_CHECK_EQUAL(std::any_cast<int>(ans.getReturnValue()), 5 + a0 + a1);
        }
    }
BOOST_AUTO_TEST_SUITE_END()
