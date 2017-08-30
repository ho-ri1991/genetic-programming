#ifndef GP_NODE_ARGUMENT_NODE
#define GP_NODE_ARGUMENT_NODE

#include "node_base.hpp"
#include <utility/left_hand_value.hpp>
#include <utility/reference.hpp>

namespace gp::node {
    template <typename T, std::size_t n>
    class ArgumentNode: public NodeBase<T(void)> {
        using ThisType = ArgumentNode;
        using node_instance_type = NodeInterface::node_instance_type;
    private:
        T evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            return evaluationContext.getArgument(n).get<T>();
        }
    public:
        std::string getNodeName()const override {
            return std::string("Argument<") + utility::typeInfo<T>().name() + std::string(",") + std::to_string(n) + std::string(">");
        }
        node_instance_type clone()const override {return NodeInterface::createInstance<ThisType>();}
        NodeType getNodeType()const noexcept override final {return NodeType::Argument;}
    };

    template <typename T, std::size_t n>
    class ArgumentNode<utility::LeftHandValue<T>, n>: public NodeBase<utility::LeftHandValue<T>(void)> {
    private:
        using ThisType = ArgumentNode;
        using ReturnType = utility::LeftHandValue<T>;
        using node_instance_type = NodeInterface::node_instance_type;
    private:
        ReturnType evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            return ReturnType(evaluationContext.getArgument(n));
        }
    public:
        std::string getNodeName()const override {
            return std::string("Argument<") + utility::typeInfo<ReturnType>().name() + std::string(",") + std::to_string(n) + std::string(">");
        }
        node_instance_type clone()const override {return NodeInterface::createInstance<ThisType>();}
        NodeType getNodeType()const noexcept override final {return NodeType::Argument;}
    };

    template <typename T, std::size_t n>
    class ArgumentNode<utility::Reference<T>, n>: public NodeBase<utility::Reference<T>(void)> {
    private:
        using ThisType = ArgumentNode;
        using ReturnType = utility::Reference<T>;
        using node_instance_type = NodeInterface::node_instance_type;
    private:
        ReturnType evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            return ReturnType(evaluationContext.getArgument(n));
        }
    public:
        std::string getNodeName()const override {
            return std::string("Argument<") + utility::typeInfo<ReturnType>().name() + std::string(",") + std::to_string(n) + std::string(">");
        }
        node_instance_type clone()const override {return NodeInterface::createInstance<ThisType>();}
        NodeType getNodeType()const noexcept override final {return NodeType::Argument;}
    };
}

#endif