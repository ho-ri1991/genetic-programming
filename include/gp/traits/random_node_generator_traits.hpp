#ifndef GP_TRAITS_RANDOM_NODE_GENETATOR_TRAITS
#define GP_TRAITS_RANDOM_NODE_GENETATOR_TRAITS

#include <type_traits>

namespace gp::traits {
    template <typename RandomNodeGenerator>
    struct random_node_generator_traits;

    template <typename RandomNodeGenerator>
    struct is_random_node_generator_type: std::false_type{};

    template <typename RandomNodeGenerator>
    constexpr bool is_random_node_generator_type_v = is_random_node_generator_type<RandomNodeGenerator>::value;
}

#endif
