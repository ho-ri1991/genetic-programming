#ifndef GP_TREE_RANDOM_NODE_GENERATOR
#define GP_TREE_RANDOM_NODE_GENERATOR

#include <unordered_map>
#include <tuple>
#include <functional>
#include <gp/node/node_interface.hpp>
#include <random>
#include "tree.hpp"

namespace gp::tree {
    template <typename RandomEngine, typename ...SupportConstNodeTypes>
    class RandomConstValueGenerator{
    public:
        template <typename T>
        using generate_function = std::function<T(RandomEngine&)>;
        using generate_functions = std::tuple<generate_function<SupportConstNodeTypes>...>;
        const generate_functions& generateFunctions;
    private:
        template <std::size_t offset>
        void setConstHelper(node::NodeInterface& constNode, RandomEngine& rnd)const {
            using type = typename std::tuple_element_t<offset, generate_functions>::result_type;
            if(constNode.getReturnType() == utility::typeInfo<type>()){
                constNode.setNodePropertyByAny(std::get<offset>(generateFunctions)(rnd));
            } else {
                if constexpr (offset + 1 < std::tuple_size_v<generate_functions>) {
                    setConstHelper<offset + 1>(constNode, rnd);
                } else {
                    throw std::runtime_error("tried to set const value randomly, but proper type of geenrator not found");
                }
            }
        }
    public:
        void setConstRandom(node::NodeInterface& constNode, RandomEngine& rnd) const {
            if(constNode.getNodeType() != node::NodeType::Const) throw std::runtime_error("tried to set const value randomly, but the passed node was not const node");
            setConstHelper<0>(constNode, rnd);
        }
    public:
        RandomConstValueGenerator(const generate_functions& generateFunctions_)
                : generateFunctions(generateFunctions_){}
    };

    template <typename RandomEngine, typename ...SupportConstNodeTypes>
    class RandomNodeGenerator {
    public:
        using node_instance_type = node::NodeInterface::node_instance_type;
        using type_info = node::NodeInterface::type_info;
        using tree_property = TreeProperty;
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
        static bool isValidNode(const node::NodeInterface& node, const TreeProperty& treeProperty) {
            using std::size;
            switch (node.getNodeType()) {
                case node::NodeType::Argument: {
                    auto argumentIdx = std::any_cast<node::NodeInterface::variable_index_type>(node.getNodePropertyByAny());
                    return (argumentIdx < size(treeProperty.argumentTypes) &&
                            node.getReturnType().removeReferenceType().removeLeftHandValueType() == treeProperty.argumentTypes[argumentIdx]->removeReferenceType());
                }
                case node::NodeType::LocalVariable: {
                    auto localVariableIdx = std::any_cast<node::NodeInterface::variable_index_type>(node.getNodePropertyByAny());
                    return (localVariableIdx < size(treeProperty.localVariableTypes) &&
                            node.getReturnType().removeReferenceType().removeLeftHandValueType() == *treeProperty.localVariableTypes[localVariableIdx]);
                }
                default:
                    return true;
            }
        }
        node_instance_type generateNodeHelper(const type_info& returnType, const TreeProperty& treeProperty, const container_type& container, RandomEngine& rnd)const {
            auto [begin, end] = returnType == utility::typeInfo<any_t>() ? std::make_pair(std::begin(container), std::end(container)) : container.equal_range(utility::TypeIndex(returnType));
            auto size = std::distance(begin, end);
            if(size == 0)throw std::runtime_error("tryied to generate spesified type rondomly, but nodes whose retrun type is the specified type node registered");
            std::uniform_int_distribution<int> dist(0, size - 1);
            auto itr = begin;
            do{
                itr = begin;
                std::advance(itr, dist(rnd));
            } while (!isValidNode(*itr->second, treeProperty));
            auto ans = itr->second->clone();
            if(ans->getNodeType() == node::NodeType::Const){
                randomConstValueGenerator.setConstRandom(*ans, rnd);
            }
            return ans;
        }
    public:
        node_instance_type generateNode(const type_info& returnType, const TreeProperty& treeProperty, RandomEngine& rnd)const {
            return generateNodeHelper(returnType, treeProperty, nodeMultimap, rnd);
        }
        node_instance_type generateLeafNode(const type_info& returnType, const TreeProperty& treeProperty, RandomEngine& rnd)const {
            return generateNodeHelper(returnType, treeProperty, leafNodeMultiMap, rnd);
        }
    public:
        void registerNode(node_instance_type node) {
            if(node->getChildNum() == 0){
                leafNodeMultiMap.emplace(utility::TypeIndex(node->getReturnType()), node->clone());
            }
            nodeMultimap.emplace(utility::TypeIndex(node->getReturnType()), std::move(node));
        }
    public:
        RandomNodeGenerator(const const_generators& generateFunctions)
                :randomConstValueGenerator(generateFunctions){}
    };
}

#endif
