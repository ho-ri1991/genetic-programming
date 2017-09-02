#ifndef GP_TREE_OPERATIONS_TREE_TRAITS
#define GP_TREE_OPERATIONS_TREE_TRAITS

#include <type_traits>
#include <tree/tree.hpp>
#include "string_to_node.hpp"

namespace gp::tree_operations {
    template <typename tree>
    struct tree_traits;

    template <typename tree>
    struct is_tree_type: public std::false_type{};

    template <>
    struct tree_traits<tree::Tree> {
    private:
        using this_type = tree::Tree;
    public:
//        using node_type = typename tree::NodeType;
//        static node_type& get_root_node(tree& tree_) {return tree_.getRootNode();}
//        static const node_type& get_root_node(const tree& tree_) {return tree_.getRootNode();}
    };

    template <typename tree_property>
    struct tree_property_traits;

    template <typename tree_property>
    struct is_tree_property_type: public std::false_type{};

    template <typename tree_property>
    constexpr bool is_tree_property_type_v = is_tree_property_type<tree_property>::value;

    template <>
    struct tree_property_traits<tree::TreeProperty>{
    private:
        using adapt_type = tree::TreeProperty;
    public://concepts
        using type_info = adapt_type::type_info;
        const type_info& get_return_type(const adapt_type& property){return *property.returnType;}
        const type_info& get_argument_type(const adapt_type& property, std::size_t n){return *property.argumentTypes[n];}
        std::size_t get_argument_num(const adapt_type& property){return std::size(property.argumentTypes);}
    };

    template <>
    struct is_tree_property_type<tree::TreeProperty>: public std::true_type{};
}

#endif