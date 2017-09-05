#ifndef GP_TREE_OPERATIONS_DETAIL_WRITE_TREE
#define GP_TREE_OPERATIONS_DETAIL_WRITE_TREE

#include <gp/tree_operations/node_traits.hpp>

namespace gp::tree_operations::detail {
    class WriteTreeHelper {
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
}

#endif
