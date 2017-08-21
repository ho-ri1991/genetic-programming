#ifndef GP_NODE_BASIC_OPERATION_NODE
#define GP_NODE_BASIC_OPERATION_NODE

#include "node_base.hpp"
#include <utility/left_hand_value.hpp>

namespace gp::node {
    template <typename T>
    class SubstitutionNode: public NodeBase<T(utility::LeftHandValue<T>, T)> {
        using ThisType = SubstitutionNode;
    private:
        T evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            auto lvalue = std::get<0>(this->children)->evaluate(evaluationContext);
            if(!lvalue) {
                evaluationContext.setEvaluationStatusWithoutUpdate(utility::EvaluationStatus::InvalidLeftHandValue);
                return utility::getDefaultValue<T>();
            }
            lvalue.getRef() = std::get<1>(this->children)->evaluate(evaluationContext);
            return lvalue.getRef();
        }
    public:
        std::string getNodeName()const override {
            return std::string("Substitute<") + utility::typeName<T>() + std::string(">");
        }
        std::shared_ptr<NodeInterface> clone()const override {return std::make_shared<ThisType>();}
    };

    template <typename T>
    class AddNode: public NodeBase<T(T, T)> {
        using ThisType = AddNode;
    private:
        T evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            auto [a1, a2] = evaluateChildren(this->children, evaluationContext);
            return a1 + a2;
        }
    public:
        std::string getNodeName()const override {
            return std::string("Add<") + utility::typeName<T>() + std::string(">");
        }
        std::shared_ptr<NodeInterface> clone()const override {return std::make_shared<ThisType>();}
    };

    template <typename T>
    class SubtractNode: public NodeBase<T(T, T)> {
        using ThisType = SubtractNode;
    private:
        T evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            auto [a1, a2] = evaluateChildren(this->children, evaluationContext);
            return a1 - a2;
        }
    public:
        std::string getNodeName()const override {
            return std::string("Sub<") + utility::typeName<T>() + std::string(">");
        }
        std::shared_ptr<NodeInterface> clone()const override {return std::make_shared<ThisType>();}
    };

    template <typename T>
    class MultiplyNode: public NodeBase<T(T, T)> {
        using ThisType = MultiplyNode;
    private:
        T evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            auto [a1, a2] = evaluateChildren(this->children, evaluationContext);
            return a1 * a2;
        }
    public:
        std::string getNodeName()const override {
            return std::string("Mult<") + utility::typeName<T>() + std::string(">");
        }
        std::shared_ptr<NodeInterface> clone()const override {return std::make_shared<ThisType>();}
    };

    template <typename T, bool is_arithmetic = std::is_arithmetic_v<T>>
    class DivisionNode;

    template <typename T>
    class DivisionNode<T, true>: public NodeBase<T(T, T)> {
        using ThisType = DivisionNode;
    private:
        T evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            auto [c1, c2] = evaluateChildren(this->children, evaluationContext);
            if(c2 == 0){
                evaluationContext.setEvaluationStatusWithoutUpdate(utility::EvaluationStatus::InvalidValue);
                return utility::DefaultInitializer<T>();
            }
            return c1 / c2;
        }
    public:
        std::string getNodeName()const override {
            return std::string("Div<") + utility::typeName<T>() + std::string(">");
        }
        std::shared_ptr<NodeInterface> clone()const override {return std::make_shared<ThisType>();}
    };

    class AndNode: public NodeBase<bool(bool, bool)> {
        using ThisType = AndNode;
    private:
        bool evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            auto [b1, b2] = evaluateChildren(this->children, evaluationContext);
            return b1 && b2;
        }
    public:
        std::string getNodeName()const override {return std::string("AND");}
        std::shared_ptr<NodeInterface> clone()const override {return std::make_shared<ThisType>();}
    };

    class OrNode: public NodeBase<bool(bool, bool)> {
        using ThisType = OrNode;
    private:
        bool evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            auto [b1, b2] = evaluateChildren(this->children, evaluationContext);
            return b1 || b2;
        }
    public:
        std::string getNodeName()const override {return std::string("OR");}
        std::shared_ptr<NodeInterface> clone()const override {return std::make_shared<ThisType>();}
    };

    class NotNode: public NodeBase<bool(bool)> {
        using ThisType = NotNode;
    private:
        bool evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            return !std::get<0>(this->children)->evaluate(evaluationContext);
        }
    public:
        std::string getNodeName()const override {return std::string("NOT");}
        std::shared_ptr<NodeInterface> clone()const override {return std::make_shared<ThisType>();}
    };

    template <typename T>
    class IfNode: public NodeBase<T(bool, T, T)> {
        using ThisType = IfNode;
    private:
        T evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            auto cond = std::get<0>(this->children)->evaluate(evaluationContext);
            if(cond) {
                return std::get<1>(this->children)->evaluate(evaluationContext);
            } else {
                return std::get<2>(this->children)->evaluate(evaluationContext);
            }
        }
    public:
        std::string getNodeName()const override {
            return std::string("If<") + utility::typeName<T>() + std::string(">");
        }
        std::shared_ptr<NodeInterface> clone()const override {return std::make_shared<ThisType>();}
    };
}

#endif