#ifndef GP_TREE_OPERATIONS_NODE_TRAITS
#define GP_TREE_OPERATIONS_NODE_TRAITS

#include <typeinfo>
#include <memory>

namespace gp::tree_operations {
    template <typename node>
    struct node_traits {
        //type when setting children
        using node_instance_type = std::unique_ptr<node>;
        //methods for children
        static std::size_t get_child_num(const node& node_) {return node_.getChildNum();}
        static bool has_child(const node& node_, std::size_t n) {return node_.hasChild(n);}
        static node& get_child(node& node_, std::size_t n) {return node_.getChild(n);}
        static const node& get_child(const node& node_, std::size_t n) {return node_.getChildNode(n);}
        static void set_child(node& node_, std::size_t n, node_instance_type child) {node_.setChild(n, child);}
        //methods for parent
        static bool has_parent(const node& node_) {return node_.hasParent();}
        static node& get_parent(node& node_) {return node_.getParent();}
        static const node& get_parent(const node& node_) {return node_.getParent();}
    };

    template <typename node>
    struct typed_node_traits: private node_traits<node> {
        using node_instance_type = typename node_traits<node>::node_instance_type;
        using node_traits<node>::get_child_num;
        using node_traits<node>::has_child;
        using node_traits<node>::get_child;
        using node_traits<node>::set_child;
        using node_traits<node>::has_parent;
        using node_traits<node>::get_parent;
        //type of type information
        using type = typename node_traits<node>::type;
        //methods for type information
        static type get_return_type(const node& node_) {return node_.getReturnType();}
        static type get_child_return_type(const node& node_, std::size_t n) {return node_.getChildReturnType(n);}
    };

    template <typename node>
    struct output_node_traits: private node_traits<node> {
        using node_traits<node>::get_child_num;
        using node_traits<node>::has_child;
        using node_traits<node>::get_child;
        using node_traits<node>::has_parent;
        using node_traits<node>::get_parent;
        //methods for output
        static std::string get_node_name(const node& node_) {return node_.getNodeName();}
    };

    template <typename node>
    struct input_node_traits: private typed_node_traits<node> {
        using node_instance_type = typename typed_node_traits<node>::node_instance_type;
        using typed_node_traits<node>::get_child_num;
        using typed_node_traits<node>::has_child;
        using typed_node_traits<node>::get_child;
        using typed_node_traits<node>::set_child;
        using typed_node_traits<node>::has_parent;
        using typed_node_traits<node>::get_parent;
        using typed_node_traits<node>::get_return_type;
        using typed_node_traits<node>::get_child_return_type;
        static void set_node_property_by_node_name(node& node_, const std::string& node_name) {node_.setNodePropertyByNodeName(node_name);}
    };
}

#endif