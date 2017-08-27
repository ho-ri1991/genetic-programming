#ifndef GP_IO_TREE_IO
#define GP_IO_TREE_IO

#include <fstream>
#include <node/node_interface.hpp>
#include <tree/tree.hpp>
#include <gp_config.hpp>

namespace gp::io {
    class TreeWriter {
    private:
        using ArgumentTypes = std::vector<const std::type_info*>;
        using LocalVariableTypes = std::vector<const std::type_info*>;
    private:
        const std::string outputPath;
        const utility::TypeTranslator& typeTranslator;
    private:
        static std::string createNextOffset(const std::string& currentOffset,
                                            std::shared_ptr<const node::NodeInterface> currentNode) {
            auto parent = currentNode->getParent();
            if(currentOffset.empty())return std::string(OFFSET_NUM, OFFSET_CHAR);
            if(!parent){
                return "";
            } else {
                if(parent->getChildNode(parent->getChildNum() - 1) == currentNode){
                    return currentOffset + std::string(OFFSET_NUM + 1, OFFSET_CHAR);
                } else {
                    return currentOffset + std::string(1, CONNECTION) + std::string(OFFSET_NUM, OFFSET_CHAR);
                }
            }
        }
        static std::string createNextConnectionLine(const std::string& currentConnectionLine,
                                                   std::shared_ptr<const node::NodeInterface> currentNode,
                                                   std::size_t childIndex) {
            const auto childNum = currentNode->getChildNum();
            if(childNum - 1 == childIndex){
                //in case of the last child
                auto nextChild = currentNode->getChildNode(childIndex);
                if(nextChild->getChildNum() == 0){
                    //in case of the next node is a leaf node
                    return currentConnectionLine.substr(0, std::size(currentConnectionLine) - 1);
                } else {
                    return currentConnectionLine.substr(0, std::size(currentConnectionLine) - 1) + std::string(OFFSET_NUM + 1, OFFSET_CHAR) + CONNECTION;
                }
            } else {
                auto nextChild = currentNode->getChildNode(childIndex);
                if(nextChild->getChildNum() == 0) {
                    //in case of the next node is a leaf node
                    return currentConnectionLine;
                }else {
                    return currentConnectionLine + std::string(OFFSET_NUM, OFFSET_CHAR) + CONNECTION;
                }
            }
        }
        static void writeTreeHelper(std::shared_ptr<const node::NodeInterface> node,
                                    std::ofstream& fout,
                                    std::string offsetStr = "",
                                    std::string connectionLine = std::string(OFFSET_NUM, OFFSET_CHAR) + std::string(1, CONNECTION)) {
            const auto childNum = node->getChildNum();
            fout << offsetStr << NODE_HEADER << node->getNodeName() << std::endl;
            fout << connectionLine << std::endl;
            for(int i = 0; i < node->getChildNum(); ++i) {
                writeTreeHelper(node->getChildNode(i),
                                fout,
                                createNextOffset(offsetStr, node),
                                createNextConnectionLine(connectionLine, node, i));
            }
        }
    public:
        template <typename ArgumentTypes_, typename LocalVariableTypes_, typename TreeName>
        void operator()(std::shared_ptr<const node::NodeInterface> rootNode,
                        ArgumentTypes_&& argumentTypes,
                        LocalVariableTypes_&& localVariableTypes,
                        TreeName&& treeName)const {

            std::ofstream fout(outputPath + treeName + std::string(EXTENSION));
            if(!fout.is_open()) throw std::runtime_error("file for writing tree open failed");
            writeTreeHelper(rootNode, fout);
        }
    public:
        template <typename String>
        TreeWriter(const utility::TypeTranslator& typeTranslator_, String&& outputPath_):
                typeTranslator(typeTranslator_),
                outputPath(std::forward<String>(outputPath_)){}
    };

    class TreeReader {

    };

    class SubroutineReader {

    };
}

#endif