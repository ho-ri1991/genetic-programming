#ifndef GP_TREE_OPERATIONS_NODE_TRAITS
#define GP_TREE_OPERATIONS_NODE_TRAITS

#include <typeinfo>
#include <memory>
#include <type_traits>
#include <node/node_interface.hpp>

namespace gp::tree_operations {
    template <typename node>
    struct node_traits;

    template <typename node>
    struct is_node_type: std::false_type{};

    template <>
    struct node_traits<node::NodeInterface> {
    private:
        using adapt_type = node::NodeInterface;
    public://concepts
        //type when getting children or parent
        using child_get_type = node::NodeInterface;             //type of return value of get_child(node&, std::size_t)
        using child_set_type = adapt_type::node_instance_type;  //type of argument of set_child(node&, std::size_t, child_set_type)
        using parent_get_type = node::NodeInterface;            //type of return value of get_parent(node&)
        //methods for children
        static std::size_t get_child_num(const adapt_type& node_) {return node_.getChildNum();}
        static bool has_child(const adapt_type& node_, std::size_t n) {return node_.hasChild(n);}
        static child_get_type& get_child(adapt_type& node_, std::size_t n) {return node_.getChildNode(n);}
        static const child_get_type& get_child(const adapt_type& node_, std::size_t n) {return node_.getChildNode(n);}
        static void set_child(adapt_type& node_, std::size_t n, child_set_type child) {node_.setChild(n, std::move(child));}
        //methods for parent
        static bool has_parent(const adapt_type& node_) {return node_.hasParent();}
        static parent_get_type& get_parent(adapt_type& node_) {return node_.getParent();}
        static const parent_get_type& get_parent(const adapt_type& node_) {return node_.getParent();}
    };

    template <>
    struct is_node_type<node::NodeInterface>: std::true_type{};

    template <>
    struct node_traits<node::NodeInterface::node_instance_type> {
    private:
        using adapt_type = node::NodeInterface::node_instance_type;
    public://concepts
        //type when getting children or parent
        using child_get_type = node::NodeInterface;                         //type of return value of get_child(node&, std::size_t)
        using child_set_type = node::NodeInterface::node_instance_type ;    //type of argument of set_child(node&, std::size_t, child_set_type)
        using parent_get_type = node::NodeInterface;                        //type of return value of get_parent(node&)
        //methods for children
        static std::size_t get_child_num(const adapt_type& node_) {return node_->getChildNum();}
        static bool has_child(const adapt_type& node_, std::size_t n) {return node_->hasChild(n);}
        static child_get_type& get_child(adapt_type& node_, std::size_t n) {return node_->getChildNode(n);}
        static const child_get_type& get_child(const adapt_type& node_, std::size_t n) {return node_->getChildNode(n);}
        static void set_child(adapt_type& node_, std::size_t n, child_set_type child) {node_->setChild(n, std::move(child));}
        //methods for parent
        static bool has_parent(const adapt_type& node_) {return node_->hasParent();}
        static parent_get_type& get_parent(adapt_type& node_) {return node_->getParent();}
        static const parent_get_type& get_parent(const adapt_type& node_) {return node_->getParent();}
    };

    template <>
    struct is_node_type<node::NodeInterface::node_instance_type>: std::true_type{};

    template <typename typed_node>
    struct typed_node_traits;

    template <typename typed_node>
    struct is_typed_node_type: public std::false_type{};

    template <>
    struct typed_node_traits<node::NodeInterface>: private node_traits<node::NodeInterface> {
    private:
        using adapt_type = node::NodeInterface;
    public://concepts
        using child_get_type = node_traits<adapt_type>::child_get_type;
        using parent_get_type = node_traits<adapt_type>::parent_get_type;
        using child_set_type = node_traits<adapt_type>::child_set_type;
        using node_traits<adapt_type>::get_child_num;
        using node_traits<adapt_type>::has_child;
        using node_traits<adapt_type>::get_child;
        using node_traits<adapt_type>::set_child;
        using node_traits<adapt_type>::has_parent;
        using node_traits<adapt_type>::get_parent;
        //type of type information
        using type_info = typename adapt_type::type;
        //methods for type information
        static type_info get_return_type(const adapt_type& node_) {return node_.getReturnType();}
        static type_info get_child_return_type(const adapt_type& node_, std::size_t n) {return node_.getChildReturnType(n);}
    };

    template <>
    struct is_typed_node_type<node::NodeInterface>: public std::true_type{};

    template <>
    struct typed_node_traits<node::NodeInterface::node_instance_type>: private node_traits<node::NodeInterface::node_instance_type> {
    private:
        using adapt_type = node::NodeInterface::node_instance_type;
    public://concepts
        using child_get_type = node_traits<adapt_type>::child_get_type;
        using parent_get_type = node_traits<adapt_type>::parent_get_type;
        using child_set_type = node_traits<adapt_type>::child_set_type;
        using node_traits<adapt_type>::get_child_num;
        using node_traits<adapt_type>::has_child;
        using node_traits<adapt_type>::get_child;
        using node_traits<adapt_type>::set_child;
        using node_traits<adapt_type>::has_parent;
        using node_traits<adapt_type>::get_parent;
        //type of type information
        using type_info = node::NodeInterface::type;
        //methods for type information
        static type_info get_return_type(const adapt_type& node_) {return node_->getReturnType();}
        static type_info get_child_return_type(const adapt_type& node_, std::size_t n) {return node_->getChildReturnType(n);}
    };

    template <typename output_node>
    struct output_node_traits;

    template <typename output_node>
    struct is_output_node_type: public std::false_type{};

    template <>
    struct output_node_traits<node::NodeInterface>: private node_traits<node::NodeInterface> {
    private:
        using adapt_type = node::NodeInterface;
    public:
        using child_get_type = node_traits<adapt_type>::child_get_type;
        using parent_get_type = node_traits<adapt_type>::parent_get_type;
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
    struct output_node_traits<node::NodeInterface::node_instance_type>: private node_traits<node::NodeInterface::node_instance_type> {
    private:
        using adapt_type = node::NodeInterface::node_instance_type;
    public:
    public:
        using child_get_type = node_traits<adapt_type>::child_get_type;
        using parent_get_type = node_traits<adapt_type>::parent_get_type;
        using node_traits<adapt_type>::has_child;
        using node_traits<adapt_type>::get_child;
        using node_traits<adapt_type>::get_child_num;
        using node_traits<adapt_type>::has_parent;
        using node_traits<adapt_type>::get_parent;
        //methods for output
        static std::string get_node_name(const adapt_type& node_) {return node_->getNodeName();}
    };

    template <>
    struct is_output_node_type<node::NodeInterface::node_instance_type>: public std::true_type{};

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