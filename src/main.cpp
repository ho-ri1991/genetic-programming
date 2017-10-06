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
using namespace gp;

using rnd = std::mt19937;

constexpr std::size_t maxTreeDepth = 5;

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
    genetic_operations::DefaultRandomNodeGenerator<rnd, int, bool> randomNodeGenerator(random_gens);
    genetic_operations::DefaultRandomTreeGenerator<rnd, decltype(randomNodeGenerator)> defaultRandomTreeGenerator(Rnd, randomNodeGenerator);
    genetic_operations::DefaultNodeSelector<rnd> defaultNodeSelector(Rnd, maxTreeDepth);

    treeIO.registerNode(std::make_unique<node::PrognNode<int,2>>());
    treeIO.registerNode(std::make_unique<node::AddNode<int>>());
    treeIO.registerNode(std::make_unique<node::SubstitutionNode<int>>());
    treeIO.registerNode(std::make_unique<node::LocalVariableNode<int>>());
    treeIO.registerNode(std::make_unique<node::LocalVariableNode<utility::LeftHandValue<int>>>());
    treeIO.registerNode(std::make_unique<node::LocalVariableNode<utility::Reference<int>>>());
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

//    randomNodeGenerator.registerNode(std::make_unique<node::PrognNode<int,2>>());
    randomNodeGenerator.registerNode(std::make_unique<node::AddNode<int>>());
    randomNodeGenerator.registerNode(std::make_unique<node::SubstitutionNode<int>>());
    randomNodeGenerator.registerNode(std::make_unique<node::LocalVariableNode<int>>());
    randomNodeGenerator.registerNode(std::make_unique<node::LocalVariableNode<utility::LeftHandValue<int>>>());
    randomNodeGenerator.registerNode(std::make_unique<node::LocalVariableNode<utility::Reference<int>>>());
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
    randomNodeGenerator.registerNode(std::make_unique<node::NopNode<int>>());
    randomNodeGenerator.registerNode(std::make_unique<node::NopNode<bool>>());

    std::ifstream fin("tree.xml");
    auto tree = treeIO.readTree(fin, stringToType);

    for(int i = 0; i < 10; ++i) {
        auto mutatedTree = genetic_operations::mutation(tree, defaultRandomTreeGenerator, defaultNodeSelector, maxTreeDepth);

        std::ofstream fout("mutation-tree" + std::to_string(i) + ".xml");
        mutatedTree.getTreeProperty().name = "MutationTree";
        treeIO.writeTree(mutatedTree, fout);
        fout.close();

        auto ans = mutatedTree.evaluate(std::make_tuple(1, 2));
        std::cout << std::any_cast<int>(ans.getReturnValue()) << std::endl;
    }


    return 0;
}
