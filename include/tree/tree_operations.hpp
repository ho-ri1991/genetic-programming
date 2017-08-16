#ifndef GP_TREE_TREE_OPERATIONS
#define GP_TREE_TREE_OPERATIONS

#include <node/node_interface.hpp>

namespace gp::tree {
    std::size_t getDepth(const node::NodeInterface&);
    std::size_t getHeight(const node::NodeInterface&);
    std::shared_ptr<node::NodeInterface> copyTreeStructure(const node::NodeInterface&);
}

#endif