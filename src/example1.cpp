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
#include <gp/gp_manager.hpp>
#include <gp/probrem/probrem.hpp>

using namespace gp;
using RND = std::mt19937;
template <typename T>
using RandomConstGenerator = std::function<T(RND&)>;
template <typename T>
using StringToValue = std::function<T(const std::string&)>;

using TypeTuple = std::tuple<int, bool, double>;
using LvalueTypeTuple = std::tuple<utility::LeftHandValue<int>, utility::LeftHandValue<bool>, utility::LeftHandValue<double>>;
using RefTypeTuple = std::tuple<utility::Reference<int>, utility::Reference<bool>, utility::Reference<double>>;
using NumericTypeTuple = std::tuple<int, double>;

constexpr std::size_t MAX_PROGN_SIZE = 5;
constexpr std::size_t MAX_ARGUMENT_NUM = 2;
constexpr std::size_t MAX_INITIAL_LOCALVARIABLE_NUM = 3;

template <typename T, typename... Args>
std::vector<const utility::TypeInfo*> generateTypeList() {
    if constexpr (sizeof...(Args) > 0) {
        auto tmp = generateTypeList<Args...>();
        tmp.push_back(&utility::typeInfo<T>());
        return tmp;
    } else {
        return std::vector<const utility::TypeInfo*>{&utility::typeInfo<T>()};
    }
}

template <std::size_t MAX_LOCALVARIABLE_NUM, typename ...Args>
auto generateLocalVariableTypesRandom(RND& rnd) {
    std::uniform_int_distribution<std::size_t> dist(0, MAX_LOCALVARIABLE_NUM);
    std::vector<const utility::TypeInfo*> localVariableTypes(dist(rnd) + 1);
    std::uniform_int_distribution<std::size_t> typeDist(0, sizeof...(Args) - 1);
    auto typeList = generateTypeList<Args...>();
    for(int i = 0; i < std::size(localVariableTypes); ++i) {
        localVariableTypes[i] = typeList[typeDist(rnd)];
    }
    return localVariableTypes;
}

struct ProblemSettings {
    static constexpr const char* POPULATION_SIZE_FIELD = "problem.settings.population_size";
    static constexpr const char* MAX_TREE_DEPTH_FIELD = "problem.settings.max_tree_depth";
    static constexpr const char* EVOLUTION_NUM_FIELD = "problem.settings.evolution_num";
    static constexpr const char* MUTATION_RATE_FIELD = "problem.settings.mutation_rate";
    static constexpr const char* CROSSOVER_RATE_FIELD = "problem.settings.crossover_rate";
    std::size_t populationSize;
    std::size_t maxTreeDepth;
    std::size_t evolutionNum;
    double mutationRate;
    double crossoverRate;
};

ProblemSettings loadSettings(const std::string& name) {
    using namespace boost::property_tree;
    ProblemSettings settings;
    std::ifstream fin(name);
    ptree tree;
    xml_parser::read_xml(fin, tree);
    settings.populationSize = boost::lexical_cast<decltype(settings.populationSize)>(tree.get<std::string>(ProblemSettings::POPULATION_SIZE_FIELD));
    settings.maxTreeDepth = boost::lexical_cast<decltype(settings.maxTreeDepth)>(tree.get<std::string>(ProblemSettings::MAX_TREE_DEPTH_FIELD));
    settings.evolutionNum = boost::lexical_cast<decltype(settings.evolutionNum)>(tree.get<std::string>(ProblemSettings::EVOLUTION_NUM_FIELD));
    settings.mutationRate = boost::lexical_cast<decltype(settings.mutationRate)>(tree.get<std::string>(ProblemSettings::MUTATION_RATE_FIELD));
    settings.crossoverRate = boost::lexical_cast<decltype(settings.crossoverRate)>(tree.get<std::string>(ProblemSettings::CROSSOVER_RATE_FIELD));
    return settings;
}

int main(int argc, char* argv[]) {
    //random engine for this example
    RND rnd(0);

    //random value generators for const node
    auto randomConstGenerators = std::make_tuple(
            RandomConstGenerator<int>([](RND& rnd){return std::uniform_int_distribution<int>(-10, 10)(rnd);}),
            RandomConstGenerator<bool>([](RND& rnd){return static_cast<bool>(std::uniform_int_distribution<int>(0, 1)(rnd));}),
            RandomConstGenerator<double>([](RND& rnd){return std::uniform_real_distribution<double>(-10, 10)(rnd);})
    );

    //string to value functions for problem io
    auto stringToValues = std::make_tuple(
            StringToValue<int>([](const std::string& str){return boost::lexical_cast<int>(str);}),
            StringToValue<bool>([](const std::string& str){return boost::lexical_cast<bool>(str);}),
            StringToValue<double>([](const std::string& str){return boost::lexical_cast<double>(str);})
    );

    GPManager<MAX_ARGUMENT_NUM, MAX_PROGN_SIZE, TypeTuple> gpManager("int", "bool", "double");
    gpManager.registerNodes<node::AddNode, TypeTuple>();
    gpManager.registerNodes<node::SubtractNode, TypeTuple>();
    gpManager.registerNodes<node::MultiplyNode, TypeTuple>();
    gpManager.registerNodes<node::DivisionNode, TypeTuple>();
    gpManager.registerNodes<node::GreaterEqNode, TypeTuple>();
    gpManager.registerNodes<node::GreaterNode, TypeTuple>();
    gpManager.registerNodes<node::LessThanEqNode, TypeTuple>();
    gpManager.registerNodes<node::LessThanNode, TypeTuple>();
    gpManager.registerNodes<node::EqualNode, TypeTuple>();
    gpManager.registerNodes<node::NotEqualNode, TypeTuple>();
    gpManager.registerNodes<node::IfNode, TypeTuple>();
    gpManager.registerNodes<node::NopNode, TypeTuple>();
    gpManager.registerNodes<node::ConstNode, TypeTuple>();
    gpManager.registerNodes<node::SubstitutionNode, TypeTuple>();
    gpManager.registerNodes<node::NopNode, TypeTuple>();
    gpManager.registerNodes<node::NopNode, LvalueTypeTuple>();
    gpManager.registerNodes<node::NopNode, RefTypeTuple>();

    genetic_operations::DefaultRandomNodeGenerator<RND, TypeTuple> randomNodeGenerator(randomConstGenerators);

    randomNodeGenerator.registerNodes<node::AddNode, NumericTypeTuple>();
    randomNodeGenerator.registerNodes<node::SubtractNode, NumericTypeTuple>();
    randomNodeGenerator.registerNodes<node::MultiplyNode, NumericTypeTuple>();
    randomNodeGenerator.registerNodes<node::GreaterNode, NumericTypeTuple>();
    randomNodeGenerator.registerNodes<node::GreaterEqNode, NumericTypeTuple>();
    randomNodeGenerator.registerNodes<node::LessThanNode, NumericTypeTuple>();
    randomNodeGenerator.registerNodes<node::LessThanEqNode, NumericTypeTuple>();
    randomNodeGenerator.registerNodes<node::IfNode, TypeTuple>();
    randomNodeGenerator.registerNodes<node::NopNode, TypeTuple>();
    randomNodeGenerator.registerNodes<node::NopNode, LvalueTypeTuple>();
    randomNodeGenerator.registerNodes<node::NopNode, RefTypeTuple>();
    randomNodeGenerator.registerNodes<node::ConstNode, TypeTuple>();
    randomNodeGenerator.registerNodes<node::LocalVariableNode, TypeTuple>();
    randomNodeGenerator.registerNodes<node::LocalVariableNode, LvalueTypeTuple>();
    randomNodeGenerator.registerNodes<node::LocalVariableNode, RefTypeTuple>();
    randomNodeGenerator.registerNodes<node::SubstitutionNode, TypeTuple>();

    randomNodeGenerator.registerNode<node::ArgumentNode<int, 0>>();
    randomNodeGenerator.registerNode<node::ArgumentNode<int, 1>>();
    randomNodeGenerator.registerNode<node::ArgumentNode<int, 2>>();
    randomNodeGenerator.registerNode<node::ArgumentNode<double, 0>>();
    randomNodeGenerator.registerNode<node::ArgumentNode<double, 1>>();
    randomNodeGenerator.registerNode<node::ArgumentNode<double, 2>>();
    randomNodeGenerator.registerNode<node::ArgumentNode<bool, 0>>();
    randomNodeGenerator.registerNode<node::ArgumentNode<bool, 1>>();
    randomNodeGenerator.registerNode<node::ArgumentNode<bool, 2>>();
    randomNodeGenerator.registerNode<node::PrognNode<int, 3>>();
    randomNodeGenerator.registerNode<node::PrognNode<int, 4>>();
    randomNodeGenerator.registerNode<node::PrognNode<bool, 3>>();
    randomNodeGenerator.registerNode<node::PrognNode<bool, 4>>();
    randomNodeGenerator.registerNode<node::PrognNode<double, 3>>();
    randomNodeGenerator.registerNode<node::PrognNode<double, 4>>();

    if(argc < 2) {
        std::cout<<"usage: example problem_file1 problem_file2 ..."<<std::endl;
    }

    const std::size_t fileNum = argc - 1;

    for(int fileCnt = 0; fileCnt < fileNum; ++fileCnt){
        const auto settings = loadSettings(argv[fileCnt + 1]);
        std::ifstream fin(argv[fileCnt + 1]);
        auto problem1 = problem::load(fin, gpManager.getStringToType(), stringToValues);
        fin.close();
        if(MAX_ARGUMENT_NUM < problem1.argumentTypes.size()) {
            std::cout << "the problem " << problem1.name << "exceeds max argument num.\n";
            std::cout << "the maximam number of arguments is" << MAX_ARGUMENT_NUM << '\n';
            std::cout << "skip this problem." <<std::endl;
        } else {
            std::cout << "==============================\n";
            std::cout << "problem:" << problem1.name << '\n';
            std::cout << "return_type: " << problem1.returnType->name() << '\n';
            std::cout << "argument_types: ";
            for(int i = 0; i < problem1.argumentTypes.size(); ++i){
                std::cout << problem1.argumentTypes[i]->name() << ", ";
            }
            std::cout << "popultion size: " << settings.populationSize << '\n';
            std::cout << "max tree depth: " <<settings.maxTreeDepth << '\n';
            std::cout << "evoluation num: " << settings.evolutionNum << '\n';
            std::cout << "mutation rate: " << settings.mutationRate << '\n';
            std::cout << "crossover rate: " << settings.crossoverRate << '\n';
            std::cout << std::flush;

            std::vector<tree::Tree> population;
            population.reserve(settings.populationSize);
            for(int i = 0; i < settings.populationSize; ++i) {
                auto treeProperty = tree::TreeProperty{problem1.returnType, problem1.argumentTypes, generateLocalVariableTypesRandom<MAX_INITIAL_LOCALVARIABLE_NUM, int, bool, double>(rnd)};
                auto rootNode = tree_operations::generateTreeRandom(treeProperty, randomNodeGenerator, rnd, settings.maxTreeDepth);
                population.emplace_back(std::move(treeProperty), std::move(rootNode));
            }

            for(int i = 0; i < population.size(); ++i) {
                std::ofstream fout(problem1.name + "_random_tree" + std::to_string(i) + ".xml");
                population[i].getTreeProperty().name = problem1.name;
                gpManager.writeTree(population[i], fout);
                fout.close();
            }
        }
    }

    return 0;
}
