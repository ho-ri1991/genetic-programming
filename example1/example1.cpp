#include <iostream>
#include <fstream>
#include <random>
#include <unordered_map>
#include <gp/tree/tree.hpp>
#include <boost/type_index.hpp>
#include <gp/node/node.hpp>
#include <gp/tree_operations/tree_operations.hpp>
#include <gp/tree/io.hpp>
#include <gp/genetic_operations/mutation.hpp>
#include <gp/genetic_operations/default_modules.hpp>
#include <gp/gp_manager.hpp>
#include <gp/problem/problem.hpp>
#include <gp/utility/result.hpp>

using namespace gp;
using RND = std::mt19937;
template <typename T>
using RandomConstGenerator = std::function<T(RND&)>;
template <typename T>
using StringToValue = std::function<T(const std::string&)>;
using Evaluator = std::function<double(utility::EvaluationContext&, const utility::Variable&)>;

//settings for genetic programming
using TypeTuple = std::tuple<int, bool, double>;
using LvalueTypeTuple = std::tuple<utility::LeftHandValue<int>, utility::LeftHandValue<bool>, utility::LeftHandValue<double>>;
using RefTypeTuple = std::tuple<utility::Reference<int>, utility::Reference<bool>, utility::Reference<double>>;
using NumericTypeTuple = std::tuple<int, double>;
const std::vector<const utility::TypeInfo*> localVariableTypeArray = {&utility::typeInfo<int>(), &utility::typeInfo<bool>(), &utility::typeInfo<double>()};

constexpr std::size_t MAX_PROGN_SIZE = 5;
constexpr std::size_t MAX_ARGUMENT_NUM = 2;
constexpr std::size_t MAX_INITIAL_LOCALVARIABLE_NUM = 3;

template <std::size_t MAX_LOCALVARIABLE_NUM = MAX_INITIAL_LOCALVARIABLE_NUM>
auto generateLocalVariableTypesRandom(RND& rnd) {
    std::uniform_int_distribution<std::size_t> dist(0, MAX_LOCALVARIABLE_NUM);
    std::vector<const utility::TypeInfo*> localVariableTypes(dist(rnd) + 1);
    std::uniform_int_distribution<std::size_t> typeDist(0, std::size(localVariableTypeArray) - 1);
    for(int i = 0; i < std::size(localVariableTypes); ++i) {
        localVariableTypes[i] = localVariableTypeArray[typeDist(rnd)];
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

utility::Result<ProblemSettings> loadSettings(const std::string& name) {
    using namespace boost::property_tree;
    using namespace utility;
    ProblemSettings settings;
    std::ifstream fin(name);
    if(!fin.is_open())return result::err<ProblemSettings>("failed to load problem, file: " + name + " not found");
    ptree tree;
    try {
        xml_parser::read_xml(fin, tree);
    } catch (boost::property_tree::xml_parser_error& ex){
        return result::err<ProblemSettings>(std::string("failed to load problem, some error found in the xml file\n") + "line " + std::to_string(ex.line()) + ": " + ex.message());
    }

    return result::sequence(result::fromOptional(tree.get_optional<std::string>(ProblemSettings::POPULATION_SIZE_FIELD), "failed to load problem, population_size field not found"),
                            result::fromOptional(tree.get_optional<std::string>(ProblemSettings::MAX_TREE_DEPTH_FIELD), "failed to load problem, max_tree_depth field not found"),
                            result::fromOptional(tree.get_optional<std::string>(ProblemSettings::EVOLUTION_NUM_FIELD), "failed to load problem, evolution_num field not found"),
                            result::fromOptional(tree.get_optional<std::string>(ProblemSettings::MUTATION_RATE_FIELD), "failed to load problem, mutation_rate field not found"),
                            result::fromOptional(tree.get_optional<std::string>(ProblemSettings::CROSSOVER_RATE_FIELD), "failed to load problem, crossover_rate field not found"))
            .flatMap([](auto&& propertyStrings){
                auto& populationNum = std::get<0>(propertyStrings);
                auto& maxTreeDepth = std::get<1>(propertyStrings);
                auto& evolutionNum = std::get<2>(propertyStrings);
                auto& mutationRate = std::get<3>(propertyStrings);
                auto& crossoverRate = std::get<4>(propertyStrings);
                return result::sequence(result::tryFunction([&populationNum](){return boost::lexical_cast<decltype(ProblemSettings{}.populationSize)>(populationNum);}, "failed to load problem, invalid population_size field: " + populationNum),
                                        result::tryFunction([&maxTreeDepth](){return boost::lexical_cast<decltype(ProblemSettings{}.maxTreeDepth)>(maxTreeDepth);}, "failed to load problem, invalid max_tree_depth field: " + maxTreeDepth),
                                        result::tryFunction([&evolutionNum](){return boost::lexical_cast<decltype(ProblemSettings{}.evolutionNum)>(evolutionNum);}, "failed to load problem, invalid evoluion_num field: " + evolutionNum),
                                        result::tryFunction([&mutationRate](){return boost::lexical_cast<decltype(ProblemSettings{}.mutationRate)>(mutationRate);}, "failed to load problem, invalid mutation_rate field: " + mutationRate),
                                        result::tryFunction([&crossoverRate](){return boost::lexical_cast<decltype(ProblemSettings{}.crossoverRate)>(crossoverRate);}, "failed to load problem, invalid crossover_rate field: " + crossoverRate));
            }).map([](auto&& prop){
                return ProblemSettings{std::get<0>(prop), std::get<1>(prop), std::get<2>(prop), std::get<3>(prop), std::get<4>(prop)};
            });
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

    //evaluators
    std::unordered_map<utility::TypeIndex, Evaluator, utility::TypeIndex::Hash> evaluators;
    Evaluator intEvaluator = [](utility::EvaluationContext& actual, const utility::Variable& expected)->double {
        static constexpr double err = 1e5;
        if(actual.getEvaluationStatus() != utility::EvaluationStatus::ValueReturned) return err;
        auto a = std::any_cast<int>(actual.getReturnValue());
        auto b = expected.get<int>();
        return b != 0 ? std::abs(a - b)/std::abs(b) : std::abs(a - b);
    };
    evaluators.insert(std::make_pair(utility::TypeIndex(utility::typeInfo<int>()), intEvaluator));
    Evaluator boolEvaluator = [](utility::EvaluationContext& actual, const utility::Variable& expected)->double{
        static constexpr double err = 1e5;
        if(actual.getEvaluationStatus() != utility::EvaluationStatus::ValueReturned)return err;
        return static_cast<double>(std::any_cast<bool>(actual.getReturnValue()) != expected.get<bool>());
    };
    evaluators.insert(std::make_pair(utility::TypeIndex(utility::typeInfo<bool>()), boolEvaluator));
    Evaluator doubleEvaluator = [](utility::EvaluationContext& actual, const utility::Variable& expected)->double{
        static constexpr double err = 1e5;
        if(actual.getEvaluationStatus() != utility::EvaluationStatus::ValueReturned)return err;
        static constexpr double e = 1e-1;
        auto a = std::any_cast<double>(actual.getReturnValue());
        auto b = expected.get<double>();
        return b > e ? std::abs(a - b)/std::abs(b) : std::abs(a - b);
    };
    evaluators.insert(std::make_pair(utility::TypeIndex(utility::typeInfo<double>()), doubleEvaluator));

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
    randomNodeGenerator.registerNode<node::PrognNode<bool, 3>>();
    randomNodeGenerator.registerNode<node::PrognNode<double, 3>>();

    if(argc < 2) {
        std::cout<<"usage: example problem_file1 problem_file2 ..."<<std::endl;
    }

    const std::size_t fileNum = argc - 1;

    for(int fileCnt = 0; fileCnt < fileNum; ++fileCnt){
        auto settingResult = loadSettings(argv[fileCnt + 1]);
        auto fileName = argv[fileCnt + 1];
        std::ifstream fin(fileName);
        auto problemResult = problem::load(fin, gpManager.getStringToType(), stringToValues)
                .flatMap([maxArgumentNum = MAX_ARGUMENT_NUM](problem::Problem&& problem_){
                    return problem_.argumentTypes.size() <= maxArgumentNum ?
                           utility::result::ok(std::move(problem_)) :
                           utility::result::err<problem::Problem>("failed to load problem, the problem exceeds max argument num, the maximum number of arguments is " + std::to_string(maxArgumentNum));
                }).flatMap([&evaluators](problem::Problem&& problem_){
                    auto itr = evaluators.find(utility::TypeIndex(*problem_.returnType));
                    using AnsType = decltype(std::make_tuple(std::move(problem_), itr));
                    return itr != std::end(evaluators) ?
                           utility::result::ok(std::make_tuple(std::move(problem_), itr)) :
                           utility::result::err<AnsType>("failed to load problem, evaluator not found for type " + problem_.returnType->name());
                });
        fin.close();
        if(!problemResult || !settingResult) {
            std::cout << utility::result::sequence(std::move(problemResult), std::move(settingResult)).errMessage() << '\n';
            std::cout << "skip problem: " << fileName << std::endl;
            continue;
        }
        auto& settings = settingResult.unwrap();
        auto& [problem1, evaluatorItr] = problemResult.unwrap();
        auto& evaluator = evaluatorItr->second;
        std::cout << "==============================\n";
        std::cout << "problem:" << problem1.name << '\n';
        std::cout << "return_type: " << problem1.returnType->name() << '\n';
        std::cout << "argument_types: ";
        for(int i = 0; i < problem1.argumentTypes.size(); ++i){
            std::cout << problem1.argumentTypes[i]->name() << ", ";
        }
        std::cout << "population size: " << settings.populationSize << '\n';
        std::cout << "max tree depth: " <<settings.maxTreeDepth << '\n';
        std::cout << "evolution num: " << settings.evolutionNum << '\n';
        std::cout << "mutation rate: " << settings.mutationRate << '\n';
        std::cout << "crossover rate: " << settings.crossoverRate << '\n';
        std::cout << std::flush;

        //create initial population
        std::vector<std::tuple<tree::Tree, double>> population;
        population.reserve(settings.populationSize);
        for(int i = 0; i < settings.populationSize; ++i) {
            auto treeProperty = tree::TreeProperty{problem1.returnType, problem1.argumentTypes, generateLocalVariableTypesRandom<>(rnd)};
            auto rootNode = tree_operations::generateTreeRandom(treeProperty, randomNodeGenerator, rnd, settings.maxTreeDepth);
            population.push_back(std::make_tuple(tree::Tree(std::move(treeProperty), std::move(rootNode)), 0.));
        }

        for(std::size_t count = 0; count < settings.evolutionNum; ++count){
            std::cout << "====" << std::to_string(count) << "th generation" << "====\n";
            std::vector<std::tuple<std::size_t, double>> idxEvalPairs(std::size(population));
            //evaluation for each tree
            for(auto& [tree, inverseFitness]: population){
                inverseFitness = 0;
                //evaluation for each data set
                for(std::size_t i = 0; i < std::size(problem1.ansArgList); ++i) {
                    const auto& [answer, arguments] = problem1.ansArgList[i];
                    auto evaluatedValue = tree.evaluate(arguments);
                    inverseFitness += evaluator(evaluatedValue, answer);
                }
            }
            //order by inverse of fitness
            std::sort(std::begin(population), std::end(population), [](const auto& x, const auto& y){return std::get<1>(x) < std::get<1>(y);});

            //create next generation
            std::vector<std::tuple<tree::Tree, double>> nextPopulation;
            nextPopulation.reserve(std::size(population));
        }

        for(int i = 0; i < population.size(); ++i) {
            std::ofstream fout(problem1.name + "_random_tree" + std::to_string(i) + ".xml");
            auto& [tree, inverseFitness] = population[i];
            tree.getTreeProperty().name = problem1.name;
            gpManager.writeTree(tree, fout);
            fout.close();
        }
    }

    return 0;
}
