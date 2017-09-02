#ifndef GP_TREE_TREE_OPERATIONS_TREE_OPERATIONS
#define GP_TREE_TREE_OPERATIONS_TREE_OPERATIONS

#include <iostream>
#include <gp/gp_config.hpp>
#include "node_traits.hpp"
#include "tree_traits.hpp"
#include "string_to_node_traits.hpp"

namespace gp::tree_operations {
    class TreeOperationHelper {
    private:
        template <typename node>
        static std::string createNextOffset(const std::string& currentOffset,
                                            const node& currentNode) {
            using traits = output_node_traits<node>;
            auto hasParent = traits::has_parent(currentNode);
            if(currentOffset.empty())return std::string(io::OFFSET_NUM, io::OFFSET_CHAR);
            if(!hasParent){
                return "";
            } else {
                const auto& parent = traits::get_parent(currentNode);
                auto parentChildNum = traits::get_child_num(parent);
                if(&traits::get_child(parent, parentChildNum - 1) == &currentNode){
                    return currentOffset + std::string(io::OFFSET_NUM + 1, io::OFFSET_CHAR);
                } else {
                    return currentOffset + std::string(1, io::CONNECTION) + std::string(io::OFFSET_NUM, io::OFFSET_CHAR);
                }
            }
        }
        template <typename node>
        static std::string createNextConnectionLine(const std::string& currentConnectionLine,
                                                    const node& currentNode,
                                                    std::size_t childIndex) {
            using traits = output_node_traits<node>;
            const auto childNum = traits::get_child_num(currentNode);
            if(childNum - 1 == childIndex){
                //in case of the last child
                const auto& nextChild = traits::get_child(currentNode, childIndex);
                if(traits::get_child_num(nextChild) == 0){
                    //in case of the next node is a leaf node
                    return currentConnectionLine.substr(0, std::size(currentConnectionLine) - 1);
                } else {
                    return currentConnectionLine.substr(0, std::size(currentConnectionLine) - 1) + std::string(io::OFFSET_NUM + 1, io::OFFSET_CHAR) + io::CONNECTION;
                }
            } else {
                const auto& nextChild = traits::get_child(currentNode, childIndex);
                if(traits::get_child_num(nextChild) == 0) {
                    //in case of the next node is a leaf node
                    return currentConnectionLine;
                }else {
                    return currentConnectionLine + std::string(io::OFFSET_NUM, io::OFFSET_CHAR) + io::CONNECTION;
                }
            }
        }
    public:
        template <typename node>
        static void writeTreeHelper(const node& node_,
                                    std::ostream& out,
                                    std::string offsetStr = "",
                                    std::string connectionLine = std::string(io::OFFSET_NUM, io::OFFSET_CHAR) + std::string(1, io::CONNECTION)) {
            using traits = output_node_traits<node>;
            const auto childNum = traits::get_child_num(node_);
            out << offsetStr << io::NODE_HEADER << traits::get_node_name(node_) << std::endl;
            out << connectionLine << std::endl;
            for(int i = 0; i < traits::get_child_num(node_); ++i) {
                writeTreeHelper(traits::get_child(node_, i),
                                out,
                                createNextOffset(offsetStr, node_),
                                createNextConnectionLine(connectionLine, node_, i));
            }
        }
    };

    template <typename node>
    std::size_t getDepth(const node& node_) {
        static_assert(is_node_type_v<node> || is_node_ptr_type_v<node>);
        if constexpr(is_node_ptr_type_v<node>) {
            static_assert(is_node_type_v<remove_cv_reference_t<decltype(*node_)>>);
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
            static_assert(is_node_type_v<remove_cv_reference_t<decltype(*node_)>>);
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
            static_assert(is_node_type_v<remove_cv_reference_t<decltype(*rootNode)>>);
            return writeTree(*rootNode, out);
        } else {
            return TreeOperationHelper::writeTreeHelper(rootNode, out);
        }
    }

    template <typename string_to_node, typename tree_property>
    auto readTree(const string_to_node& stringToNode, const tree_property& treeProperty, std::istream& in) {
        static_assert(is_string_to_node_type_v<string_to_node>);
        static_assert(is_tree_property_type_v<tree_property>);
        using node_type = typename string_to_node_traits<string_to_node>::node_instance_type;
        static_assert(is_node_type_v<node_type> || is_node_ptr_type_v<node_type>);
    }
}

#endif