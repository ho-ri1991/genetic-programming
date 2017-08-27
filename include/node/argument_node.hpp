#ifndef GP_NODE_ARGUMENT_NODE
#define GP_NODE_ARGUMENT_NODE

#include "node_base.hpp"
#include <utility/left_hand_value.hpp>
#include <utility/reference.hpp>

namespace gp::node {
    template <typename T, std::size_t n>
    class ArgumentNode: public NodeBase<T(void)> {
        using ThisType = ArgumentNode;
    private:
        T evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            return evaluationContext.getArgument(n).get<T>();
        }
    public:
        std::string getNodeName()const override {
            return std::string("Argument<") + utility::typeName<T>() + std::string(",") + std::to_string(n) + std::string(">");
        }
        std::unique_ptr<NodeInterface> clone()const override {return std::make_unique<ThisType>();}
        NodeType getNodeType()const noexcept override final {return NodeType::Argument;}
    };

    template <typename T, std::size_t n>
    class ArgumentNode<utility::LeftHandValue<T>, n>: public NodeBase<utility::LeftHandValue<T>(void)> {
    private:
        using ThisType = ArgumentNode;
        using ReturnType = utility::LeftHandValue<T>;
    private:
        ReturnType evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            return ReturnType(evaluationContext.getArgument(n));
        }
    public:
        std::string getNodeName()const override {
            return std::string("Argument<") + utility::typeName<ReturnType>() + std::string(",") + std::to_string(n) + std::string(">");
        }
        std::unique_ptr<NodeInterface> clone()const override {return std::make_unique<ThisType>();}
        NodeType getNodeType()const noexcept override final {return NodeType::Argument;}
    };

    template <typename T, std::size_t n>
    class ArgumentNode<utility::Reference<T>, n>: public NodeBase<utility::Reference<T>(void)> {
    private:
        using ThisType = ArgumentNode;
        using ReturnType = utility::Reference<T>;
    private:
        ReturnType evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            return ReturnType(evaluationContext.getArgument(n));
        }
    public:
        std::string getNodeName()const override {
            return std::string("Argument<") + utility::typeName<ReturnType>() + std::string(",") + std::to_string(n) + std::string(">");
        }
        std::unique_ptr<NodeInterface> clone()const override {return std::make_unique<ThisType>();}
        NodeType getNodeType()const noexcept override final {return NodeType::Argument;}
    };
}

#endif