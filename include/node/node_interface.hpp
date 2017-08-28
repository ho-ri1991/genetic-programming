#ifndef GP_NODE_INTERFACE
#define GP_NODE_INTERFACE

#include <typeinfo>
#include <memory>
#include <any>
#include <utility/evaluation_context.hpp>
#include <utility/type.hpp>

namespace gp::node{
    enum class NodeType {
        Normal,
        Argument,
        LocalVariable,
        Progn,
        Subroutine,
        Const
    };

    //primary declarations for friend class declarations
    template <typename T>
    class NodeBase;
    template <typename T, std::size_t n>
    class PrognNode;

    class NodeInterface {
    private:
        template <typename T> friend class NodeBase;
        template <typename T, std::size_t n> friend class PrognNode;
        virtual void setParent(NodeInterface* node) = 0;
    public:
        //type information methods
        virtual const std::type_info& getReturnType()const noexcept= 0;
        virtual const std::type_info& getChildReturnType(std::size_t n)const noexcept = 0;
        //methods for children
        virtual std::size_t getChildNum()const noexcept = 0;
        virtual bool hasChild(std::size_t n)const noexcept = 0;
        virtual NodeInterface& getChildNode(std::size_t n) = 0;
        virtual const NodeInterface& getChildNode(std::size_t n)const = 0;
        virtual void setChild(std::size_t n, std::unique_ptr<NodeInterface> node) = 0;
        //methods for parent
        virtual bool hasParent()const noexcept = 0;
        virtual NodeInterface& getParent() = 0;
        virtual const NodeInterface& getParent()const = 0;
        //method for evaluation
        virtual std::any evaluateByAny(utility::EvaluationContext&)const = 0;
        //methods for properties
        virtual void setNodePropertyByNodeName(const std::string&) = 0;
        virtual void setNodePropertyByAny(const std::any&) = 0;
        virtual std::any getNodePropertyByAny()const = 0;
        virtual std::string getNodeName()const = 0;
        virtual std::unique_ptr<NodeInterface> clone()const = 0;
        virtual NodeType getNodeType()const noexcept = 0;
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