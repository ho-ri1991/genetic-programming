#ifndef GP_NODE_INTERFACE
#define GP_NODE_INTERFACE

#include <typeinfo>
#include <memory>
#include <any>
#include <utility/evaluation_context.hpp>

namespace gp::node{
    enum class NodeType {
        Normal,
        Argument,
        LocalVariable,
        Progn,
        Subroutine,
        Const
    };

    class NodeInterface {
    public:
        virtual const std::type_info& getReturnType()const noexcept= 0;
        virtual const std::type_info& getChildReturnType(std::size_t n)const noexcept = 0;
        virtual std::string getNodeName()const = 0;
        virtual std::size_t getChildNum()const noexcept = 0;
        virtual std::shared_ptr<NodeInterface> getChildNode(std::size_t n) noexcept = 0;
        virtual std::shared_ptr<const NodeInterface> getChildNode(std::size_t n)const noexcept = 0;
        virtual void setChild(std::size_t n, std::shared_ptr<NodeInterface> node) = 0;
        virtual std::shared_ptr<NodeInterface> getParent() noexcept = 0;
        virtual std::shared_ptr<const NodeInterface> getParent()const noexcept = 0;
        virtual void setParent(std::shared_ptr<NodeInterface> node) = 0;
        virtual std::shared_ptr<NodeInterface> clone()const = 0;
        virtual std::any evaluateByAny(utility::EvaluationContext&)const = 0;
        virtual NodeType getNodeType()const noexcept = 0;
        virtual void setNodePropertyByString(const std::string&) = 0;
        virtual std::string getNodePropertyByString()const = 0;
        virtual void setNodePropertyByAny(const std::any&) = 0;
        virtual std::any getNodePropertyByAny()const = 0;
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