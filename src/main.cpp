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

    tree::TreeIO<int> treeIO;

    treeIO.registerNode(std::make_unique<node::PrognNode<int,2>>());
    treeIO.registerNode(std::make_unique<node::AddNode<int>>());
    treeIO.registerNode(std::make_unique<node::SubstitutionNode<int>>());
    treeIO.registerNode(std::make_unique<node::LocalVariableNode<int,0>>());
    treeIO.registerNode(std::make_unique<node::LocalVariableNode<utility::LeftHandValue<int>,0>>());
    treeIO.registerNode(std::make_unique<node::ArgumentNode<int,0>>());
    treeIO.registerNode(std::make_unique<node::ArgumentNode<int,1>>());
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

    std::ifstream fin("tree.xml");
    auto tree = treeIO.readTree(fin, stringToType);
    std::ofstream fout1("treeOut.xml");
    treeIO.writeTree(tree, fout1);
    fout1.close();
    for(int i = 0; i < 10; ++i) {
        auto ans = tree.evaluate(std::vector<utility::Variable>{i, i});
        std::cout << std::any_cast<int>(ans.getReturnValue()) << std::endl;
    }

    std::ofstream fout("tree.txt");
    tree_operations::writeTree(tree.getRootNode(), fout);

    std::cout<<tree_operations::getHeight(tree.getRootNode())<<std::endl;
    std::cout<<tree_operations::getDepth(tree.getRootNode())<<std::endl;

    return 0;
}
