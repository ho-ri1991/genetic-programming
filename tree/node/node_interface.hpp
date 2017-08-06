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
        virtual std::shared_ptr<NodeInterface> getChildNode(std::size_t n) = 0;
        virtual std::shared_ptr<const NodeInterface> getChildNode(std::size_t n)const = 0;
        virtual void setChild(std::size_t n, std::shared_ptr<NodeInterface> node) = 0;
        virtual std::shared_ptr<NodeInterface> getParent() = 0;
        virtual std::shared_ptr<const NodeInterface> getParent()const = 0;
        virtual void setParent(std::shared_ptr<NodeInterface> node) = 0;
        virtual ~NodeInterface(){}
    };
}

#endif