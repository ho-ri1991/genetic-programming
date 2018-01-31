#ifndef GP_GENETIC_OPERATIONS_MUTATION
#define GP_GENETIC_OPERATIONS_MUTATION

#include <gp/node/node_interface.hpp>
#include <gp/tree/tree.hpp>
#include <gp/tree_operations/tree_operations.hpp>
#include <gp/utility/is_detected.hpp>
#include <cassert>

namespace gp::genetic_operations {
    namespace detail {
        template <typename RandomTreeGenerator>
        using is_match_random_tree_generator_concept =
            std::is_invocable_r<
                    node::NodeInterface::node_instance_type,
                    RandomTreeGenerator,
                    const tree::TreeProperty&,
                    std::size_t
            >;

        template <typename RandomTreeGenerator>
        static constexpr bool is_match_random_tree_generator_concept_v = is_match_random_tree_generator_concept<RandomTreeGenerator>::value;

        template <typename NodeSelector>
        using is_match_node_selector_concept =
            std::is_invocable_r<
                    const node::NodeInterface&,
                    NodeSelector,
                    const node::NodeInterface&
            >;

        template <typename NodeSelector>
        static constexpr bool is_match_node_selector_concept_v = is_match_node_selector_concept<NodeSelector>::value;
    }

    template <typename RandomTreeGenerator, //concept: node::NodeInterface::node_instance_type operator()(const tree::TreeProperty&, std::size_t), the first argument is property of tree, second is max tree depth
              typename NodeSelector         //concept: const node::NodeInterface& operator()(const node::NodeInterface&) , the argument is the root node of tree
    >
    auto mutation(tree::Tree tree,
                  RandomTreeGenerator& randomTreeGenerator,
                  NodeSelector& nodeSelector,
                  std::size_t maxTreeDepth)
    -> std::enable_if_t<
            std::conjunction_v<
                    detail::is_match_random_tree_generator_concept<RandomTreeGenerator>,
                    detail::is_match_node_selector_concept<NodeSelector>
            >,
            tree::Tree
    >
    {
        const auto& selectedNode = nodeSelector(tree.getRootNode());
        auto depth = tree_operations::getDepth(selectedNode);
        assert(depth <= maxTreeDepth);
        auto subtreeProperty = tree.getTreeProperty();
        subtreeProperty.returnType = &selectedNode.getReturnType();
        auto newSubtree = randomTreeGenerator(subtreeProperty, maxTreeDepth - depth);
        if(selectedNode.hasParent()) {
            auto& parent = const_cast<node::NodeInterface&>(selectedNode.getParent());
            for(int i = 0; i < parent.getChildNum(); ++i) {
                if(parent.hasChild(i) && &parent.getChild(i) == &selectedNode) {
                    parent.setChild(i, std::move(newSubtree));
                    return tree;
                }
            }
            return tree;
        } else {
            return tree::Tree(std::move(tree).getTreeProperty(), std::move(newSubtree));
        }
    };
}

#endif
