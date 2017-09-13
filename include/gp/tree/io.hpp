#ifndef GP_IO_TREE_IO
#define GP_IO_TREE_IO

#include <fstream>
#include <gp/node/node.hpp>
#include <gp/tree/tree.hpp>
#include <gp/node/string_to_node.hpp>
#include <gp/gp_config.hpp>
#include <gp/tree_operations/tree_operations.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace gp::tree {
    class TypesToSubroutineNode{
    private:
        using type_info = node::NodeInterface::type_info;
        using node_instance_type = node::NodeInterface::node_instance_type;
        using key = std::string;
        using subroutine_node_create_function = node_instance_type(*)(const std::string&, const node::SubroutineEntitySet&);
        using containter_type = std::unordered_map<key, subroutine_node_create_function>;
        containter_type Container;
    private:
        static std::string createSubroutineString(const type_info& returnType, const std::vector<const type_info*>& arguments) {
            auto ans = returnType.name() + "(";
            for(int i = 0; i < std::size(arguments); ++i) {
                if(arguments[i] == nullptr) throw std::runtime_error("nullptr found in the argument types");
                ans += arguments[i]->name();
                if(i != std::size(arguments) - 1) ans += ",";
            }
            ans += ")";
            return ans;
        }
        template <typename T, typename ...Args>
        static std::string createSubroutineString() {
            return createSubroutineString(utility::typeInfo<T>(), {&utility::typeInfo<Args>()...});
        }
    public:
        node_instance_type createSubroutineNode(const type_info& returnType,
                                                const std::vector<const type_info*>& arguments,
                                                const std::string& name,
                                                const node::SubroutineEntitySet& subroutineEntitySet)const {

            auto itr = Container.find(createSubroutineString(returnType, arguments));
            if(itr == std::end(Container)) return nullptr;
            return itr->second(name, subroutineEntitySet);
        }
        template <typename T, typename ...Args>
        void registerSubroutineNodeType() {
            Container[createSubroutineString<T, Args...>()] = node::NodeInterface::createInstance<node::SubroutineNode<T(Args...)>, const std::string&, const node::SubroutineEntitySet&>;
        };
    };

    template <typename ...SupportTypes>
    class SubroutineIO {
    public:
        static constexpr const char* ROOT_FIELD = "tree";
        static constexpr const char* NAME_FIELD = "name";
        static constexpr const char* RETURN_TYPE_FIELD = "return_type";
        static constexpr const char* ARGUMENT_TYPE_FIELD = "arguments";
        static constexpr const char* LOCAL_VARIABLE_FIELD = "local_variables";
        static constexpr const char* TREE_ENTITY_FIELD = "tree_entity";
        static constexpr const char* VARIABLE_TYPE_FIELD = "type";
    private:
        using node_instance_type = node::NodeInterface::node_instance_type;
        TypesToSubroutineNode typesToSubroutineNode;
        node::SubroutineEntitySet subroutineEntitySet;
    public:
        void write(const node::NodeInterface& rootNode, const TreeProperty& property, std::ostream& out)const {
            using namespace boost::property_tree;
            ptree tree;

            tree.put(NAME_FIELD, property.name);

            tree.put(RETURN_TYPE_FIELD, property.returnType->name());

            ptree arguments;
            for(const auto& pType: property.argumentTypes){
                if(pType == nullptr) throw std::runtime_error("argument type ptr is null");
                ptree child;
                child.put("", pType->name());
                arguments.push_back(std::make_pair(VARIABLE_TYPE_FIELD, child));
            }
            tree.add_child(ARGUMENT_TYPE_FIELD, arguments);

            ptree localVariables;
            for(const auto& pType: property.localVariableTypes){
                if(pType == nullptr) throw std::runtime_error("local variable type ptr is null");
                boost::property_tree::ptree child;
                child.put("", pType->name());
                localVariables.push_back(std::make_pair(VARIABLE_TYPE_FIELD, child));
            }
            tree.add_child(LOCAL_VARIABLE_FIELD, localVariables);

            std::stringstream sstream;
            sstream << "\n";
            tree_operations::writeTree(rootNode, sstream);
            tree.put(TREE_ENTITY_FIELD, sstream.str());

            ptree root;
            root.add_child(ROOT_FIELD, tree);

            xml_parser::write_xml(out, root, xml_parser::xml_writer_make_settings<std::string>(' ' , 2));
        }
        TreeProperty load(std::istream& in, const utility::StringToType& stringToType, node::StringToNode& stringToNode) {
            using namespace boost::property_tree;
            ptree tree;
            TreeProperty treeProperty;
            xml_parser::read_xml(in, tree);
            //get tree name
            if(auto treeName = tree.get_optional<std::string>(std::string(ROOT_FIELD) + "." + NAME_FIELD)){
                treeProperty.name = *treeName;
            }else throw std::runtime_error("name field not found in reading tree");
            //get return type
            if(auto returnTypeStr = tree.get_optional<std::string>(std::string(ROOT_FIELD) + "." + RETURN_TYPE_FIELD)){
                if(!stringToType.hasType(*returnTypeStr)) throw std::runtime_error("return type name not found in reading tree");
                treeProperty.returnType = &stringToType(*returnTypeStr);
            } else throw std::runtime_error("the root field must be tree");
            //get arguments
            for(const auto& [key, val]: tree.get_child(std::string(ROOT_FIELD) + "." + ARGUMENT_TYPE_FIELD)){
                if(key == VARIABLE_TYPE_FIELD){
                    const auto& typeStr = val.data();
                    if(!stringToType.hasType(typeStr)) throw std::runtime_error("argument type name not found in reading tree");
                    treeProperty.argumentTypes.push_back(&stringToType(typeStr));
                }
            }
            //get local variables
            for(const auto& [key, val]: tree.get_child(std::string(ROOT_FIELD) + "." + LOCAL_VARIABLE_FIELD)){
                if(key == VARIABLE_TYPE_FIELD){
                    const auto& typeStr = val.data();
                    if(!stringToType.hasType(typeStr)) throw std::runtime_error("argument type name not found in reading tree");
                    treeProperty.localVariableTypes.push_back(&stringToType(typeStr));
                }
            }
            //get tree entity
            if(auto treeEntity = tree.get_optional<std::string>(std::string(ROOT_FIELD) + "." + TREE_ENTITY_FIELD)) {
                //check if the same node name exists
                if(stringToNode.hasNode(treeProperty.name)) throw std::runtime_error("read tree but the same node name already exists");
                //regist this node for the case where this subroutine is recursive
                stringToNode.registerNode(typesToSubroutineNode.createSubroutineNode(*treeProperty.returnType, treeProperty.argumentTypes, treeProperty.name, subroutineEntitySet));
                try {
                    std::stringstream sstream(*treeEntity);
                    auto entity = tree_operations::readTree(stringToNode, treeProperty, sstream);
                    //regist subroutine entity
                    subroutineEntitySet.insert(treeProperty.name, std::make_pair(std::move(entity), treeProperty.localVariableTypes));
                } catch (std::exception&) {//the case where we failed to read tree, delete the registed subroutine node
                    stringToNode.deleteNode(treeProperty.name);
                    throw;
                } catch (...) {
                    stringToNode.deleteNode(treeProperty.name);
                    throw std::runtime_error("unexpected exception was thrown");
                }
            }else throw std::runtime_error("tree_entity field not found in reading tree");
            return treeProperty;
        }
    public:
        SubroutineIO(){
            this->registerSubroutineTypes<MAX_SUBROUTINE_ARGUMENT_NUM, SupportTypes...>(this->typesToSubroutineNode);
        }
    private:
        template <typename ...Ts>
        struct type_holder{};

        template <std::size_t n, typename ...Args>
        static void registerSubroutineTypes(TypesToSubroutineNode& typesToSubroutineNode) {
            //add reference types for argument types of subroutine node
            registerTypes<n>(typesToSubroutineNode, type_holder<Args...>{}, type_holder<Args..., utility::Reference<Args>...>{});
            if constexpr (n > 1){
                registerSubroutineTypes<n - 1, Args...>(typesToSubroutineNode);
            }
        }

        template <std::size_t n, typename T, typename ...Args, typename ...Args1>
        //2nd argument(type_holder<T, Args...>): return type of Subroutine node, 3rd argument(type_holder<Args1...>): candidate types for arguments of subroutine node
        static void registerTypes(TypesToSubroutineNode& typesToSubroutineNode, type_holder<T, Args...>, type_holder<Args1...>){
            acc<n, type_holder<T>, type_holder<Args1...>, type_holder<Args1...>>::registerSubroutineType(typesToSubroutineNode);
            if constexpr (sizeof...(Args) > 0) {
                registerTypes<n>(typesToSubroutineNode, type_holder<Args...>{}, type_holder<Args1...>{});
            }
        }

        template <std::size_t n, typename ...> struct acc;
        template <std::size_t n,
                template <typename ...> class Tpl,
                typename T,
                typename ...Args1,
                typename ...Args2,
                typename ...Args>
        struct acc<n, Tpl<Args1...>, Tpl<T, Args2...>, Tpl<Args...>>{
            static void registerSubroutineType(TypesToSubroutineNode& typesToSubroutineNode) {
                if constexpr (n == 0){
                    typesToSubroutineNode.registerSubroutineNodeType<Args1...>();
                } else {
                    acc<n - 1, Tpl<Args1..., T>, Tpl<Args...>, Tpl<Args...>>::registerSubroutineType(typesToSubroutineNode);
                    if constexpr (sizeof...(Args2) > 0) {
                        acc<n, Tpl<Args1...>, Tpl<Args2...>, Tpl<Args...>>::registerSubroutineType(typesToSubroutineNode);
                    }
                }
            }
        };

    };
}

#endif
