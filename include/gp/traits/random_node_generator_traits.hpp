#ifndef GP_TRAITS_RANDOM_NODE_GENETATOR_TRAITS
#define GP_TRAITS_RANDOM_NODE_GENETATOR_TRAITS

#include <type_traits>
#include <gp/tree/random_node_generator.hpp>

namespace gp::traits {
    template <typename RandomNodeGenerator>
    struct random_node_generator_traits;

    template <typename RandomNodeGenerator>
    struct is_random_node_generator_type: std::false_type{};

    template <typename RandomNodeGenerator>
    constexpr bool is_random_node_generator_v = is_random_node_generator_type::value;

    template <>
    struct random_node_generator_traits<tree::RandomNodeGenerator> {
    private:
        using adapt_type = tree::RandomNodeGenerator;
    public:
        using node_instance_type = adapt_type::node_instance_type;
        using type_info = adapt_type::type_info;
        using tree_property = adapt_type::tree_property;
        node_instance_type generate_node(const adapt_type& randomNodeGenerator, const type_info& returnType, const tree_property& treeProperty){
            return randomNodeGenerator.generateNode(returnType, treeProperty);
        }
        node_instance_type generate_leaf_node(const adapt_type& randomNodeGenerator, const type_info& returnType, const tree_property& treeProperty){
            return randomNodeGenerator.generateLeafNode(returnType, treeProperty);
        }
    };

    template <>
    struct is_random_node_generator_type: std::true_type{};
}

#endif
