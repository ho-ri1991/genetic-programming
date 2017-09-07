#ifndef GP_TREE_OPERATIONS_STRING_TO_NODE_TRAITS
#define GP_TREE_OPERATIONS_STRING_TO_NODE_TRAITS

#include <gp/node/string_to_node.hpp>


namespace gp::tree_operations {
    template <typename string_to_node>
    struct string_to_node_traits;

    template <typename string_to_node>
    struct is_string_to_node_type : public std::false_type {};

    template <typename string_to_node>
    constexpr bool is_string_to_node_type_v = is_string_to_node_type<string_to_node>::value;

    template<>
    struct string_to_node_traits<StringToNode> {
    private:
        using adapt_type = StringToNode;
    public://concepts
        using node_instance_type = StringToNode::node_instance_type;
        static bool has_node(const adapt_type &stringToNode, const std::string &name) {return stringToNode.hasNode(name);}
        static node_instance_type get_node(const adapt_type &stringToNode, const std::string &name) {return stringToNode(name);}
    };

    template<>
    struct is_string_to_node_type<StringToNode> : public std::true_type {};
}

#endif //GP_TREE_OPERATIONS_STRING_TO_NODE_TRAIT
