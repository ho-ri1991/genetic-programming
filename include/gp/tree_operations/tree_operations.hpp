#ifndef GP_TREE_TREE_OPERATIONS_TREE_OPERATIONS
#define GP_TREE_TREE_OPERATIONS_TREE_OPERATIONS

#include <iostream>
#include <gp/gp_config.hpp>
#include "node_traits.hpp"
#include "tree_traits.hpp"
#include "string_to_node_traits.hpp"
#include "detail/detail.hpp"

namespace gp::tree_operations {
    template <typename node>
    std::size_t getDepth(const node& node_) {
        static_assert(is_node_type_v<node> || is_node_ptr_type_v<node>);
        if constexpr(is_node_ptr_type_v<node>) {
            static_assert(is_node_type_v<std::decay_t<decltype(*node_)>>);
            return getDepth(*node_);
        } else {
            using traits = node_traits<node>;
            if (!traits::has_parent(node_)) return 0;
            else return 1 + getDepth(traits::get_parent(node_));
        }
    }

    template <typename node>
    std::size_t getHeight(const node& node_) {
        static_assert(is_node_type_v<node> || is_node_ptr_type_v<node>);
        if constexpr (is_node_ptr_type_v<node>) {
            static_assert(is_node_type_v<std::decay_t<decltype(*node_)>>);
            return getHeight(*node_);
        } else {
            using traits = node_traits<node>;
            if (traits::get_child_num(node_) == 0) return 0;
            std::size_t ans = 0;
            for (int i = 0; i < traits::get_child_num(node_); ++i) {
                if (!traits::has_child(node_, i))
                    throw std::runtime_error("the operating tree structure must not have null node");
                ans = std::max(ans, getHeight(traits::get_child(node_, i)));
            }
            return ans + 1;
        }
    }

    template <typename output_node>
    void writeTree(const output_node& rootNode, std::ostream& out) {
        static_assert(is_output_node_type_v<output_node> || is_output_node_ptr_type_v<output_node>);
        if constexpr (is_output_node_ptr_type_v<output_node>) {
            static_assert(is_node_type_v<std::decay_t<decltype(*rootNode)>>);
            return writeTree(*rootNode, out);
        } else {
            return detail::WriteTreeHelper::writeTreeHelper(rootNode, out);
        }
    }

    template <typename string_to_node, typename tree_property>
    auto readTree(const string_to_node& stringToNode, const tree_property& treeProperty, std::istream& in) {
        static_assert(is_string_to_node_type_v<string_to_node>);
        static_assert(is_tree_property_type_v<tree_property>);
        using node_type = typename string_to_node_traits<string_to_node>::node_instance_type;
        static_assert(is_input_node_type_v<node_type> || is_input_node_ptr_type_v<node_type>);
        return detail::ReadTreeHelper::readTreeHelper(stringToNode, treeProperty, in);
    }
}

#endif
