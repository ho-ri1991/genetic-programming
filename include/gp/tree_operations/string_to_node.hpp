#ifndef GP_TREE_OPERATIONS_STRING_TO_NODE
#define GP_TREE_OPERATIONS_STRING_TO_NODE

#include <gp/node/node_interface.hpp>
#include <unordered_map>

namespace gp::tree_operations {
    class StringToNode {
    public:
        using node_instance_type = node::NodeInterface::node_instance_type;
    private:
        using key_type = std::string;
        using value_type = node_instance_type;
        using ContainerType = std::unordered_map<key_type, value_type>;
        ContainerType container;
    public:
        template <typename String>
        bool hasNode(String&& name) const {
            return container.find(std::forward<String>(name)) != std::end(container);
        }
        template <typename String>
        node_instance_type operator()(String&& name)const {
            auto itr = container.find(std::forward<String>(name));
            if (itr == std::end(container)) return nullptr;
            else return itr->second->clone();
        }
    public:
        template <typename String>
        void setNodeNamePair(node_instance_type node, String&& name);
    };
}

#endif