#ifndef GP_TREE_OPERATIONS_NODE_TRAITS
#define GP_TREE_OPERATIONS_NODE_TRAITS

#include <memory>
#include <type_traits>
#include <gp/node/node_interface.hpp>

namespace gp::tree_operations {
    template <typename T>
    using remove_cv_reference_t = std::remove_cv_t<std::remove_reference_t<T>>;

    template <typename node>
    struct node_traits;

    template <typename node>
    struct is_node_type: std::false_type{};

    template <typename node>
    constexpr bool is_node_type_v = is_node_type<node>::value;

    template <typename node_ptr>
    struct is_node_ptr_type: std::false_type{};

    template <typename node_ptr>
    constexpr bool is_node_ptr_type_v = is_node_ptr_type<node_ptr>::value;

    template <>
    struct node_traits<node::NodeInterface> {
    private:
        using adapt_type = node::NodeInterface;
    public://concepts
        using node_instance_type = adapt_type::node_instance_type;
        //methods for children
        static std::size_t get_child_num(const adapt_type& node_) {return node_.getChildNum();}
        static bool has_child(const adapt_type& node_, std::size_t n) {return node_.hasChild(n);}
        static adapt_type& get_child(adapt_type& node_, std::size_t n) {return node_.getChildNode(n);}
        static const adapt_type& get_child(const adapt_type& node_, std::size_t n) {return node_.getChildNode(n);}
        static void set_child(adapt_type& node_, std::size_t n, node_instance_type child) {node_.setChild(n, std::move(child));}
        //methods for parent
        static bool has_parent(const adapt_type& node_) {return node_.hasParent();}
        static adapt_type& get_parent(adapt_type& node_) {return node_.getParent();}
        static const adapt_type& get_parent(const adapt_type& node_) {return node_.getParent();}
    };

    template <>
    struct is_node_type<node::NodeInterface>: public std::true_type{};

    template <>
    struct is_node_ptr_type<node::NodeInterface::node_instance_type>: public std::true_type{};

    template <>
    struct is_node_ptr_type<node::NodeInterface*>: public std::true_type{};

    template <typename typed_node>
    struct typed_node_traits;

    template <typename typed_node>
    struct is_typed_node_type: public std::false_type{};

    template <typename typed_node>
    constexpr bool is_typed_node_type_v = is_typed_node_type<typed_node>::value;

    template <typename typed_node_ptr>
    struct is_typed_node_ptr_type: public std::false_type{};

    template <typename typed_node_ptr>
    constexpr bool is_typed_node_ptr_type_v = is_typed_node_ptr_type<typed_node_ptr>::value;

    template <>
    struct typed_node_traits<node::NodeInterface>: private node_traits<node::NodeInterface> {
    private:
        using adapt_type = node::NodeInterface;
    public://concepts
        using node_instance_type = node_traits<adapt_type>::node_instance_type;
        using node_traits<adapt_type>::get_child_num;
        using node_traits<adapt_type>::has_child;
        using node_traits<adapt_type>::get_child;
        using node_traits<adapt_type>::set_child;
        using node_traits<adapt_type>::has_parent;
        using node_traits<adapt_type>::get_parent;
        //type of type information
        using type_info = typename adapt_type::type_info;
        //methods for type information
        static const type_info& get_return_type(const adapt_type& node_) {return node_.getReturnType();}
        static const type_info& get_child_return_type(const adapt_type& node_, std::size_t n) {return node_.getChildReturnType(n);}
    };

    template <>
    struct is_typed_node_type<node::NodeInterface>: public std::true_type{};

    template <>
    struct is_typed_node_ptr_type<node::NodeInterface::node_instance_type>: public std::true_type{};

    template <>
    struct is_typed_node_ptr_type<node::NodeInterface*>: public std::true_type{};

    template <typename output_node>
    struct output_node_traits;

    template <typename output_node>
    struct is_output_node_type: public std::false_type{};

    template <typename output_node>
    constexpr bool is_output_node_type_v = is_output_node_type<output_node>::value;

    template <typename output_node_ptr>
    struct is_output_node_ptr_type: public std::false_type{};

    template <typename output_node_ptr>
    constexpr bool is_output_node_ptr_type_v = is_output_node_ptr_type<output_node_ptr>::value;

    template <>
    struct output_node_traits<node::NodeInterface>: private node_traits<node::NodeInterface> {
    private:
        using adapt_type = node::NodeInterface;
    public:
        using node_traits<adapt_type>::has_child;
        using node_traits<adapt_type>::get_child;
        using node_traits<adapt_type>::get_child_num;
        using node_traits<adapt_type>::has_parent;
        using node_traits<adapt_type>::get_parent;
        //methods for output
        static std::string get_node_name(const adapt_type& node_) {return node_.getNodeName();}
    };

    template <>
    struct is_output_node_type<node::NodeInterface>: public std::true_type{};

    template <>
    struct is_output_node_ptr_type<node::NodeInterface::node_instance_type>: public std::true_type{};

    template <>
    struct is_output_node_ptr_type<node::NodeInterface*>: public std::true_type{};

    template <typename node>
    struct input_node_traits: private typed_node_traits<node> {
        using node_instance_type = typename typed_node_traits<node>::node_instance_type;
        using type_info = typename typed_node_traits<node>::type_info;
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