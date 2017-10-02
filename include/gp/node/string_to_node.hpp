#ifndef GP_TREE_OPERATIONS_STRING_TO_NODE
#define GP_TREE_OPERATIONS_STRING_TO_NODE

#include <gp/node/node_interface.hpp>
#include <unordered_map>
#include <gp/node/const_node.hpp>

namespace gp::node {
    class StringToNode {
    public:
        using node_instance_type = node::NodeInterface::node_instance_type;
    private:
        using key_type = std::string;
        using value_type = node_instance_type;
        using ContainerType = std::unordered_map<key_type, value_type>;
        ContainerType container;
    private:
        //this method is for formatting ConstNodeName
        static std::string formatNodeName(std::string name) {
            auto begin = name.find(node::const_node::nameHeader);
            if(begin == 0){//in case of const node
                auto end = name.find(node::const_node::propertySeparator);
                if(end == std::string::npos) return name;
                return name.substr(0, end) + node::const_node::nameDelimiter;
            }
            begin = name.find(node::local_variable_node::nameHeader);
            if(begin == 0){//in case of local variable node
                auto end = name.rfind(node::local_variable_node::propertySeparator);
                if(end == std::string::npos) return name;
                return name.substr(0, end) + node::local_variable_node::nameDelimiter;
            }
            return name;
        }
    public:
        template <typename String>
        bool hasNode(String&& name) const {
            return container.find(formatNodeName(std::forward<String>(name))) != std::end(container);
        }
        template <typename String>
        node_instance_type operator()(String&& name)const {
            auto itr = container.find(formatNodeName(name));
            if (itr == std::end(container)) return nullptr;
            else {
                auto node = itr->second->clone();
                node->setNodePropertyByNodeName(std::forward<String>(name));
                return node;
            }
        }
    public:
        void registerNode(node_instance_type node) {
            container[formatNodeName(node->getNodeName())] = std::move(node);
        }
        template <typename String>
        void deleteNode(String&& name) {
            auto itr = container.find(std::forward<String>(name));
            if(itr != std::end(container))container.erase(itr);
        }
    };
}

#endif
