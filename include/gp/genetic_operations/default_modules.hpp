#ifndef GP_GENETIC_OPERATIONS_DEFAULT_MODULES
#define GP_GENETIC_OPERATIONS_DEFAULT_MODULES

#include <gp/node/node_interface.hpp>
#include <gp/tree/tree.hpp>
#include <gp/tree_operations/tree_operations.hpp>
#include <random>
#include <vector>
#include <algorithm>

namespace gp::genetic_operations {
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
                throw std::runtime_error("tried to set const value randomly, but the passed node was not const node");
            setConstHelper<0>(constNode, rnd);
        }

    public:
        RandomConstValueGenerator(const generate_functions &generateFunctions_)
                : generateFunctions(generateFunctions_) {}
    };

    template<typename RandomEngine, typename ...SupportConstNodeTypes>
    class DefaultRandomNodeGenerator {
    public:
        using node_instance_type = node::NodeInterface::node_instance_type;
        using type_info = node::NodeInterface::type_info;
        using tree_property = tree::TreeProperty;
        using random_const_generator = RandomConstValueGenerator<RandomEngine, SupportConstNodeTypes...>;
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
                    auto localVariableIdx = std::any_cast<node::NodeInterface::variable_index_type>(
                            node.getNodePropertyByAny());
                    return (localVariableIdx < size(treeProperty.localVariableTypes) &&
                            node.getReturnType().removeReferenceType().removeLeftHandValueType() ==
                            *treeProperty.localVariableTypes[localVariableIdx]);
                }
                default:
                    return true;
            }
        }

        node_instance_type generateNodeHelper(const type_info &returnType, const tree::TreeProperty &treeProperty,
                                              const container_type &container, RandomEngine &rnd) const {
            auto[begin, end] = returnType == utility::typeInfo<any_t>() ? std::make_pair(std::begin(container),
                                                                                         std::end(container))
                                                                        : container.equal_range(
                            utility::TypeIndex(returnType));
            auto size = std::distance(begin, end);
            if (size == 0)
                throw std::runtime_error(
                        "tryied to generate spesified type rondomly, but nodes whose retrun type is the specified type node registered");
            std::uniform_int_distribution<int> dist(0, size - 1);
            auto itr = begin;
            do {
                itr = begin;
                std::advance(itr, dist(rnd));
            } while (!isValidNode(*itr->second, treeProperty));
            auto ans = itr->second->clone();
            if (ans->getNodeType() == node::NodeType::Const) {
                randomConstValueGenerator.setConstRandom(*ans, rnd);
            }
            return ans;
        }

    public:
        node_instance_type
        generateNode(const type_info &returnType, const tree::TreeProperty &treeProperty, RandomEngine &rnd) const {
            return generateNodeHelper(returnType, treeProperty, nodeMultimap, rnd);
        }

        node_instance_type
        generateLeafNode(const type_info &returnType, const tree::TreeProperty &treeProperty, RandomEngine &rnd) const {
            return generateNodeHelper(returnType, treeProperty, leafNodeMultiMap, rnd);
        }

    public:
        void registerNode(node_instance_type node) {
            if (node->getChildNum() == 0) {
                leafNodeMultiMap.emplace(utility::TypeIndex(node->getReturnType()), node->clone());
            }
            nodeMultimap.emplace(utility::TypeIndex(node->getReturnType()), std::move(node));
        }

    public:
        DefaultRandomNodeGenerator(const const_generators &generateFunctions)
                : randomConstValueGenerator(generateFunctions) {}
    };
}

namespace gp::traits {
    template <typename RandomEngine, typename ...SupportConstTypes>
    struct traits::random_node_generator_traits<genetic_operations::DefaultRandomNodeGenerator<RandomEngine, SupportConstTypes...>> {
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
                nodeInfo.emplace_back(node, 0, currentDepth);
                return 0;
            } else {
                std::size_t height = 0;
                for(int i = 0; i < node.getChildNum(); ++i){
                    height = std::max(height, collectNodeInfoHelper(node.getChild(i), nodeInfo, currentDepth + 1) + 1);
                }
                nodeInfo.emplace_back(node, height, currentDepth);
                return height;
            }
        }
        static std::vector<NodeInfo> collectNodeInfo(const node::NodeInterface& rootNode) {
            auto size = tree_operations::getSubtreeNodeNum(rootNode);
            std::vector<NodeInfo> nodeInfo;
            nodeInfo.reserve(size);
            collectNodeInfoHelper(rootNode, nodeInfo);
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
                auto maxSwapDepth = maxTreeDepth - depth1;
                auto num = std::count_if(std::begin(nodeInfo2),
                                         std::end(nodeInfo2),
                                         [&type = node1.getReturnType(), maxDepth = maxSwapDepth](auto info){return info.node.getReturnType() == type && info.depth <= maxDepth;});
                if(num > 0) {
                    std::uniform_int_distribution<int> dist(0, num - 1);
                    auto i = dist(rnd);
                    for(const auto& info: nodeInfo2) {
                        if(info.node.getReturnType() == node1.getReturnType() && info.depth <= maxSwapDepth) {
                            --i;
                            if(i < 0) return std::make_pair(node1, info.node);
                        }
                    }
                }
            }
        };
    };

    template <typename RandomEngine, std::size_t MAX_LOCAL_VARIABLE_NUM>
    class DefaultLocalVariableAdapter {
    private:
        RandomEngine rnd;
    public:
        DefaultLocalVariableAdapter(RandomEngine& rnd_): rnd(rnd_) {}
    };
}

#endif
