#include <iostream>
#include <fstream>
#include <random>
#include <unordered_map>
#include <gp/tree/tree.hpp>
#include <gp/node/node.hpp>
#include <gp/tree_operations/tree_operations.hpp>
#include <gp/tree/io.hpp>
#include <gp/genetic_operations/mutation.hpp>
#include <gp/genetic_operations/crossover.hpp>
#include <gp/genetic_operations/default_modules.hpp>
#include <gp/gp_manager.hpp>
#include <gp/problem/problem.hpp>
#include <gp/utility/result.hpp>
#include "problem_settings.hpp"

using namespace gp;
using namespace gp::example1;
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
constexpr std::size_t MAX_ARGUMENT_NUM = 3;
constexpr std::size_t MAX_INITIAL_LOCALVARIABLE_NUM = 3;
constexpr std::size_t MAX_OUTPUT_TREE_NUM = 5;

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
    std::unordered_map<utility::TypeIndex, Evaluator, utility::TypeIndex::Hash> evaluators = {
            {utility::TypeIndex(utility::typeInfo<int>()), Evaluator([](utility::EvaluationContext& actual, const utility::Variable& expected)->double {
                static constexpr double err = 1e5;
                if(actual.getEvaluationStatus() != utility::EvaluationStatus::ValueReturned) return err;
                auto a = std::any_cast<int>(actual.getReturnValue());
                auto b = expected.get<int>();
                return b != 0 ? static_cast<double>(std::abs(a - b))/std::abs(b) : std::abs(a - b);
            })},
            {utility::TypeIndex(utility::typeInfo<bool>()), Evaluator([](utility::EvaluationContext& actual, const utility::Variable& expected)->double{
                static constexpr double err = 1e5;
                if(actual.getEvaluationStatus() != utility::EvaluationStatus::ValueReturned)return err;
                return static_cast<double>(std::any_cast<bool>(actual.getReturnValue()) != expected.get<bool>());
            })},
            {utility::TypeIndex(utility::typeInfo<double>()), Evaluator([](utility::EvaluationContext& actual, const utility::Variable& expected)->double{
                static constexpr double err = 1e5;
                if(actual.getEvaluationStatus() != utility::EvaluationStatus::ValueReturned)return err;
                static constexpr double e = 1e-1;
                auto a = std::any_cast<double>(actual.getReturnValue());
                auto b = expected.get<double>();
                return b > e ? std::abs(a - b)/std::abs(b) : std::abs(a - b);
            })}
    };

    GPManager<MAX_ARGUMENT_NUM, MAX_PROGN_SIZE, TypeTuple> gpManager("int", "bool", "double");
    //register nodes for read tree from file, argument and local variable, progn nodes are registered as default.
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
    gpManager.registerNode<node::RepeatNode<int, int>>();
    gpManager.registerNode<node::RepeatNode<bool, int>>();
    gpManager.registerNode<node::RepeatNode<double, int>>();
    gpManager.registerNode<node::DivisionNode<int>>();
    gpManager.registerNode<node::IncrementNode<int>>();

    genetic_operations::DefaultRandomNodeGenerator<RND, TypeTuple> randomNodeGenerator(randomConstGenerators);

    //register nodes for genetic operations (mutation, initial population)
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
    randomNodeGenerator.registerNode<node::RepeatNode<int, int>>();
    randomNodeGenerator.registerNode<node::RepeatNode<bool, int>>();
    randomNodeGenerator.registerNode<node::RepeatNode<double, int>>();

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
    randomNodeGenerator.registerNode<node::DivisionNode<int>>();
    randomNodeGenerator.registerNode<node::IncrementNode<int>>();

    if(argc < 2) {
        std::cout<<"usage: example problem_file1 problem_file2 ..."<<std::endl;
    }

    const std::size_t fileNum = argc - 1;

    for(int fileCnt = 0; fileCnt < fileNum; ++fileCnt){
        auto fileName = argv[fileCnt + 1];
        auto settingResult = loadSettings<RND>(fileName);
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
        std::cout << '\n';
        std::cout << "population size: " << settings.populationSize << '\n';
        std::cout << "max tree depth: " <<settings.maxTreeDepth << '\n';
        std::cout << "evolution num: " << settings.evolutionNum << '\n';
        std::cout << "mutation rate: " << settings.mutationRate << '\n';
        std::cout << "crossover rate: " << settings.crossoverRate << '\n';
        std::cout << "random seed: " << settings.randomSeed << '\n';
        std::cout << "teacher data num: " << std::size(problem1.ansArgList) << '\n';
        std::cout << std::flush;

        rnd.seed(settings.randomSeed);
        genetic_operations::DefaultNodeSelector nodeSelector(rnd, settings.maxTreeDepth);
        genetic_operations::DefaultLocalVariableAdapter localVariableAdapter(rnd);
        genetic_operations::DefaultRandomTreeGenerator randomTreeGenerator(rnd, randomNodeGenerator);

        //create initial population
        std::vector<tree::Tree> population;
        population.reserve(settings.populationSize);
        for(int i = 0; i < settings.populationSize; ++i) {
            auto treeProperty = tree::TreeProperty{problem1.returnType, problem1.argumentTypes, generateLocalVariableTypesRandom<>(rnd)};
            auto rootNode = tree_operations::generateTreeRandom(treeProperty, randomNodeGenerator, rnd, settings.maxTreeDepth);
            population.emplace_back(std::move(treeProperty), std::move(rootNode));
        }

        std::cout << "======\n";
        std::cout << "generation, minimum value of inverse fitness\n";
        //execute evolution
        for(std::size_t count = 0; count < settings.evolutionNum; ++count){
//            std::cout << "====" << std::to_string(count) << "th generation" << "====\n";
            std::vector<double> probabilities;
            probabilities.reserve(std::size(population));
            //evaluation of each tree
            for(const auto& tree: population){
                double inverseFitness = 0;

                //evaluation by each data set
                for(std::size_t i = 0; i < std::size(problem1.ansArgList); ++i) {
                    const auto& [answer, arguments] = problem1.ansArgList[i];
                    auto evaluatedValue = tree.evaluate(arguments);
                    inverseFitness += std::abs(evaluator(evaluatedValue, answer));
                }
                probabilities.push_back(1./(1. + inverseFitness));
            }
            assert(std::size(population) == std::size(probabilities));
            auto maxFitnessItr = std::max_element(std::begin(probabilities), std::end(probabilities));
//            std::cout << "minimum value of inverse fitness is " << 1./ *maxFitnessItr - 1. <<'\n';
            std::cout << count << ", " << 1./ *maxFitnessItr - 1. << '\n';

            //create next generation
            std::vector<tree::Tree> nextPopulation;
            nextPopulation.reserve(std::size(population));

            std::discrete_distribution<std::size_t> distribution(std::begin(probabilities), std::end(probabilities));

            std::size_t mutationNum = settings.mutationRate * std::size(population);
            std::size_t crossoverNum = settings.crossoverRate * std::size(population) / 2.;
            assert(mutationNum + 2 * crossoverNum <= std::size(population));
            std::size_t surviveNum = std::size(population) - mutationNum - 2 * crossoverNum;

            while(mutationNum--) {
                nextPopulation.push_back(genetic_operations::mutation(population[distribution(rnd)],
                                                                      randomTreeGenerator,
                                                                      nodeSelector,
                                                                      settings.maxTreeDepth));
            }
            while(crossoverNum--) {
                auto [tree1, tree2] = genetic_operations::crossover(population[distribution(rnd)],
                                                                    population[distribution(rnd)],
                                                                    nodeSelector,
                                                                    localVariableAdapter);
                nextPopulation.push_back(std::move(tree1));
                nextPopulation.push_back(std::move(tree2));
            }
            while(surviveNum--) {
                nextPopulation.push_back(population[distribution(rnd)]);
            }
            assert(std::size(population) == std::size(nextPopulation));

            swap(population, nextPopulation);
        }

        //evaluate last population
        std::vector<std::tuple<double, tree::Tree&>> evaluatedPopulation;
        evaluatedPopulation.reserve(std::size(population));
        //evaluation for each tree
        for(auto& tree: population){
            double inverseFitness = 0;
            //evaluation for each data set
            for(std::size_t i = 0; i < std::size(problem1.ansArgList); ++i) {
                const auto& [answer, arguments] = problem1.ansArgList[i];
                auto evaluatedValue = tree.evaluate(arguments);
                inverseFitness += evaluator(evaluatedValue, answer);
            }
            evaluatedPopulation.push_back(std::tuple<double, tree::Tree&>(inverseFitness, tree));
        }

        std::sort(std::begin(evaluatedPopulation), std::end(evaluatedPopulation), [](const auto& x, const auto& y){return std::get<0>(x) < std::get<0>(y);});

        for(int i = 0; i < std::min(population.size(), MAX_OUTPUT_TREE_NUM); ++i) {
            std::ofstream fout(problem1.name + "_" + std::to_string(i) + "th_tree.xml");
            auto& [fitness, tree] = evaluatedPopulation[i];
            tree.getTreeProperty().name = problem1.name;
            gpManager.writeTree(tree, fout);
            fout.close();
        }
    }

    return 0;
}
