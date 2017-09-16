#ifndef GP_TREE_TREE_OPERATIONS_TREE_OPERATIONS
#define GP_TREE_TREE_OPERATIONS_TREE_OPERATIONS

#include <iostream>
#include <gp/gp_config.hpp>
#include <gp/traits/node_traits.hpp>
#include <gp/traits/tree_traits.hpp>
#include <gp/traits/string_to_node_traits.hpp>
#include "detail/detail.hpp"

namespace gp::tree_operations {
    template <typename node>
    std::size_t getDepth(const node& node_) {
        static_assert(traits::is_node_type_v<node> || traits::is_node_ptr_type_v<node>);
        if constexpr(traits::is_node_ptr_type_v<node>) {
            static_assert(traits::is_node_type_v<std::decay_t<decltype(*node_)>>);
            return getDepth(*node_);
        } else {
            using trait = traits::node_traits<node>;
            if (!trait::has_parent(node_)) return 0;
            else return 1 + getDepth(trait::get_parent(node_));
        }
    }

    template <typename node>
    std::size_t getHeight(const node& node_) {
        static_assert(traits::is_node_type_v<node> || traits::is_node_ptr_type_v<node>);
        if constexpr (traits::is_node_ptr_type_v<node>) {
            static_assert(traits::is_node_type_v<std::decay_t<decltype(*node_)>>);
            return getHeight(*node_);
        } else {
            using trait = traits::node_traits<node>;
            if (trait::get_child_num(node_) == 0) return 0;
            std::size_t ans = 0;
            for (int i = 0; i < trait::get_child_num(node_); ++i) {
                if (!trait::has_child(node_, i))
                    throw std::runtime_error("the operating tree structure must not have null node");
                ans = std::max(ans, getHeight(trait::get_child(node_, i)));
            }
            return ans + 1;
        }
    }

    template <typename output_node>
    void writeTree(const output_node& rootNode, std::ostream& out) {
        static_assert(traits::is_output_node_type_v<output_node> || traits::is_output_node_ptr_type_v<output_node>);
        if constexpr (traits::is_output_node_ptr_type_v<output_node>) {
            static_assert(traits::is_node_type_v<std::decay_t<decltype(*rootNode)>>);
            return writeTree(*rootNode, out);
        } else {
            return detail::WriteTreeHelper::writeTreeHelper(rootNode, out);
        }
    }

    template <typename string_to_node, typename tree_property>
    auto readTree(const string_to_node& stringToNode, const tree_property& treeProperty, std::istream& in) {
        static_assert(traits::is_string_to_node_type_v<string_to_node>);
        static_assert(traits::is_tree_property_type_v<tree_property>);
        using node_type = typename traits::string_to_node_traits<string_to_node>::node_instance_type;
        static_assert(traits::is_input_node_type_v<node_type> || traits::is_input_node_ptr_type_v<node_type>);
        if constexpr (traits::is_input_node_ptr_type_v<node_type>) {
            //check is same node_traies<node>::tree_properry and tree_property
            static_assert(std::is_same_v<typename traits::input_node_traits<typename std::decay_t<decltype(*std::declval<node_type>())>>::tree_property, tree_property>);
        } else {
            static_assert(std::is_same_v<typename traits::input_node_traits<node_type>::tree_property, tree_property>);
        }
        return detail::ReadTreeHelper::readTreeHelper(stringToNode, treeProperty, in);
    }

    template <typename TreeProperty, typename RandomNodeGenerator>
    auto generateTreeRandom(const TreeProperty& treeProperty, const RandomNodeGenerator& randomNodeGenerator, std::size_t maxTreeDepth) {
        if(maxTreeDepth == 0) {

        } else {

        }
    };
}

#endif
