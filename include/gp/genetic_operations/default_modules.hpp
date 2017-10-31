#ifndef GP_GENETIC_OPERATIONS_DEFAULT_MODULES
#define GP_GENETIC_OPERATIONS_DEFAULT_MODULES

#include <gp/node/node_interface.hpp>
#include <gp/tree/tree.hpp>
#include <gp/tree_operations/tree_operations.hpp>
#include <random>
#include <vector>
#include <algorithm>

namespace gp::genetic_operations {
    namespace detail {
        template<typename RandomEngine, typename ...SupportConstNodeTypes>
        class RandomConstValueGenerator {
        public:
            template<typename T>
            using generate_function = std::function<T(RandomEngine &)>;
            using generate_functions = std::tuple<generate_function<SupportConstNodeTypes>...>;
            const generate_functions &generateFunctions;
        private:
            template<std::size_t offset>
            void setConstHelper(node::NodeInterface &constNode, RandomEngine &rnd) const {
                using type = typename std::tuple_element_t<offset, generate_functions>::result_type;
                if (constNode.getReturnType() == utility::typeInfo<type>()) {
                    constNode.setNodePropertyByAny(std::get<offset>(generateFunctions)(rnd));
                } else {
                    if constexpr (offset + 1 < std::tuple_size_v<generate_functions>)
                    {
                        setConstHelper<offset + 1>(constNode, rnd);
                    } else {
                        throw std::runtime_error(
                                "tried to set const value randomly, but proper type of geenrator not found");
                    }
                }
            }

        public:
            void setConstRandom(node::NodeInterface &constNode, RandomEngine &rnd) const {
                if (constNode.getNodeType() != node::NodeType::Const)
                    throw std::runtime_error(
                            "tried to set const value randomly, but the passed node was not const node");
                setConstHelper<0>(constNode, rnd);
            }

        public:
            RandomConstValueGenerator(const generate_functions &generateFunctions_)
                    : generateFunctions(generateFunctions_) {}
        };
    }

    template <typename, typename>
    class DefaultRandomNodeGenerator;

    template<typename RandomEngine, template <typename...> class Tpl, typename ...SupportConstNodeTypes>
    class DefaultRandomNodeGenerator<RandomEngine, Tpl<SupportConstNodeTypes...>> {
    public:
        using node_instance_type = node::NodeInterface::node_instance_type;
        using type_info = node::NodeInterface::type_info;
        using tree_property = tree::TreeProperty;
        using random_const_generator = detail::RandomConstValueGenerator<RandomEngine, SupportConstNodeTypes...>;
        using const_generators = typename random_const_generator::generate_functions;
        using random_engine = RandomEngine;
    private:
        using any_t = node::NodeInterface::any_t;
        using type_index = node::NodeInterface::type_index;
        using container_type = std::unordered_multimap<type_index, node_instance_type, type_index::Hash>;
        using iterator = typename container_type::iterator;
        using const_iterator = typename container_type::const_iterator;
        container_type nodeMultimap;
        container_type leafNodeMultiMap;
        random_const_generator randomConstValueGenerator;
    private:
        static bool isValidNode(const node::NodeInterface &node, const tree::TreeProperty &treeProperty) {
            using std::size;
            switch (node.getNodeType()) {
                case node::NodeType::Argument: {
                    auto argumentIdx = std::any_cast<node::NodeInterface::variable_index_type>(
                            node.getNodePropertyByAny());
                    return (argumentIdx < size(treeProperty.argumentTypes) &&
                            node.getReturnType().removeReferenceType().removeLeftHandValueType() ==
                            treeProperty.argumentTypes[argumentIdx]->removeReferenceType());
                }
                case node::NodeType::LocalVariable: {
                    auto itr = std::find_if(treeProperty.localVariableTypes.begin(),
                                            treeProperty.localVariableTypes.end(),
                                            [&type = node.getReturnType()](auto x){return *x == type.removeLeftHandValueType().removeReferenceType();});
                    return itr != std::end(treeProperty.localVariableTypes);
                }
                default:
                    return true;
            }
        }
        static void setLocalVariableIndexRandom(node::NodeInterface& node, const tree::TreeProperty& treeProperty, RandomEngine& rnd){
            std::size_t size = std::count_if(std::begin(treeProperty.localVariableTypes),
                                             std::end(treeProperty.localVariableTypes),
                                             [&type = node.getReturnType()](auto x){return *x == type.removeReferenceType().removeLeftHandValueType();});
            if (size == 0)throw std::runtime_error("try to set local variable index, but the local variable type does not match tree property");
            std::uniform_int_distribution<node::NodeInterface::variable_index_type> dist(0, size - 1);
            auto cnt = dist(rnd);
            auto c = cnt;
            for(node::NodeInterface::variable_index_type i = 0; i < std::size(treeProperty.localVariableTypes); ++i){
                if(node.getReturnType().removeReferenceType().removeLeftHandValueType() == *treeProperty.localVariableTypes[i]) {
                    if(cnt == 0) {
                        node.setNodePropertyByAny(i);
                        return;
                    }
                    --cnt;
                }
            };
        }
        node_instance_type generateNodeHelper(const type_info &returnType,
                                              const tree::TreeProperty &treeProperty,
                                              const container_type &container,
                                              RandomEngine &rnd) const {
            auto[begin, end] = returnType == utility::typeInfo<any_t>() ? std::make_pair(std::begin(container), std::end(container))
                                                                        : container.equal_range(utility::TypeIndex(returnType));
            auto size = std::distance(begin, end);
            if (size == 0)throw std::runtime_error("tryied to generate spesified type rondomly, but nodes whose retrun type is the specified type node registered");
            std::uniform_int_distribution<int> dist(0, size - 1);
            auto itr = begin;
            do {
                itr = begin;
                std::advance(itr, dist(rnd));
            } while (!isValidNode(*itr->second, treeProperty));
            auto ans = itr->second->clone();
            if (ans->getNodeType() == node::NodeType::Const) {
                randomConstValueGenerator.setConstRandom(*ans, rnd);
            } else if (ans->getNodeType() == node::NodeType::LocalVariable) {
                setLocalVariableIndexRandom(*ans, treeProperty, rnd);
            }
            return ans;
        }

    public:
        node_instance_type generateNode(const type_info &returnType, const tree::TreeProperty &treeProperty, RandomEngine &rnd) const {
            return generateNodeHelper(returnType, treeProperty, nodeMultimap, rnd);
        }

        node_instance_type generateLeafNode(const type_info &returnType, const tree::TreeProperty &treeProperty, RandomEngine &rnd) const {
            return generateNodeHelper(returnType, treeProperty, leafNodeMultiMap, rnd);
        }

    private:
        template <template <typename...> class, typename>
        struct RegisterNodeHelper;
        template <template <typename...> class Node, template <typename...> class Tpl_, typename T, typename... Args>
        struct RegisterNodeHelper<Node, Tpl_<T, Args...>> {
            static void registerNodes(container_type& nodeSet, container_type& leafNodeSet) {
                auto node = node::NodeInterface::createInstance<Node<T>>();
                if(node->getChildNum() == 0) {
                    leafNodeSet.emplace(utility::TypeIndex(node->getReturnType()), node->clone());
                }
                nodeSet.emplace(utility::TypeIndex(node->getReturnType()), std::move(node));

                if constexpr (sizeof...(Args) > 0) {
                    RegisterNodeHelper<Node, Tpl_<Args...>>::registerNodes(nodeSet, leafNodeSet);
                }
            }
        };
    public:
        void registerNode(node_instance_type node) {
            if (node->getChildNum() == 0) {
                leafNodeMultiMap.emplace(utility::TypeIndex(node->getReturnType()), node->clone());
            }
            nodeMultimap.emplace(utility::TypeIndex(node->getReturnType()), std::move(node));
        }
        template <typename Node>
        void registerNode(){registerNode(node::NodeInterface::createInstance<Node>());}
        template <template <typename ...> class Node, typename Tpl_>
        void registerNodes() {
            RegisterNodeHelper<Node, Tpl_>::registerNodes(nodeMultimap, leafNodeMultiMap);
        }
    public:
        DefaultRandomNodeGenerator(const const_generators &generateFunctions)
                : randomConstValueGenerator(generateFunctions) {}
    };
}

namespace gp::traits {
    template <typename RandomEngine, typename ...SupportConstTypes>
    struct random_node_generator_traits<genetic_operations::DefaultRandomNodeGenerator<RandomEngine, SupportConstTypes...>> {
    private:
        using adapt_type = genetic_operations::DefaultRandomNodeGenerator<RandomEngine, SupportConstTypes...>;
    public:
        using node_instance_type = typename adapt_type::node_instance_type;
        using type_info = typename adapt_type::type_info;
        using tree_property = typename adapt_type::tree_property;
        static node_instance_type generate_node(const adapt_type& randomNodeGenerator, const type_info& returnType, const tree_property& treeProperty, RandomEngine& rnd){
            return randomNodeGenerator.generateNode(returnType, treeProperty, rnd);
        }
        static node_instance_type generate_leaf_node(const adapt_type& randomNodeGenerator, const type_info& returnType, const tree_property& treeProperty, RandomEngine& rnd){
            return randomNodeGenerator.generateLeafNode(returnType, treeProperty, rnd);
        }
    };

    template <typename RandomEngine, typename ...SupportTypes>
    struct is_random_node_generator_type<genetic_operations::DefaultRandomNodeGenerator<RandomEngine, SupportTypes...>>: std::true_type{};
}

namespace gp::genetic_operations {
    template <typename RandomEngine, typename RandomNodeGenerator>
    class DefaultRandomTreeGenerator {
    private:
        RandomEngine& rnd;
        RandomNodeGenerator& randomNodeGenerator;
    public:
        DefaultRandomTreeGenerator(RandomEngine& rnd_, RandomNodeGenerator& randomNodeGenerator_): rnd(rnd_), randomNodeGenerator(randomNodeGenerator_){}
    public:
        node::NodeInterface::node_instance_type operator()(const tree::TreeProperty& treeProperty, std::size_t maxTreeDepth){
            return tree_operations::generateTreeRandom(treeProperty, randomNodeGenerator, rnd, maxTreeDepth);
        }
    };

    template <typename RandomEngine>
    class DefaultNodeSelector {
    private:
        RandomEngine& rnd;
        const std::size_t maxTreeDepth;
    public:
        DefaultNodeSelector(RandomEngine& rnd_, std::size_t maxTreeDepth_): rnd(rnd_), maxTreeDepth(maxTreeDepth_){}
    private:
        const node::NodeInterface* selectHelper(const node::NodeInterface& rootNode, std::size_t& num)const {
            if(num == 0) return &rootNode;
            else {
                for(int i = 0; i < rootNode.getChildNum(); ++i) {
                    --num;
                    const node::NodeInterface* ans = selectHelper(rootNode.getChild(i), num);
                    if(ans != nullptr) return ans;
                }
                return nullptr;
            }
        }
    public:
        //select function for mutation
        const node::NodeInterface& operator()(const node::NodeInterface& rootNode) {
            auto nodeNum = tree_operations::getSubtreeNodeNum(rootNode);
            std::uniform_int_distribution<int> dist(0, nodeNum - 1);
            std::size_t num = dist(rnd);
            const node::NodeInterface* ans = selectHelper(rootNode, num);
            assert(ans != nullptr);
            return *ans;
        }
    private:
        struct NodeInfo {
            const node::NodeInterface& node;
            std::size_t height;
            std::size_t depth;
        };
        static std::size_t collectNodeInfoHelper(const node::NodeInterface& node, std::vector<NodeInfo>& nodeInfo, std::size_t currentDepth) {
            if(node.getChildNum() == 0) {
                nodeInfo.push_back({node, 0, currentDepth});
                return 0;
            } else {
                std::size_t height = 0;
                for(int i = 0; i < node.getChildNum(); ++i){
                    height = std::max(height, collectNodeInfoHelper(node.getChild(i), nodeInfo, currentDepth + 1) + 1);
                }
                nodeInfo.push_back({node, height, currentDepth});
                return height;
            }
        }
        static std::vector<NodeInfo> collectNodeInfo(const node::NodeInterface& rootNode) {
            auto size = tree_operations::getSubtreeNodeNum(rootNode);
            std::vector<NodeInfo> nodeInfo;
            nodeInfo.reserve(size);
            collectNodeInfoHelper(rootNode, nodeInfo, 0);
            return nodeInfo;
        }
    public:
        //select function for crossover
        std::pair<const node::NodeInterface&, const node::NodeInterface&> operator()(const node::NodeInterface& rootNode1,
                                                                                     const node::NodeInterface& rootNode2) {
            assert(tree_operations::getDepth(rootNode1) <= maxTreeDepth && tree_operations::getDepth(rootNode2) <= maxTreeDepth);
            auto nodeInfo2 = collectNodeInfo(rootNode2);
            while(true) {
                const auto& node1 = (*this)(rootNode1);
                auto depth1 = tree_operations::getDepth(node1);
                auto height1 = tree_operations::getHeight(node1);
                auto cond = [&type = node1.getReturnType(), depth = depth1, height = height1, maxTreeDepth = maxTreeDepth](auto info){
                    return info.node.getReturnType() == type
                           && depth + info.height <= maxTreeDepth
                           && info.depth + height <= maxTreeDepth;
                };
                auto num = std::count_if(std::begin(nodeInfo2), std::end(nodeInfo2), cond);
                if(num > 0) {
                    std::uniform_int_distribution<int> dist(0, num - 1);
                    auto i = dist(rnd);
                    for(const auto& info: nodeInfo2) {
                        if(cond(info)){
                            --i;
                            if(i < 0) return std::pair<const node::NodeInterface&, const node::NodeInterface&>(node1, info.node);
                        }
                    }
                }
            }
        };
    };

    template <typename RandomEngine>
    class DefaultLocalVariableAdapter {
    private:
        RandomEngine& rnd;
    public:
        DefaultLocalVariableAdapter(RandomEngine& rnd_): rnd(rnd_) {}
    public:
        void operator()(node::NodeInterface& subtreeRoot, tree::TreeProperty& treeProperty) {
            if(subtreeRoot.getNodeType() == node::NodeType::LocalVariable) {
                const auto& type = subtreeRoot.getReturnType().removeLeftHandValueType().removeReferenceType();
                auto size = std::count_if(std::begin(treeProperty.localVariableTypes),
                                          std::end(treeProperty.localVariableTypes),
                                          [&type = type](auto x){return *x == type;});

                if (size == 0){
                    treeProperty.localVariableTypes.push_back(&type);
                    subtreeRoot.setNodePropertyByAny(std::size(treeProperty.localVariableTypes) - 1);
                } else {
                    std::uniform_int_distribution<node::NodeInterface::variable_index_type> dist(0, size - 1);
                    auto n = dist(rnd);
                    for(decltype(n) i = 0; i < std::size(treeProperty.localVariableTypes); ++i){
                        if(*treeProperty.localVariableTypes[i] == type){
                            if(n == 0){
                                subtreeRoot.setNodePropertyByAny(i);
                                break;
                            }
                            --n;
                        }
                    }
                }
            }
            for(int i = 0; i < subtreeRoot.getChildNum(); ++i) {
                (*this)(subtreeRoot.getChild(i), treeProperty);
            }
        }
    };
}

#endif
