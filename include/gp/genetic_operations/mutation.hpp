#ifndef GP_GENETIC_OPERATIONS_MUTATION
#define GP_GENETIC_OPERATIONS_MUTATION

#include <gp/node/node_interface.hpp>
#include <gp/tree/tree.hpp>
#include <gp/tree_operations/tree_operations.hpp>
#include <gp/utility/is_detected.hpp>
#include <cassert>

namespace gp::genetic_operations {
    template <typename RandomTreeGenerator, //concept: node::NodeInterface::node_instance_type operator()(const tree::TreeProperty&, std::size_t), the first argument is property of tree, second is max tree depth
              typename NodeSelector,        //concept: const node::NodeInterface& operator()(const node::NodeInterface&) , the argument is the root node of tree
              typename = std::enable_if_t<
                      std::conjunction_v<
                              std::is_same<
                                      node::NodeInterface::node_instance_type,
                                      decltype(std::declval<RandomTreeGenerator>()(std::declval<const tree::TreeProperty&>(), std::declval<std::size_t>()))
                              >,
                              std::is_same<
                                      const node::NodeInterface&,
                                      decltype(std::declval<NodeSelector>()(std::declval<const node::NodeInterface&>()))
                              >
                      >
              >
    >
    auto mutation(tree::Tree tree,
                  RandomTreeGenerator& randomTreeGenerator,
                  NodeSelector& nodeSelector,
                  std::size_t maxTreeDepth) -> tree::Tree {
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
