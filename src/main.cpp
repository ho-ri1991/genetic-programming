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
    auto subtruct = std::make_unique<node::SubtractNode<int>>();
    auto mult = std::make_unique<node::MultiplyNode<int>>();
    auto progn = std::make_unique<node::PrognNode<int, 2>>();
    auto add = std::make_unique<node::AddNode<int>>();
    auto add1 = std::make_unique<node::AddNode<int>>();
    auto subst = std::make_unique<node::SubstitutionNode<int>>();
    auto lovalVar = std::make_unique<node::LocalVariableNode<int, 0>>();
    auto localVarL = std::make_unique<node::LocalVariableNode<utility::LeftHandValue<int>, 0>>();
    auto a1 = std::make_unique<node::ArgumentNode<int, 0>>();
    auto a2 = std::make_unique<node::ArgumentNode<int, 1>>();
    auto c1 = std::make_unique<node::ConstNode<int>>(10);


    stringToNode.registerNode(subtruct->clone());
    stringToNode.registerNode(mult->clone());
    stringToNode.registerNode(progn->clone());
    stringToNode.registerNode(add->clone());
    stringToNode.registerNode(subst->clone());
    stringToNode.registerNode(localVarL->clone());
    stringToNode.registerNode(lovalVar->clone());
    stringToNode.registerNode(a1->clone());
    stringToNode.registerNode(a2->clone());
    stringToNode.registerNode(c1->clone());
    stringToNode.registerNode(std::make_unique<node::GreaterNode<int>>());
    stringToNode.registerNode(std::make_unique<node::GreaterEqNode<int>>());
    stringToNode.registerNode(std::make_unique<node::LessThanEqNode<int>>());
    stringToNode.registerNode(std::make_unique<node::LessThanNode<int>>());
    stringToNode.registerNode(std::make_unique<node::EqualNode<int>>());
    stringToNode.registerNode(std::make_unique<node::NotEqualNode<int>>());

    std::ifstream fin("tree.xml");
    auto treeProperty = subroutineIO.load(fin, stringToType, stringToNode);

    auto rootNode = stringToNode(treeProperty.name);

    rootNode->setChild(0, a1->clone());
    rootNode->setChild(1, a2->clone());

    std::ofstream fout1("tree_.xml");
    subroutineIO.write(*rootNode, treeProperty, fout1);
    fout1.close();

    tree::Tree tree(typeid(int), std::vector<const std::type_info*>{&typeid(int), &typeid(int)}, std::vector<const std::type_info*>{&typeid(int)}, std::move(rootNode));
    auto ans = tree.evaluate(std::vector<utility::Variable>{1, 2});
    std::cout<<std::any_cast<int>(ans.getReturnValue())<<std::endl;

    std::ofstream fout("tree.txt");
    tree_operations::writeTree(tree.getRootNode(), fout);

    std::cout<<tree_operations::getHeight(tree.getRootNode())<<std::endl;
    std::cout<<tree_operations::getDepth(tree.getRootNode())<<std::endl;

    return 0;
}
