#ifndef GP_TREE_TREE_OPERATIONS_CPP
#define GP_TREE_TREE_OPERATIONS_CPP

#include <tree/tree_operations.hpp>
#include <exception>

namespace gp::tree {
    std::unique_ptr<node::NodeInterface> copyTreeStructure(const node::NodeInterface& rootNode) {
        std::unique_ptr<node::NodeInterface> targetRootNode = rootNode.clone();
        for(int i = 0; i < rootNode.getChildNum(); ++i) {
            if(!rootNode.hasChild(i)) throw std::runtime_error("the copy node must not be null");
            targetRootNode->setChild(i, copyTreeStructure(rootNode.getChildNode(i)));
        }
        return targetRootNode;
    }

    std::size_t getHeight(const node::NodeInterface& node) {
        if(node.getChildNum() == 0) return 0;
        std::size_t ans = 0;
        for(int i = 0; i < node.getChildNum(); ++i) {
            if(!node.hasChild(i)) throw std::runtime_error("the operating tree structure must not have null node");
            ans = std::max(ans, getHeight(node.getChildNode(i)));
        }
        return ans + 1;
    }

    std::size_t getDepth(const node::NodeInterface& node) {
        if(!node.hasParent()) return 0;
        else {
            return 1 + getDepth(node.getParent());
        }
    }
}

#endif