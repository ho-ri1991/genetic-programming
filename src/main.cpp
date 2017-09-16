#include <iostream>
#include <fstream>
#include <gp/tree/tree.hpp>
#include <boost/type_index.hpp>
#include <gp/node/node.hpp>
#include <gp/tree_operations/tree_operations.hpp>
#include <gp/tree/io.hpp>
using namespace gp;

int main() {
    utility::StringToType stringToType;
    stringToType.setTypeNamePair<int>("int");
    stringToType.setTypeNamePair<utility::LeftHandValue<int>>("lvalue[int]");
    stringToType.setTypeNamePair<utility::Reference<int>>("ref[int]");

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

    {
        std::ifstream fin("ReferenceTestSub.xml");
        auto treeProperty = treeIO.loadSubroutine(fin, stringToType);
    }

    std::ifstream fin("ReferenceTest.xml");
    auto tree = treeIO.readTree(fin, stringToType);

    for(int i = 0; i < 10; ++i) {
        auto ans = tree.evaluate(std::vector<utility::Variable>{i});
        std::cout << std::any_cast<int>(ans.getReturnValue()) << std::endl;
    }

    return 0;
}
