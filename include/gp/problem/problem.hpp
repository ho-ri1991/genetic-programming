#ifndef GP_PROBLEM_PROBLEM
#define GP_PROBLEM_PROBLEM

#include <any>
#include <algorithm>
#include <gp/utility/type.hpp>
#include <gp/utility/variable.hpp>
#include <gp/utility/result.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <limits>

namespace gp::problem {
    namespace detail {
        template <std::size_t offset, typename ...Ts>
        utility::Result<utility::Variable> stringToVariableHelper(const std::string& str,
                                                                  const utility::TypeInfo& type,
                                                                  const std::tuple<std::function<Ts(const std::string&)>...>& stringToValues){
            if(type == utility::typeInfo<
                                typename std::tuple_element_t<offset, std::decay_t<decltype(stringToValues)>>::result_type
                            >()) return utility::result::ok(utility::Variable(std::get<offset>(stringToValues)(str)));
            if constexpr (offset + 1 < std::tuple_size_v<std::decay_t<decltype(stringToValues)>>) {
                return stringToVariableHelper<offset + 1>(str, type, stringToValues);
            } else {
                return utility::result::err<utility::Variable>("failed to load problem, string to value conversion function of " + type.name() + "not registerd");
            }
        };

        template <typename ...Ts>
        utility::Result<utility::Variable> stringToVariable(const std::string& str,
                                                            const utility::TypeInfo& type,
                                                            const std::tuple<std::function<Ts(const std::string&)>...>& stringToValues) {
            return stringToVariableHelper<0>(str, type, stringToValues);
        }
    }

    namespace io {
        constexpr const char* ROOT_FIELD = "problem";
        constexpr const char* NAME_FIELD = "name";
        constexpr const char* RETURN_TYPE_FIELD = "return_type";
        constexpr const char* ARGUMENTS_FIELD = "arguments";
        constexpr const char* VARIABLE_TYPE_FIELD = "type";
        constexpr const char* TEACHER_DATA_SET_FIELD = "teacher_data_set";
        constexpr const char* TEACHER_DATA_FIELD = "data";
        constexpr const char* TEACHER_DATA_ARGUMENT_FIELD = "argument";
        constexpr const char* TEACHER_DATA_ARGUMENT_INDEX_ATTRIBUTE = "idx";
        constexpr const char* TEACHER_DATA_ANSWER_FIELD = "answer";
    }

    struct Problem {
        using AnsArgPair = std::tuple<utility::Variable, std::vector<utility::Variable>>; //first: ans, second: args
        std::string name;
        const utility::TypeInfo* returnType;
        std::vector<const utility::TypeInfo*> argumentTypes;
        std::vector<AnsArgPair> ansArgList;
    };

    template <typename ...SupportTypes>
    utility::Result<Problem> load(std::istream& in,
                        const utility::StringToType& stringToType,
                        const std::tuple<std::function<SupportTypes(const std::string&)>...>& stringToValues){
        using namespace boost::property_tree;
        using namespace utility;
        ptree tree;
        try {
            xml_parser::read_xml(in, tree);
        } catch (const std::exception& ex) {
            return result::err<Problem>(std::string("failed to load problem\n") + ex.what());
        }
        Problem problem1;
        //get problem name
        auto nameResult = result::fromOptional(tree.template get_optional<std::string>(std::string(io::ROOT_FIELD) + "." + io::NAME_FIELD),
                                               "failed to load problem, name field not found.");
        if(!nameResult) return result::err<Problem>(std::move(nameResult).errMessage());
        problem1.name = std::move(nameResult).unwrap();

        //get return type
        auto returnTypeResult = result::fromOptional(tree.template get_optional<std::string>(std::string(io::ROOT_FIELD) + "." + io::RETURN_TYPE_FIELD),
                                                     "failed to load problem, return_type field not found.")
                .flatMap([&stringToType](const std::string& typeName){
                    using ResultType = const TypeInfo*;
                    if(!stringToType.hasType(typeName)) return result::err<ResultType>("failed to load problem, unknown type name \"" + typeName + "\"");
                    else return result::ok(&stringToType(typeName));
                });

        if(!returnTypeResult) return result::err<Problem>(std::move(returnTypeResult).errMessage());
        problem1.returnType = std::move(returnTypeResult).unwrap();

        //get argument types
        auto argsResult = result::fromOptional(tree.get_child_optional(std::string(io::ROOT_FIELD) + "." + io::ARGUMENTS_FIELD),
                                               "failed to load problem, arguments field not found")
                .flatMap([&stringToType](ptree& child){
                    using ResultType = decltype(Problem{}.argumentTypes);
                    ResultType argumentTypes;
                    for(const auto& [key, val]: child) {
                        if(key != io::VARIABLE_TYPE_FIELD) continue;
                        const auto& typeStr = val.data();
                        if(!stringToType.hasType(typeStr)) return result::err<ResultType>(std::string("failed to load problem, unknown argument type name \"") + typeStr + "\"");
                        argumentTypes.push_back(&stringToType(typeStr));
                    }
                    return result::ok(std::move(argumentTypes));
                });

        if(!argsResult) return result::err<Problem>(std::move(argsResult).errMessage());
        problem1.argumentTypes = std::move(argsResult).unwrap();

        //get teacher data set
        auto teacherDataResult = result::fromOptional(tree.get_child_optional(std::string(io::ROOT_FIELD) + "." + io::TEACHER_DATA_SET_FIELD),
                                                      "failed to load problem, teacher_data_set field not found")
                .flatMap([&problem1, &stringToValues, argNum = std::size(problem1.argumentTypes)](ptree& child){
                    using ResultType = decltype(Problem{}.ansArgList);
                    ResultType ansArgList;
                    for(const auto& [key, value]: child) {
                        if(key != io::TEACHER_DATA_FIELD)continue;

                        auto dataResult = result::fromOptional(value.template get_optional<std::string>(io::TEACHER_DATA_ANSWER_FIELD),
                                                               "failed to load problem, answer field not found in the data field.")
                                .flatMap([&stringToValues, &problem1](auto&& answerStr){
                                    return detail::stringToVariable(answerStr, *problem1.returnType, stringToValues);
                                });

                        if(!dataResult) return result::err<ResultType>(std::move(dataResult).errMessage());
                        auto ans = std::move(dataResult).unwrap();

                        std::vector<Variable> args(argNum);
                        for(const auto& [dataKey, dataValue]: value) {
                            if(dataKey != io::TEACHER_DATA_ARGUMENT_FIELD) continue;

                            auto idxResult = result::fromOptional(dataValue.template get_optional<std::string>(std::string("<xmlattr>.") + io::TEACHER_DATA_ARGUMENT_INDEX_ATTRIBUTE),
                                                                  "failed to load problem, idx attribute not found int the argument field")
                                    .flatMap([](auto&& idxStr){
                                        if(idxStr.empty()
                                           || !std::all_of(std::begin(idxStr), std::end(idxStr), [](auto c){return '0' <= c && c <= '9';})
                                           || (1 < std::size(idxStr)  && idxStr[0] == '0')
                                           || std::numeric_limits<int>::digits10 < std::size(idxStr)) return result::err<int>("failed to load problem, invalid idx \"" + idxStr + "\"");

                                        auto idx = std::stoll(idxStr);
                                        if (static_cast<long long>(INT_MAX) < idx) return result::err<int>("failed to load problem, invalid idx \"" + idxStr + "\"");
                                        return result::ok(static_cast<int>(idx));
                                    }).flatMap([argNum = std::size(problem1.argumentTypes)](int idx){
                                        if(idx < 0 || argNum <= idx) return result::err<int>("failed to load problem, invalid idx \"" + std::to_string(idx) + "\"");
                                        else return result::ok(idx);
                                    });
                            if(!idxResult) return result::err<ResultType>(std::move(idxResult).errMessage());
                            auto idx = idxResult.unwrap();

                            auto argResult = detail::stringToVariable(dataValue.data(), *problem1.argumentTypes[idx], stringToValues);
                            if(!argResult) return result::err<ResultType>(std::move(argResult).errMessage());
                            args[idx] = std::move(argResult).unwrap();
                        }
                        if(!std::all_of(std::begin(args), std::end(args), [](auto x)->bool{return static_cast<bool>(x);})) return result::err<ResultType>("failed to load problem, some argument is lacking");
                        ansArgList.push_back(std::make_tuple(std::move(ans), std::move(args)));
                    }
                    return result::ok(std::move(ansArgList));
                });

        if(!teacherDataResult) return result::err<Problem>(std::move(teacherDataResult).errMessage());
        problem1.ansArgList = std::move(teacherDataResult).unwrap();

        return result::ok(std::move(problem1));
    }
}

#endif
