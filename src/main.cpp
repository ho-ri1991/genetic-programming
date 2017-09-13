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

    node::SubroutineEntitySet subroutineEntitySet;

    node::SubroutineNode<int(int,int)> subroutine("test", subroutineEntitySet);

    tree::TypesToSubroutineNode typesToSubroutineNode;
    typesToSubroutineNode.registerSubroutineNodeType<int, int, int>();

    tree::SubroutineIO<int> subroutineIO;

    node::StringToNode stringToNode;

    stringToNode.registerNode(std::make_unique<node::PrognNode<int,2>>());
    stringToNode.registerNode(std::make_unique<node::AddNode<int>>());
    stringToNode.registerNode(std::make_unique<node::SubstitutionNode<int>>());
    stringToNode.registerNode(std::make_unique<node::LocalVariableNode<int,0>>());
    stringToNode.registerNode(std::make_unique<node::LocalVariableNode<utility::LeftHandValue<int>,0>>());
    stringToNode.registerNode(std::make_unique<node::ArgumentNode<int,0>>());
    stringToNode.registerNode(std::make_unique<node::ArgumentNode<int,1>>());
    stringToNode.registerNode(std::make_unique<node::ConstNode<int>>(0));
    stringToNode.registerNode(std::make_unique<node::GreaterNode<int>>());
    stringToNode.registerNode(std::make_unique<node::GreaterEqNode<int>>());
    stringToNode.registerNode(std::make_unique<node::LessThanEqNode<int>>());
    stringToNode.registerNode(std::make_unique<node::LessThanNode<int>>());
    stringToNode.registerNode(std::make_unique<node::EqualNode<int>>());
    stringToNode.registerNode(std::make_unique<node::NotEqualNode<int>>());
    stringToNode.registerNode(std::make_unique<node::IfNode<int>>());
    stringToNode.registerNode(std::make_unique<node::MultiplyNode<int>>());
    stringToNode.registerNode(std::make_unique<node::SubtractNode<int>>());

    std::ifstream fin("Fact[int].xml");
    auto treeProperty = subroutineIO.load(fin, stringToType, stringToNode);

    auto rootNode = stringToNode(treeProperty.name);

    rootNode->setChild(0, std::make_unique<node::ArgumentNode<int,0>>());

    std::ofstream fout1("Fact[int]_.xml");
    subroutineIO.write(*rootNode, treeProperty, fout1);
    fout1.close();

    tree::Tree tree(tree::TreeProperty{&utility::typeInfo<int>(), {&utility::typeInfo<int>()}, {}, ""}, std::move(rootNode));
    for(int i = 0; i < 10; ++i) {
        auto ans = tree.evaluate(std::vector<utility::Variable>{i});
        std::cout << std::any_cast<int>(ans.getReturnValue()) << std::endl;
    }

    std::ofstream fout("tree.txt");
    tree_operations::writeTree(tree.getRootNode(), fout);

    std::cout<<tree_operations::getHeight(tree.getRootNode())<<std::endl;
    std::cout<<tree_operations::getDepth(tree.getRootNode())<<std::endl;

    return 0;
}
