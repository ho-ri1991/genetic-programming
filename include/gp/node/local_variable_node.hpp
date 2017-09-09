#ifndef GP_NODE_LOCAL_VARIABLE_NODE
#define GP_NODE_LOCAL_VARIABLE_NODE

#include "node_base.hpp"
#include <gp/utility/left_hand_value.hpp>
#include <gp/utility/reference.hpp>

namespace gp::node {
    template <typename T, node::NodeInterface::variable_index_type n>
    class LocalVariableNode: public NodeBase<T(void)> {
        using ThisType = LocalVariableNode;
        using node_instance_type = NodeInterface::node_instance_type;
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
            return std::string("LocalVariable[") + utility::typeInfo<T>().name() + std::string(",") + std::to_string(n) + std::string("]");
        }
        node_instance_type clone()const override {return NodeInterface::createInstance<ThisType>();}
        NodeType getNodeType()const noexcept override final {return NodeType::LocalVariable;}
        std::any getNodePropertyByAny()const override final {return n;}
    };

    template <typename T, node::NodeInterface::variable_index_type n>
    class LocalVariableNode<utility::LeftHandValue<T>, n>: public NodeBase<utility::LeftHandValue<T>(void)> {
        using ThisType = LocalVariableNode;
        using ReturnType = utility::LeftHandValue<T>;
        using node_instance_type = NodeInterface::node_instance_type;
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
            return std::string("LocalVariable[") + utility::typeInfo<ReturnType>().name() + std::string(",") + std::to_string(n) + std::string("]");
        }
        node_instance_type clone()const override {return NodeInterface::createInstance<ThisType>();}
        NodeType getNodeType()const noexcept override final {return NodeType::LocalVariable;}
        std::any getNodePropertyByAny()const override final {return n;}
    };

    template <typename T, node::NodeInterface::variable_index_type n>
    class LocalVariableNode<utility::Reference<T>, n>: public NodeBase<utility::Reference<T>(void)> {
    private:
        using ThisType = LocalVariableNode;
        using ReturnType = utility::Reference<T>;
        using node_instance_type = NodeInterface::node_instance_type;
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
            return std::string("LocalVariable[") + utility::typeInfo<ReturnType>().name() + std::string(",") + std::to_string(n) + std::string("]");
        }
        node_instance_type clone()const override {return NodeInterface::createInstance<ThisType>();}
        NodeType getNodeType()const noexcept override final {return NodeType::LocalVariable;}
        std::any getNodePropertyByAny()const override final {return n;}
    };
}

#endif
