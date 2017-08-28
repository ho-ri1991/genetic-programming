#ifndef GP_TREE_OPERATIONS_TREE_TRAITS
#define GP_TREE_OPERATIONS_TREE_TRAITS

#include <typeinfo>

namespace gp::tree_operations {
    template <typename tree>
    struct tree_traits {
        using node_type = typename tree::NodeType;
        static node_type& get_root_node(tree& tree_) {return tree_.getRootNode();}
        static const node_type& get_root_node(const tree& tree_) {return tree_.getRootNode();}
    };
}

#endif