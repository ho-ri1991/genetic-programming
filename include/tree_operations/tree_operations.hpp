#ifndef GP_TREE_TREE_OPERATIONS_TREE_OPERATIONS
#define GP_TREE_TREE_OPERATIONS_TREE_OPERATIONS

#include "node_traits.hpp"
#include "tree_traits.hpp"

namespace gp::tree_operations {
    template <typename node>
    std::size_t getDepth(const node& node_) {
        using traits = node_traits<node>;
        if(!traits::has_parent(node_)) return 0;
        else return 1 + getDepth(traits::get_parent(node_));
    }

    template <typename node>
    std::size_t getHeight(const node& node_) {
        using traits = node_traits<node>;
        if(traits::get_child_num(node_) == 0) return 0;
        std::size_t ans = 0;
        for(int i = 0; i < traits::get_child_num(node_); ++i) {
            if(!traits::has_child(node_, i)) throw std::runtime_error("the operating tree structure must not have null node");
            ans = std::max(ans, getHeight(traits::get_child(node_, i)));
        }
        return ans + 1;
    }
}

#endif