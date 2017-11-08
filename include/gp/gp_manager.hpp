#ifndef GP_GP_MANAGER
#define GP_GP_MANAGER

#include "utility/type.hpp"
#include "node/node.hpp"
#include "tree/io.hpp"

namespace gp {
    template <std::size_t, std::size_t, typename>
    class GPManager;

    template <std::size_t MAX_ARGUMENT_NUM, std::size_t MAX_PROGN_SIZE, template <typename ...> class Tpl, typename ...SupportTypes>
    class GPManager<MAX_ARGUMENT_NUM, MAX_PROGN_SIZE, Tpl<SupportTypes...>> {
        static_assert(MAX_ARGUMENT_NUM > 0);
        static_assert(MAX_PROGN_SIZE > 1);
    public:
        static constexpr const char* LVALUE_PREFIX = "lvalue";
        static constexpr const char* REFERENCE_PREFIX = "ref";
    private:
        using TreeIOType = tree::TreeIO<MAX_ARGUMENT_NUM, Tpl<SupportTypes...>>;
        utility::StringToType stringToType;
        TreeIOType treeIO;
    private:
        template <typename ...Ts>
        struct TypeTuple{};

        template <typename T1, typename ...Args1, typename T2, typename ...Args2>
        void registerTypeNamePair(TypeTuple<T2, Args2...>, T1 t, Args1... args) {
            static_assert(sizeof...(Args1) == sizeof...(Args2));
            static_assert(std::is_constructible_v<utility::StringToType::KeyType, T1>);
            stringToType.setTypeNamePair<T2>(t);
            stringToType.setTypeNamePair<utility::LeftHandValue<T2>>(std::string(LVALUE_PREFIX) + "[" + t + "]");
            stringToType.setTypeNamePair<utility::Reference<T2>>(std::string(REFERENCE_PREFIX) + "[" + t + "]");
            if constexpr (sizeof...(Args1) > 0) {
                registerTypeNamePair(TypeTuple<Args2...>{}, args...);
            }
        }

        template <std::size_t n, typename T>
        void registerPrognNodeHelper(){
            treeIO.registerNode(node::NodeInterface::createInstance<node::PrognNode<T, n>>());
            if constexpr (n < MAX_PROGN_SIZE) {
                registerPrognNodeHelper<n + 1, T>();
            }
        };
        template <typename T, typename ...Args>
        void registerPrognNodes(){
            registerPrognNodeHelper<2, T>();
            if constexpr (sizeof...(Args) > 0) {
                registerPrognNodes<Args...>();
            }
        };
        template <std::size_t n, typename T>
        void registerArgumentNodeHelper() {
            treeIO.registerNode(node::NodeInterface::createInstance<node::ArgumentNode<T, n>>());
            treeIO.registerNode(node::NodeInterface::createInstance<node::ArgumentNode<utility::Reference<T>, n>>());
            treeIO.registerNode(node::NodeInterface::createInstance<node::ArgumentNode<utility::LeftHandValue<T>, n>>());
            if constexpr (n + 1 < MAX_ARGUMENT_NUM) {
                registerArgumentNodeHelper<n + 1, T>();
            }
        };
        template <typename T, typename ...Args>
        void registerArgumentNodes(){
            registerArgumentNodeHelper<0, T>();
            if constexpr (sizeof...(Args) > 0) {
                registerArgumentNodes<Args...>();
            }
        };
        template <typename T, typename ...Args>
        void registerLocalVariableNodes(){
            treeIO.registerNode(node::NodeInterface::createInstance<node::LocalVariableNode<T>>());
            treeIO.registerNode(node::NodeInterface::createInstance<node::LocalVariableNode<utility::Reference<T>>>());
            treeIO.registerNode(node::NodeInterface::createInstance<node::LocalVariableNode<utility::LeftHandValue<T>>>());
            if constexpr (sizeof...(Args) > 0) {
                registerLocalVariableNodes<Args...>();
            }
        };

        template <template <typename...> class Node, typename T, typename ...Args>
        void registerNodesHelper() {
            treeIO.registerNode(node::NodeInterface::createInstance<Node<T>>());
            if constexpr (sizeof...(Args) > 0) {
                registerNodesHelper<Node, Args...>();
            }
        };
        template <template <typename...> class, typename>
        struct RegisterNodeHelper;
        template <template <typename...> class Node, template <typename...> class Tpl_, typename T, typename... Args>
        struct RegisterNodeHelper<Node, Tpl_<T, Args...>> {
            static void registerNodes(TreeIOType& treeIOType) {
                treeIOType.registerNode(node::NodeInterface::createInstance<Node<T>>());
                if constexpr (sizeof...(Args) > 0) {
                    RegisterNodeHelper<Node, Tpl_<Args...>>::registerNodes(treeIOType);
                }
            }
        };
    public:
        template <typename ...Args>
        GPManager(Args... args){
            static_assert(sizeof...(Args) == sizeof...(SupportTypes));
            //register type-name pairs
            registerTypeNamePair(TypeTuple<SupportTypes...>{}, args...);
            //register PrognNodes and ArgumentNodes and LocalVariableNodes as default
            registerPrognNodes<SupportTypes...>();
            registerArgumentNodes<SupportTypes...>();
            registerLocalVariableNodes<SupportTypes...>();
        }
        template <template <typename...> class Node, typename Tpl_>
        void registerNodes() {
            RegisterNodeHelper<Node, Tpl_>::registerNodes(treeIO);
        }
        template <typename Node>
        void registerNode(){
            treeIO.registerNode(node::NodeInterface::createInstance<Node>());
        }
    public:
        const utility::StringToType& getStringToType()const {
            return stringToType;
        }
    public:
        void writeTree(const node::NodeInterface& rootNode, const tree::TreeProperty& treeProperty, std::ostream& out)const {
            treeIO.writeTree(rootNode, treeProperty, out);
        }
        void writeTree(const tree::Tree& tree, std::ostream& out)const {
            treeIO.writeTree(tree, out);
        }
        auto loadSubroutine(std::istream& in) {
            return treeIO.loadSubroutine(in, stringToType);
        }
        template <typename String>
        auto getSubroutineAsTree(String&& name)const {return treeIO.getSubroutineAsTree(std::forward<String>(name));}
        template <typename Tree_, typename = std::enable_if_t<std::is_same_v<tree::Tree, std::decay_t<Tree_>>>>
        void registerTreeAsSubroutine(Tree_&& tree){treeIO.registerTreeAsSubroutine(std::forward<Tree_>(tree));}
        utility::Result<tree::Tree> readTree(std::istream& in) {
            return treeIO.readTree(in, stringToType);
        }
    };
}

#endif
