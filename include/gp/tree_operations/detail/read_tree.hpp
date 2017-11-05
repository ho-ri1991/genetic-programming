#ifndef GP_TREE_OPERATIONS_DETAIL_READ_TREE
#define GP_TREE_OPERATIONS_DETAIL_READ_TREE

#include "defines.hpp"
#include <gp/utility/result.hpp>
#include <gp/traits/node_traits.hpp>
#include <gp/traits/string_to_node_traits.hpp>
#include <optional>

namespace gp::tree_operations::detail {
    class ReadTreeImpl {
    private:
        static std::string getNodeNameFromStream(std::istream& in) {
            std::string line = "";
            while(std::getline(in, line)){
                auto headerPoint = line.find(io::NODE_HEADER);
                if(headerPoint != std::string::npos) {
                    //node name found, remove white spaces at the end of the line
                    auto lastItr = std::find_if(std::rbegin(line), std::rend(line), [](auto c){return c != ' ';});
                    auto beg = headerPoint + std::strlen(io::NODE_HEADER);
                    auto lastIdx = std::size(line) - (lastItr - std::rbegin(line)) - 1;
                    return line.substr(beg, lastIdx - beg + 1);
                }
            }
            return "";
        }
        //@return err message
        template <typename input_node, typename string_to_node, typename tree_property>
        static std::optional<std::string> readHelper(input_node& node_, const string_to_node& stringToNode, const tree_property& treeProperty, std::istream& in) {
            if constexpr(traits::is_input_node_ptr_type_v<std::decay_t<decltype(node_)>>) {
                return readHelper(*node_, stringToNode, treeProperty, in);
            } else {
                using node_trait = traits::input_node_traits<input_node>;
                using string_to_node_trait = traits::string_to_node_traits<string_to_node>;
                using tree_property_trait = traits::tree_property_traits<tree_property>;
                static_assert(std::is_same_v<typename node_trait::node_instance_type, typename string_to_node_trait::node_instance_type>);

                for(int i = 0; i < node_trait::get_child_num(node_); ++i){
                    auto nextName = getNodeNameFromStream(in);
                    if(!string_to_node_trait::has_node(stringToNode, nextName)) {
                        return std::optional<std::string>("failed to load tree, unknown node name: " + nextName);
                    }
                    auto nextNode = string_to_node_trait::get_node(stringToNode, nextName);
                    //check the validity of the return type of child
                    if constexpr (traits::is_input_node_ptr_type_v<std::decay_t<decltype(nextNode)>>) {
                        if(!node_trait::is_valid_child(node_, i, *nextNode, treeProperty)) {
                            return std::optional<std::string>("failed to load tree, type mismatch between " + nextName + " and parent node.");
                        }
                    } else {
                        if(!node_trait::is_valid_child(node_, i, nextNode, treeProperty)) {
                            return std::optional<std::string>("failed to load tree, type mismatch between " + nextName + " and parent node.");
                        }
                    }
                    if constexpr (std::is_move_constructible_v<decltype(nextNode)>) {
                        node_trait::set_child(node_, i, std::move(nextNode));
                    } else {
                        node_trait::set_child(nextNode, i);
                    }
                    auto errMsg = readHelper(node_trait::get_child(node_, i), stringToNode, treeProperty, in);
                    if(errMsg) return errMsg;
                }
                return std::optional<std::string>{};
            }
        };
    public:
        template <typename string_to_node, typename tree_property>
        static auto read(const string_to_node& stringToNode, const tree_property& treeProperty, std::istream& in)
            -> utility::Result<typename traits::string_to_node_traits<string_to_node>::node_instance_type> {
            using node_instance_type = typename traits::string_to_node_traits<string_to_node>::node_instance_type;
            auto nodeName = getNodeNameFromStream(in);
            using string_to_node_trait = traits::string_to_node_traits<string_to_node>;
            if(nodeName.empty()) return utility::result::err<node_instance_type>("failed to load tree, node header \"+--\" found but no node name found.");
            if(!string_to_node_trait::has_node(stringToNode, nodeName)) return utility::result::err<node_instance_type>("failed to load tree, unknown node name: " + nodeName);

            auto rootNode = stringToNode(nodeName);
            auto errMsg = readHelper(rootNode, stringToNode, treeProperty, in);
            return errMsg ? utility::result::err<node_instance_type>(std::move(*errMsg)) : utility::result::ok(std::move(rootNode));
        };
    };
}

#endif
