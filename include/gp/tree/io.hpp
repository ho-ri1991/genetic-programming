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
#include <tuple>

namespace gp::tree {
    namespace io {
        constexpr const char* ROOT_FIELD = "tree";
        constexpr const char* NAME_FIELD = "name";
        constexpr const char* RETURN_TYPE_FIELD = "return_type";
        constexpr const char* ARGUMENT_TYPE_FIELD = "arguments";
        constexpr const char* LOCAL_VARIABLE_FIELD = "local_variables";
        constexpr const char* TREE_ENTITY_FIELD = "tree_entity";
        constexpr const char* VARIABLE_TYPE_FIELD = "type";
    }
    namespace detail{
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

        template <typename ptree>
        utility::Result<TreeProperty> getTreeProperty(const ptree& tree, const utility::StringToType& stringToType) {
            using namespace gp::tree::io;
            using namespace boost::property_tree;
            TreeProperty treeProperty;
            //get tree name
            if(auto treeName = tree.template get_optional<std::string>(std::string(ROOT_FIELD) + "." + NAME_FIELD)){
                treeProperty.name = *treeName;
            }else return utility::result::err<TreeProperty>("failed to load tree, name field not found");
            //get return type
            if(auto returnTypeStr = tree.template get_optional<std::string>(std::string(ROOT_FIELD) + "." + RETURN_TYPE_FIELD)){
                if(!stringToType.hasType(*returnTypeStr)) return utility::result::err<TreeProperty>("failed to load tree, unknown return type: " + *returnTypeStr);
                treeProperty.returnType = &stringToType(*returnTypeStr);
            } else return utility::result::err<TreeProperty>("failed to load tree, return_type field not found.");
            //get arguments
            for(const auto& [key, val]: tree.get_child(std::string(ROOT_FIELD) + "." + ARGUMENT_TYPE_FIELD)){
                if(key == VARIABLE_TYPE_FIELD){
                    const auto& typeStr = val.data();
                    if(!stringToType.hasType(typeStr)) return utility::result::err<TreeProperty>("failed to load tree, unknown argument type: " + typeStr);
                    treeProperty.argumentTypes.push_back(&stringToType(typeStr));
                }
            }
            //get local variables
            for(const auto& [key, val]: tree.get_child(std::string(ROOT_FIELD) + "." + LOCAL_VARIABLE_FIELD)){
                if(key == VARIABLE_TYPE_FIELD){
                    const auto& typeStr = val.data();
                    if(!stringToType.hasType(typeStr)) return utility::result::err<TreeProperty>("failed to load tree, unknown local variable type: " + typeStr);
                    treeProperty.localVariableTypes.push_back(&stringToType(typeStr));
                }
            }
            return utility::result::ok(std::move(treeProperty));
        }


        template <std::size_t MAX_SUBROUTINE_ARGUMENT_NUM, typename ...SupportTypes>
        class SubroutineIO {
        private:
            using node_instance_type = node::NodeInterface::node_instance_type;
            detail::TypesToSubroutineNode typesToSubroutineNode;
            node::SubroutineEntitySet subroutineEntitySet;
        public:
            using SubroutineEntity = node::SubroutineEntitySet::SubroutineEntity;
            template <typename String>
            std::optional<std::reference_wrapper<SubroutineEntity>> getSubroutineEntity(String&& name) {
                using WrapType = std::reference_wrapper<SubroutineEntity>;
                auto itr = subroutineEntitySet.find(std::forward<String>(name));
                return itr != subroutineEntitySet.end() ? std::optional<WrapType>(std::ref(itr->second)) : std::optional<WrapType>{};
            }
            template <typename String>
            std::optional<std::reference_wrapper<const SubroutineEntity>> getSubroutineEntity(String&& name)const {
                using WrapType = std::reference_wrapper<const SubroutineEntity>;
                auto itr = subroutineEntitySet.find(std::forward<String>(name));
                return itr != subroutineEntitySet.end() ? std::optional<WrapType>(std::cref(itr->second)) : std::optional<WrapType>{};
            }
        public:
            void write(const node::NodeInterface& rootNode, const TreeProperty& property, std::ostream& out)const {
                using namespace boost::property_tree;
                using namespace gp::tree::io;
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
            utility::Result<std::tuple<node_instance_type, TreeProperty>> load(std::istream& in, const utility::StringToType& stringToType, node::StringToNode& stringToNode) {
                using AnsType = std::tuple<node_instance_type, TreeProperty>;
                using namespace boost::property_tree;
                using namespace gp::tree::io;
                ptree tree;
                try {
                    xml_parser::read_xml(in, tree);
                } catch (boost::property_tree::xml_parser_error& ex){
                    return utility::result::err<AnsType>(std::string("failed to load tree, some error found in the xml file\n") + "line " + std::to_string(ex.line()) + ": " + ex.message());
                }
                auto treePropertyResult = detail::getTreeProperty(tree, stringToType);
                if(!treePropertyResult) return utility::result::err<AnsType>(std::move(treePropertyResult).errMessage());
                auto& treeProperty = treePropertyResult.unwrap();
                //get tree entity
                if(auto treeEntity = tree.get_optional<std::string>(std::string(ROOT_FIELD) + "." + TREE_ENTITY_FIELD)) {
                    //check if the same node name exists
                    if(stringToNode.hasNode(treeProperty.name)) return utility::result::err<AnsType>("failed to load tree, the same node name has already loaded.");
                    //regist this node for the case where this subroutine is recursive
                    stringToNode.registerNode(typesToSubroutineNode.createSubroutineNode(*treeProperty.returnType, treeProperty.argumentTypes, treeProperty.name, subroutineEntitySet));
                    try {
                        std::stringstream sstream(*treeEntity);
                        auto entityResult = tree_operations::readTree(stringToNode, treeProperty, sstream);
                        if(!entityResult){
                            stringToNode.deleteNode(treeProperty.name);
                            return utility::result::err<AnsType>(std::move(entityResult).errMessage());
                        }
                        auto entity = std::move(entityResult).unwrap();
                        //regist subroutine entity
                        subroutineEntitySet.insert(treeProperty.name, std::make_pair(std::move(entity), treeProperty.localVariableTypes));
                    } catch (...) {//the case where we failed to read tree, delete the registered subroutine node
                        stringToNode.deleteNode(treeProperty.name);
                        throw;
                    }
                }else return utility::result::err<AnsType>("failed to load tree, tree_entity field not found");
                return utility::result::ok(std::make_tuple(stringToNode(treeProperty.name), treeProperty));
            }
            template <typename Tree_, typename = std::enable_if_t<std::is_same_v<Tree, std::decay_t<Tree_>>>>
            void registerTreeAsSubroutine(Tree_&& tree, node::StringToNode& stringToNode) {
                if(stringToNode.hasNode(tree.getName())) throw std::runtime_error("the same name subroutine already exists");
                Tree tmpTree = std::forward<Tree_>(tree);
                std::string name = tmpTree.getName();
                try {
                    auto rootNode = std::move(tmpTree).getRootNodeInstance();
                    TreeProperty&& treeProperty = std::move(tmpTree).getTreeProperty();
                    subroutineEntitySet.insert(tmpTree.getName(), std::make_pair(std::move(rootNode), std::move(treeProperty.localVariableTypes)));
                    stringToNode.registerNode(typesToSubroutineNode.createSubroutineNode(*treeProperty.returnType, std::move(treeProperty.argumentTypes), std::move(treeProperty.name), subroutineEntitySet));
                } catch (...){
                    stringToNode.deleteNode(name);
                    subroutineEntitySet.deleteEntity(name);
                    throw;
                }
            }
        public:
            SubroutineIO(){
                this->registerSubroutineTypes<MAX_SUBROUTINE_ARGUMENT_NUM, SupportTypes...>(this->typesToSubroutineNode);
            }
        private:
            template <typename ...Ts>
            struct type_holder{};

            template <std::size_t n, typename ...Args>
            static void registerSubroutineTypes(detail::TypesToSubroutineNode& typesToSubroutineNode) {
                //add reference types for argument types of subroutine node
                registerTypes<n>(typesToSubroutineNode, type_holder<Args...>{}, type_holder<Args..., utility::Reference<Args>...>{});
                if constexpr (n > 1){
                    registerSubroutineTypes<n - 1, Args...>(typesToSubroutineNode);
                }
            }

            template <std::size_t n, typename T, typename ...Args, typename ...Args1>
            //2nd argument(type_holder<T, Args...>): return type of Subroutine node, 3rd argument(type_holder<Args1...>): candidate types for arguments of subroutine node
            static void registerTypes(detail::TypesToSubroutineNode& typesToSubroutineNode, type_holder<T, Args...>, type_holder<Args1...>){
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
                static void registerSubroutineType(detail::TypesToSubroutineNode& typesToSubroutineNode) {
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

    template <std::size_t, typename>
    class TreeIO;

    template <std::size_t MAX_SUBROUTINE_ARGUMENT_NUM, template <typename ...> class Tpl, typename ...SupportTypes>
    class TreeIO<MAX_SUBROUTINE_ARGUMENT_NUM, Tpl<SupportTypes...>> {
    private:
        node::StringToNode stringToNode;
        detail::SubroutineIO<MAX_SUBROUTINE_ARGUMENT_NUM, SupportTypes...> subroutineIO;
        using node_instance_type = node::NodeInterface::node_instance_type;
    public:
        //wrapper methods of SubroutineIO
        auto loadSubroutine(std::istream& in, const utility::StringToType& stringToType){return subroutineIO.load(in, stringToType, stringToNode);}
        template <typename Tree_, typename = std::enable_if_t<std::is_same_v<Tree, std::decay_t<Tree_>>>>
        void registerTreeAsSubroutine(Tree_&& tree){subroutineIO.registerTreeAsSubroutine(std::forward<Tree_>(tree), stringToNode);};
        void writeTree(const node::NodeInterface& rootNode, const TreeProperty& property, std::ostream& out)const {subroutineIO.write(rootNode, property, out);}
        void writeTree(const Tree& tree, std::ostream& out)const {subroutineIO.write(tree.getRootNode(), tree.getTreeProperty(), out);}
    public:
        //this method does not support recursion tree becase this method just read and construct tree, therefor, the tree is "inline" funciton
        utility::Result<Tree> readTree(std::istream& in, const utility::StringToType& stringToType)const {
            using namespace boost::property_tree;
            using namespace gp::tree::io;
            ptree tree;
            try {
                xml_parser::read_xml(in, tree);
            } catch (boost::property_tree::xml_parser_error& ex){
                return utility::result::err<Tree>(std::string("failed to load tree, some error found in the xml file\n") + "line " + std::to_string(ex.line()) + ": " + ex.message());
            }
            //get treeProperty
            auto treePropertyResult = detail::getTreeProperty(tree, stringToType);
            if(!treePropertyResult) return utility::result::err<Tree>(std::move(treePropertyResult).errMessage());
            auto treeProperty = std::move(treePropertyResult).unwrap();
            node_instance_type rootNode;
            //get tree entity
            if(auto treeEntity = tree.get_optional<std::string>(std::string(ROOT_FIELD) + "." + TREE_ENTITY_FIELD)) {
                std::stringstream sstream(*treeEntity);
                auto rootNodeResult = tree_operations::readTree(stringToNode, treeProperty, sstream);
                if(!rootNodeResult) return utility::result::err<Tree>(std::move(rootNodeResult).errMessage());
                rootNode = std::move(rootNodeResult).unwrap();
            }else return utility::result::err<Tree>("failed to load tree, tree_entity field not found.");
            return utility::result::ok(Tree(std::move(treeProperty), std::move(rootNode)));
        }
        utility::Result<Tree> readAndRegisterTreeAsSubroutine(std::istream& in, const utility::StringToType& stringToType) {
            auto loadResult = loadSubroutine(in, stringToType);
            if(!loadResult) return utility::result::err<Tree>(std::move(loadResult).errMessage());
            auto [subroutineNode, treeProperty] = std::move(loadResult).unwrap();
            auto subroutineEntityOptional = subroutineIO.getSubroutineEntity(subroutineNode->getNodeName());
            if(!subroutineEntityOptional) return utility::result::err<Tree>("failed to load tree, loading subtree is suceeded but regitering it to SubroutineEntitySet seems to failed");
            node::SubroutineEntitySet::SubroutineEntity& subroutineEntity = *subroutineEntityOptional;
            return utility::result::ok(Tree(std::move(treeProperty), copyTreeStructure(*std::get<0>(subroutineEntity))));
        }
        template <typename String>
        node_instance_type getNodeByNodeName(String&& name)const{return stringToNode(std::forward<String>(name));}
        template <typename String>
        std::optional<Tree> getSubroutineAsTree(String&& name)const {
            auto subroutineEntityOptional = subroutineIO.getSubroutineEntity(name);
            if(!subroutineEntityOptional) return std::optional<Tree>{};
            auto subroutineNode = getNodeByNodeName(name);
            if(!subroutineNode) return std::optional<Tree>{};
            const node::SubroutineEntitySet::SubroutineEntity& subroutineEntity = *subroutineEntityOptional;
            const auto& [entity, localVar] = subroutineEntity;
            TreeProperty treeProperty;
            treeProperty.name = std::forward<String>(name);
            treeProperty.localVariableTypes = localVar;
            treeProperty.argumentTypes.reserve(subroutineNode->getChildNum());
            for(int i = 0; i < subroutineNode->getChildNum(); ++i)
                treeProperty.argumentTypes.push_back(&subroutineNode->getChildReturnType(i));

            return std::optional<Tree>(Tree(std::move(treeProperty), copyTreeStructure(*std::get<0>(subroutineEntity))));
        }
    public:
        void registerNode(node_instance_type node) {return stringToNode.registerNode(std::move(node));}
    };
}

#endif
