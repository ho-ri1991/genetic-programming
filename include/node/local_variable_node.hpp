#ifndef GP_NODE_LOCAL_VARIABLE_NODE
#define GP_NODE_LOCAL_VARIABLE_NODE

#include "node_base.hpp"
#include <utility/left_hand_value.hpp>
#include <utility/reference.hpp>

namespace gp::node {
    template <typename T, std::size_t n>
    class LocalVariableNode: public NodeBase<T(void)> {
        using ThisType = LocalVariableNode;
    private:
        T evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            auto& localVariable = evaluationContext.getLocalVariable(n);
            if(!localVariable.hasValue()) {
                localVariable.set(utility::getDefaultValue<T>());
            }
            return localVariable.get<T>();
        }

    public:
        std::string getNodeName()const override {
            return std::string("LocalVariable<") + utility::typeName<T>() + std::string(",") + std::to_string(n) + std::string(">");
        }
        std::shared_ptr<NodeInterface> clone()const override {return std::make_shared<ThisType>();}
        NodeType getNodeType()const noexcept override final {return NodeType::LocalVariable;}
    };

    template <typename T, std::size_t n>
    class LocalVariableNode<utility::LeftHandValue<T>, n>: public NodeBase<utility::LeftHandValue<T>(void)> {
        using ThisType = LocalVariableNode;
        using ReturnType = utility::LeftHandValue<T>;
    private:
        ReturnType evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            auto& localVariable = evaluationContext.getLocalVariable(n);
            if(!localVariable.hasValue()) {
                localVariable.set(utility::getDefaultValue<T>());
            }
            return ReturnType(localVariable);
        }
    public:
        std::string getNodeName()const override {
            return std::string("LocalVariable<") + utility::typeName<ReturnType>() + std::string(",") + std::to_string(n) + std::string(">");
        }
        std::shared_ptr<NodeInterface> clone()const override {return std::make_shared<ThisType>();}
        NodeType getNodeType()const noexcept override final {return NodeType::LocalVariable;}
    };
}

#endif