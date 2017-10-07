#ifndef GP_PROBLEM_PROBLEM
#define GP_PROBLEM_PROBLEM

#include <any>
#include <gp/utility/type.hpp>
#include <gp/utility/variable.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace gp::problem {
    namespace detail {
        template <std::size_t offset, typename ...Ts>
        utility::Variable stringToVariableHelper(const std::string& str,
                                                const utility::TypeInfo& type,
                                                const std::tuple<std::function<Ts(const std::string&)>...>& stringToValues){
            if(type == utility::typeInfo<
                                typename std::tuple_element_t<offset, std::decay_t<decltype(stringToValues)>>::result_type
                            >()) return utility::Variable(std::get<offset>(stringToValues)(str));
            if constexpr (offset + 1 < std::tuple_size_v<std::decay_t<decltype(stringToValues)>>) {
                return stringToVariableHelper<offset + 1>(str, type, stringToValues);
            } else {
                throw std::runtime_error("unknown string to value conversion is called in reading problem file");
            }
        };

        template <typename ...Ts>
        utility::Variable stringToVariable(const std::string& str,
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
        constexpr const char* TEACHER_DATA_ARGUMENT_INDEX_ARRIBUTE = "idx";
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
    inline Problem load(std::istream& in,
                        const utility::StringToType& stringToType,
                        const std::tuple<std::function<SupportTypes(const std::string&)>...>& stringToValues){
        using namespace boost::property_tree;
        ptree tree;
        xml_parser::read_xml(in, tree);
        Problem problem1;
        //get problem name
        if(auto name = tree.template get_optional<std::string>(std::string(io::ROOT_FIELD) + "." + io::NAME_FIELD)){
            problem1.name = std::move(*name);
        } else {
            throw std::runtime_error("name field not found in the problem file");
        }
        //get return type
        if(auto returnTypeStr = tree.template get_optional<std::string>(std::string(io::ROOT_FIELD) + "." + io::RETURN_TYPE_FIELD)){
            if(!stringToType.hasType(*returnTypeStr)) throw std::runtime_error("unknown return type name found in the problem file");
            problem1.returnType = &stringToType(*returnTypeStr);
        }
        //get argument types
        for(const auto& [key, val]: tree.get_child(std::string(io::ROOT_FIELD) + "." + io::ARGUMENTS_FIELD)){
            if(key == io::VARIABLE_TYPE_FIELD){
                const auto& typeStr = val.data();
                if(!stringToType.hasType(typeStr)) throw std::runtime_error("unknown argument type name found in the problem file");
                problem1.argumentTypes.push_back(&stringToType(typeStr));
            }
        }
        //get teacher data set
        for(const auto& [key, value]: tree.get_child(std::string(io::ROOT_FIELD) + "." + io::TEACHER_DATA_SET_FIELD)) {
            if(key == io::TEACHER_DATA_FIELD) {
                std::vector<utility::Variable> args(std::size(problem1.argumentTypes));
                utility::Variable ans;
                for(const auto& [dataKey, dataValue]: value) {
                    if(dataKey == io::TEACHER_DATA_ARGUMENT_FIELD) {
                        auto idx = std::stoi(dataValue.get<std::string>(std::string("<xmlattr>.") + io::TEACHER_DATA_ARGUMENT_INDEX_ARRIBUTE));
                        args[idx] = detail::stringToVariable(dataValue.data(), *problem1.argumentTypes[idx], stringToValues);
                    } else if (dataKey == io::TEACHER_DATA_ANSWER_FIELD) {
                        ans = detail::stringToVariable(dataValue.data(), *problem1.returnType, stringToValues);
                    }
                }
                problem1.ansArgList.push_back(std::make_tuple(std::move(ans), std::move(args)));
            }
        }

        return problem1;
    }
}

#endif
