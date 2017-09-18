#ifndef GP_TREE_OPERATIONS_DETAIL_GENERATE_TREE_RANDOM
#define GP_TREE_OPERATIONS_DETAIL_GENERATE_TREE_RANDOM

#include <gp/traits/node_traits.hpp>
#include <gp/traits/tree_traits.hpp>
#include <gp/traits/random_node_generator_traits.hpp>

namespace gp::tree_operations::detail {
    template <typename Node, typename TreeProperty, typename RandomNodeGenerator, typename RandomEngine>
    void generateTreeRandom(Node& rootNode, const TreeProperty& treeProperty, const RandomNodeGenerator& randomNodeGenerator, RandomEngine& rnd, std::size_t maxTreeDepth) {
        static_assert(traits::is_typed_node_type_v<Node>);
        using node_trait = traits::typed_node_traits<Node>;
        using tree_property_trait = traits::tree_property_traits<TreeProperty>;
        using node_gen_trait = traits::random_node_generator_traits<RandomNodeGenerator>;
        if (maxTreeDepth < 1) return;
        if (maxTreeDepth == 1) {
            auto childNum = node_trait::get_child_num(rootNode);
            for(int i = 0; i < childNum; ++i){
                const auto& returnType = node_trait::get_child_return_type(rootNode, i);
                node_trait::set_child(rootNode, i, node_gen_trait::generate_leaf_node(randomNodeGenerator, returnType, treeProperty, rnd));
            }
        } else {
            auto childNum = node_trait::get_child_num(rootNode);
            for(int i = 0; i < childNum; ++i){
                const auto& returnType = node_trait::get_child_return_type(rootNode, i);
                node_trait::set_child(rootNode, i, node_gen_trait::generate_node(randomNodeGenerator, returnType, treeProperty, rnd));
                generateTreeRandom(node_trait::get_child(rootNode, i), treeProperty, randomNodeGenerator, rnd, maxTreeDepth - 1);
            }
        }
    };
}

#endif
