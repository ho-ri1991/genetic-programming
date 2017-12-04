#include <iostream>
#include <fstream>
#include <unordered_map>
#include <gp/tree/tree.hpp>
#include <gp/node/node.hpp>
#include <gp/node/array_nodes.hpp>
#include <gp/gp_manager.hpp>
#include <gp/utility/result.hpp>

using namespace gp;

using StringToVariable = std::function<utility::Variable(const std::string&)>;
using AnyToString = std::function<std::string(const std::any&)>;

//settings for genetic programming
using TypeTuple = std::tuple<int, bool, double>;
using LvalueTypeTuple = std::tuple<utility::LeftHandValue<int>, utility::LeftHandValue<bool>, utility::LeftHandValue<double>>;
using RefTypeTuple = std::tuple<utility::Reference<int>, utility::Reference<bool>, utility::Reference<double>>;
using NumericTypeTuple = std::tuple<int, double>;
const std::vector<const utility::TypeInfo*> localVariableTypeArray = {&utility::typeInfo<int>(), &utility::typeInfo<bool>(), &utility::typeInfo<double>()};

constexpr std::size_t MAX_PROGN_SIZE = 5;
constexpr std::size_t MAX_ARGUMENT_NUM = 3;

int main(){
    //string to value functions for load arguments
    std::unordered_map<utility::TypeIndex, StringToVariable, utility::TypeIndex::Hash> stringToVariables = {
            {utility::TypeIndex(utility::typeInfo<int>()), StringToVariable([](const std::string& str){return utility::Variable(boost::lexical_cast<int>(str));})},
            {utility::TypeIndex(utility::typeInfo<bool>()), StringToVariable([](const std::string& str){return utility::Variable(boost::lexical_cast<bool>(str));})},
            {utility::TypeIndex(utility::typeInfo<double>()), StringToVariable([](const std::string& str){return utility::Variable(boost::lexical_cast<double>(str));})}
    };

    //any to string functions for output answer
    std::unordered_map<utility::TypeIndex, AnyToString, utility::TypeIndex::Hash> anyToStrings = {
            {utility::TypeIndex(utility::typeInfo<int>()), AnyToString([](const std::any& val) {return std::to_string(std::any_cast<int>(val));})},
            {utility::TypeIndex(utility::typeInfo<bool>()), AnyToString([](const std::any& val) {return std::to_string(std::any_cast<bool>(val));})},
            {utility::TypeIndex(utility::typeInfo<double>()), AnyToString([](const std::any& val) {return std::to_string(std::any_cast<double>(val));})}
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
    gpManager.registerNodes<node::DivisionNode, TypeTuple>();

    gpManager.registerNode<node::AtNode<node::GpArray<int>>>();
    gpManager.registerNode<node::AtNode<utility::LeftHandValue<node::GpArray<int>>>>();
    gpManager.registerNode<node::AtNode<utility::Reference<node::GpArray<int>>>>();
    gpManager.registerNode<node::SizeOfArray<node::GpArray<int>>>();
    gpManager.registerNode<node::SizeOfArray<utility::LeftHandValue<node::GpArray<int>>>>();

    gpManager.registerNode<node::IncrementNode<int>>();

    try {
        while (true) {
            std::cout << "\'l\': load tree from file\n";
            std::cout << "\'e\': execute loaded tree\n";
            std::cout << "\'q\': exit" << std::endl;
            std::string input;
            std::cin >> input;
            if (input == "q") {
                std::cout << "Bye!" << std::endl;
                break;
            } else if (input == "l") {
                std::cout << "Write file name" << std::endl;
                std::string fileName;
                std::cin >> fileName;
                std::ifstream fin(std::move(fileName));
                if (!fin.is_open()) {
                    std::cout << "Failed to open file, check file name" << std::endl;
                    continue;
                }
                auto subroutineResult = gpManager.loadSubroutine(fin);
                if (!subroutineResult) {
                    std::cout << subroutineResult.errMessage() << std::endl;
                    continue;
                }
                auto & [subroutineNode, treeProperty] = subroutineResult.unwrap();
                std::cout << subroutineNode->getNodeName() << " is loaded" << std::endl;
            } else if (input == "e") {
                std::cout << "Write tree name" << std::endl;
                std::string name;
                std::cin >> name;
                auto treeResult = gpManager.getSubroutineAsTree(std::move(name));
                if (!treeResult) {
                    std::cout << "tree not found, check tree name" << std::endl;
                    continue;
                }
                auto tree = std::move(*treeResult);
                auto anyToStringResult =
                        anyToStrings.find(utility::TypeIndex(tree.getReturnType())) != std::end(anyToStrings)
                        ? utility::result::ok(
                                std::ref(anyToStrings.find(utility::TypeIndex(tree.getReturnType()))->second))
                        : utility::result::err<std::reference_wrapper<AnyToString>>(
                                "failed to load tree, unknown return type: " + tree.getReturnType().name());

                auto stringToValueResult = [&stringToVariables, &prop = tree.getTreeProperty()]() {
                    std::vector<utility::Result<StringToVariable>> ans;
                    ans.reserve(std::size(prop.argumentTypes));
                    std::transform(std::begin(prop.argumentTypes),
                                   std::end(prop.argumentTypes),
                                   std::back_inserter(ans),
                                   [&stringToVariables](const utility::TypeInfo *x) {
                                       if (!x)
                                           return utility::result::err<StringToVariable>(
                                                   "failed to load tree, argument type was not set properly");
                                       auto itr = stringToVariables.find(utility::TypeIndex(*x));
                                       return itr != std::end(stringToVariables)
                                              ? utility::result::ok(itr->second)
                                              : utility::result::err<StringToVariable>(
                                                       "failed to load tree, unknown argument type: " + x->name());
                                   });
                    return utility::result::sequence(std::move(ans));
                }();

                if (!anyToStringResult || !stringToValueResult) {
                    std::cout << utility::result::sequence(anyToStringResult, stringToValueResult).errMessage()
                              << std::endl;
                    continue;
                }

                AnyToString &anyToString = anyToStringResult.unwrap();
                auto &stringToValue = stringToValueResult.unwrap();

                std::cout << "tree name: " << tree.getTreeProperty().name << '\n';
                std::cout << "return type: " << tree.getTreeProperty().returnType->name() << '\n';
                std::cout << "argument types: ";
                for (auto typePtr: tree.getTreeProperty().argumentTypes)
                    std::cout << typePtr->name() << ',';
                std::cout << std::endl;
                {
                    //dump remaining str
                    std::string tmp;
                    std::getline(std::cin, tmp);
                }

                while (true) {
                    std::cout << "input arguments separated by space to evaluate tree or press \'q\' and enter to quit evaluation"
                              << std::endl;
                    std::string in;
                    std::getline(std::cin, in);
                    if (in == "q") {
                        std::cout << "quit evaluation of the tree \"" << tree.getTreeProperty().name << "\""
                                  << std::endl;
                        break;
                    }
                    std::istringstream iss(in);
                    std::vector<std::string> argStrs;
                    argStrs.reserve(tree.getArgumentNum());
                    std::copy(std::istream_iterator<std::string>(iss),
                              std::istream_iterator<std::string>(),
                              std::back_inserter(argStrs));
                    auto argsResult = (std::size(argStrs) == tree.getArgumentNum()
                                       ? utility::result::ok(std::move(argStrs))
                                       : utility::result::err<std::vector<std::string>>(
                                    "the number of argument is invalid"))
                            .flatMap([&stringToValue](auto &&argStrings) {
                                int n = 0;
                                std::vector<utility::Result<utility::Variable>> ans;
                                ans.reserve(std::size(argStrings));
                                std::transform(std::begin(argStrings),
                                               std::end(argStrings),
                                               std::back_inserter(ans),
                                               [&n, &stringToValue](const std::string &str) {
                                                   return utility::result::tryFunction(
                                                           [&n, &stringToValue, &str]() {
                                                               return stringToValue[n++](str);
                                                           },
                                                           "failed to convert string to value");
                                               });
                                return utility::result::sequence(ans);
                            });
                    if (!argsResult) {
                        std::cout << argsResult.errMessage() << std::endl;
                        continue;
                    }
                    auto evaluatedValue = tree.evaluate(std::move(argsResult).unwrap());
                    if (evaluatedValue.getEvaluationStatus() != utility::EvaluationStatus::ValueReturned) {
                        std::cout << "failed to evaluate tree: "
                                  << static_cast<int>(evaluatedValue.getEvaluationStatus())
                                  << std::endl;
                        continue;
                    }
                    std::cout << "the evaluated value is: " << anyToString(evaluatedValue.getReturnValue()) << std::endl;
                }
            } else {
                std::cout << "unkown command" << std::endl;
            }
        }
    } catch (std::exception& ex) {
        std::cout << "unexpected error occured, " << ex.what() <<std::endl;
    } catch (...) {
        std::cout <<  "unexpected error occured" <<std::endl;
    }

    return 0;
}