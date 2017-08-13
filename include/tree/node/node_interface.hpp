#ifndef TREE_NODE_INTERFACE
#define TREE_NODE_INTERFACE

#include<typeinfo>
#include<memory>

namespace tree::node{
    class NodeInterface {
    public:
        virtual const std::type_info* const getReturnType()const noexcept= 0;
        virtual const std::type_info* const getChildReturnType(std::size_t n)const noexcept = 0;
        virtual std::string getNodeName()const = 0;
        virtual std::size_t getChildNum()const = 0;
        virtual std::shared_ptr<NodeInterface> getChildNode(std::size_t n) noexcept = 0;
        virtual std::shared_ptr<const NodeInterface> getChildNode(std::size_t n)const noexcept = 0;
        virtual void setChild(std::size_t n, std::shared_ptr<NodeInterface> node) = 0;
        virtual std::shared_ptr<NodeInterface> getParent() noexcept = 0;
        virtual std::shared_ptr<const NodeInterface> getParent()const noexcept = 0;
        virtual void setParent(std::shared_ptr<NodeInterface> node) = 0;
        virtual std::shared_ptr<NodeInterface> clone()const = 0;
    public:
        virtual ~NodeInterface() = default;
        NodeInterface(NodeInterface&&) = default;
        NodeInterface& operator=(NodeInterface&&) = default;
        NodeInterface(const NodeInterface&) = default;
        NodeInterface& operator=(const NodeInterface&) = default;
        NodeInterface() = default;
    };
}

#endif