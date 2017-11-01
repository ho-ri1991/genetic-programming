#ifndef GP_NODE_INTERFACE
#define GP_NODE_INTERFACE

#include <memory>
#include <any>
#include <gp/utility/evaluation_context.hpp>
#include <gp/utility/type.hpp>

namespace gp::node{
    enum class NodeType {
        Normal,
        Argument,
        LocalVariable,
        Progn,
        Subroutine,
        Const
    };

    //forward declarations for friend class declarations
    template <typename T>
    class TypedNodeInterface;
    template <typename T>
    class NodeBase;
    template <typename T, std::size_t n>
    class PrognNode;

    class NodeInterface {
    public:
        using type_info = utility::TypeInfo;
        using type_index = utility::TypeIndex;
        using any_t = utility::any;
        using node_instance_type = std::unique_ptr<NodeInterface>;
        using const_node_instance_type = std::unique_ptr<const NodeInterface>;
        template <typename NodeType, typename ...Ts>
        static node_instance_type createInstance(Ts&&... args) {return std::make_unique<NodeType>(std::forward<Ts>(args)...);}
    public:
        using variable_index_type = std::size_t;
    private:
        template <typename T> friend class TypedNodeInterface;
        template <typename T> friend class NodeBase;
        template <typename T, std::size_t n> friend class PrognNode;
        NodeInterface() = default; //only the TypedNodeInterface can construct NodeInterface. Users who want to create custom node must derive NodeBase or TypedNodeInterface.
        virtual void setParent(NodeInterface* node) = 0;
    public:
        //type information methods
        virtual const type_info& getReturnType()const noexcept= 0;
        virtual const type_info& getChildReturnType(std::size_t n)const noexcept = 0;
        //methods for children
        virtual std::size_t getChildNum()const noexcept = 0;
        virtual bool hasChild(std::size_t n)const noexcept = 0;
        virtual NodeInterface& getChild(std::size_t n) = 0;
        virtual const NodeInterface& getChild(std::size_t n)const = 0;
        virtual node_instance_type setChild(std::size_t n, node_instance_type node) = 0;
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
        virtual node_instance_type clone()const = 0;
        virtual NodeType getNodeType()const noexcept = 0;
    public:
        virtual ~NodeInterface() = default;
        NodeInterface(NodeInterface&&) = default;
        NodeInterface& operator=(NodeInterface&&) = default;
        NodeInterface(const NodeInterface&) = default;
        NodeInterface& operator=(const NodeInterface&) = default;
    };
}

#endif
