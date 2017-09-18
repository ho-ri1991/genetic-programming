#include <iostream>
#include <fstream>
#include <random>
#include <gp/tree/tree.hpp>
#include <boost/type_index.hpp>
#include <gp/node/node.hpp>
#include <gp/tree_operations/tree_operations.hpp>
#include <gp/tree/io.hpp>
using namespace gp;

using rnd = std::mt19937;

int main() {
    utility::StringToType stringToType;
    stringToType.setTypeNamePair<int>("int");
    stringToType.setTypeNamePair<utility::LeftHandValue<int>>("lvalue[int]");
    stringToType.setTypeNamePair<utility::Reference<int>>("ref[int]");
    stringToType.setTypeNamePair<bool>("bool");

    rnd Rnd(0);
    std::tuple<std::function<int(rnd&)>, std::function<bool(rnd&)>> random_gens = std::make_tuple(
            std::function<int(rnd&)>([](rnd& RND){return std::uniform_int_distribution<int>(0, 10)(RND);}),
            std::function<bool(rnd&)>([](rnd& RND){return static_cast<bool>(std::uniform_int_distribution<int>(0, 1)(RND));})
    );

    tree::TreeIO<int> treeIO;
    tree::RandomNodeGenerator<rnd, int, bool> randomNodeGenerator(random_gens);

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

    randomNodeGenerator.registerNode(std::make_unique<node::PrognNode<int,2>>());
    randomNodeGenerator.registerNode(std::make_unique<node::AddNode<int>>());
    randomNodeGenerator.registerNode(std::make_unique<node::SubstitutionNode<int>>());
    randomNodeGenerator.registerNode(std::make_unique<node::LocalVariableNode<int,0>>());
    randomNodeGenerator.registerNode(std::make_unique<node::LocalVariableNode<utility::LeftHandValue<int>,0>>());
    randomNodeGenerator.registerNode(std::make_unique<node::ArgumentNode<int,0>>());
    randomNodeGenerator.registerNode(std::make_unique<node::ArgumentNode<int,1>>());
    randomNodeGenerator.registerNode(std::make_unique<node::ArgumentNode<utility::Reference<int>, 0>>());
    randomNodeGenerator.registerNode(std::make_unique<node::ArgumentNode<utility::LeftHandValue<int>, 0>>());
    randomNodeGenerator.registerNode(std::make_unique<node::ConstNode<int>>(0));
    randomNodeGenerator.registerNode(std::make_unique<node::GreaterNode<int>>());
    randomNodeGenerator.registerNode(std::make_unique<node::GreaterEqNode<int>>());
    randomNodeGenerator.registerNode(std::make_unique<node::LessThanEqNode<int>>());
    randomNodeGenerator.registerNode(std::make_unique<node::LessThanNode<int>>());
    randomNodeGenerator.registerNode(std::make_unique<node::EqualNode<int>>());
    randomNodeGenerator.registerNode(std::make_unique<node::NotEqualNode<int>>());
    randomNodeGenerator.registerNode(std::make_unique<node::IfNode<int>>());
    randomNodeGenerator.registerNode(std::make_unique<node::MultiplyNode<int>>());
    randomNodeGenerator.registerNode(std::make_unique<node::SubtractNode<int>>());
    randomNodeGenerator.registerNode(std::make_unique<node::ConstNode<bool>>(false));

    for(int i=0;i<10;++i) {

        auto treeProperty = tree::TreeProperty{&utility::typeInfo<int>(), {&utility::typeInfo<int>()}, {}, "RandomTree"};
        std::cout << "generate tree randomly" << std::endl;
        auto randomTree = tree_operations::generateTreeRandom(treeProperty, randomNodeGenerator, Rnd, 10);

        {
            std::ofstream fout("RandomTree" + std::to_string(i) + ".xml");
            std::cout << "write random tree" << std::endl;
            treeIO.writeTree(*randomTree, treeProperty, fout);
        }
    }


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
