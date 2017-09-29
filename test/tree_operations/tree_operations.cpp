#define BOOST_TEST_NO_LIB
#define BOOST_TEST_MAIN

#include <gp/node/node.hpp>
#include <gp/tree/tree.hpp>
#include <gp/tree_operations/tree_operations.hpp>
#include <boost/test/unit_test.hpp>
using namespace gp;

BOOST_AUTO_TEST_SUITE(tree_operations_test)
    BOOST_AUTO_TEST_CASE(tree_operations_test) {
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

        BOOST_CHECK_EQUAL(tree_operations::getDepth(progn), 0);
        BOOST_CHECK_EQUAL(tree_operations::getDepth(*progn), 0);
        BOOST_CHECK_EQUAL(tree_operations::getHeight(progn), 3);
        BOOST_CHECK_EQUAL(tree_operations::getHeight(*progn), 3);
        BOOST_CHECK_EQUAL(tree_operations::getSubtreeNodeNum(progn), 9);
        BOOST_CHECK_EQUAL(tree_operations::getSubtreeNodeNum(*progn), 9);

        BOOST_CHECK_EQUAL(tree_operations::getDepth(progn->getChild(0)), 1);
        BOOST_CHECK_EQUAL(tree_operations::getHeight(progn->getChild(0)), 1);
        BOOST_CHECK_EQUAL(tree_operations::getSubtreeNodeNum(progn->getChild(0)), 3);
    }
BOOST_AUTO_TEST_SUITE_END()
