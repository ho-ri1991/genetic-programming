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
    constexpr bool is_random_node_generator_type_v = is_random_node_generator_type<RandomNodeGenerator>::value;

    template <typename RandomEngine, typename ...SupportConstTypes>
    struct random_node_generator_traits<tree::RandomNodeGenerator<RandomEngine, SupportConstTypes...>> {
    private:
        using adapt_type = tree::RandomNodeGenerator<RandomEngine, SupportConstTypes...>;
    public:
        using node_instance_type = typename adapt_type::node_instance_type;
        using type_info = typename adapt_type::type_info;
        using tree_property = typename adapt_type::tree_property;
        static node_instance_type generate_node(const adapt_type& randomNodeGenerator, const type_info& returnType, const tree_property& treeProperty, RandomEngine& rnd){
            return randomNodeGenerator.generateNode(returnType, treeProperty, rnd);
        }
        static node_instance_type generate_leaf_node(const adapt_type& randomNodeGenerator, const type_info& returnType, const tree_property& treeProperty, RandomEngine& rnd){
            return randomNodeGenerator.generateLeafNode(returnType, treeProperty, rnd);
        }
    };

    template <typename RandomEngine, typename ...SupportTypes>
    struct is_random_node_generator_type<tree::RandomNodeGenerator<RandomEngine, SupportTypes...>>: std::true_type{};
}

#endif
