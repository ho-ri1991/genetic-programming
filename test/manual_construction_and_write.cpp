#define BOOST_TEST_MAIN
#include <iostream>
#include <fstream>
#include <random>
#include <gp/tree/tree.hpp>
#include <boost/type_index.hpp>
#include <gp/node/node.hpp>
#include <gp/tree_operations/tree_operations.hpp>
#include <gp/tree/io.hpp>
#include <gp/genetic_operations/mutation.hpp>
#include <gp/genetic_operations/default_modules.hpp>
#include <boost/test/included/unit_test.hpp>
using namespace gp;


BOOST_AUTO_TEST_SUITE(gplib)
BOOST_AUTO_TEST_CASE(gplib) {
    utility::StringToType stringToType;
    stringToType.setTypeNamePair<int>("int");
    stringToType.setTypeNamePair<utility::LeftHandValue<int>>("lvalue[int]");
    stringToType.setTypeNamePair<utility::Reference<int>>("ref[int]");
    stringToType.setTypeNamePair<bool>("bool");

    tree::TreeIO<int> treeIO;

    treeIO.registerNode(std::make_unique<node::PrognNode<int,2>>());
    treeIO.registerNode(std::make_unique<node::AddNode<int>>());
    treeIO.registerNode(std::make_unique<node::SubstitutionNode<int>>());
    treeIO.registerNode(std::make_unique<node::LocalVariableNode<int,0>>());
    treeIO.registerNode(std::make_unique<node::LocalVariableNode<utility::LeftHandValue<int>,0>>());
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

    std::ofstream fout("TestTree.xml");
    treeIO.writeTree(tree, fout);
}
BOOST_AUTO_TEST_SUITE_END()
