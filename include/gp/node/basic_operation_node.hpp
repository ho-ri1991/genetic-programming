#ifndef GP_NODE_BASIC_OPERATION_NODE
#define GP_NODE_BASIC_OPERATION_NODE

#include "node_base.hpp"
#include <gp/utility/left_hand_value.hpp>

namespace gp::node {
    template <typename T>
    class SubstitutionNode: public NodeBase<T(utility::LeftHandValue<T>, T)> {
        using ThisType = SubstitutionNode;
        using node_instance_type = NodeInterface::node_instance_type;
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
            return std::string("Substitute[") + utility::typeInfo<T>().name() + std::string("]");
        }
        node_instance_type clone()const override {return NodeInterface::createInstance<ThisType>();}
    };

    template <typename T,
              typename = std::enable_if_t<
                      std::is_convertible_v<T, decltype(std::declval<T>() + std::declval<T>())>
              >
    >
    class AddNode: public NodeBase<T(T, T)> {
        using ThisType = AddNode;
        using node_instance_type = NodeInterface::node_instance_type;
    private:
        T evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            auto [a1, a2] = evaluateChildren(this->children, evaluationContext);
            return a1 + a2;
        }
    public:
        std::string getNodeName()const override {
            return std::string("Add[") + utility::typeInfo<T>().name() + std::string("]");
        }
        node_instance_type clone()const override {return NodeInterface::createInstance<ThisType>();}
    };

    template <typename T,
            typename = std::enable_if_t<
                    std::is_convertible_v<T, decltype(std::declval<T>() - std::declval<T>())>
            >
    >
    class SubtractNode: public NodeBase<T(T, T)> {
        using ThisType = SubtractNode;
        using node_instance_type = NodeInterface::node_instance_type;
    private:
        T evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            auto [a1, a2] = evaluateChildren(this->children, evaluationContext);
            return a1 - a2;
        }
    public:
        std::string getNodeName()const override {
            return std::string("Sub[") + utility::typeInfo<T>().name() + std::string("]");
        }
        node_instance_type clone()const override {return NodeInterface::createInstance<ThisType>();}
    };

    template <typename T,
            typename = std::enable_if_t<
                    std::is_convertible_v<T, decltype(std::declval<T>() * std::declval<T>())>
            >
    >
    class MultiplyNode: public NodeBase<T(T, T)> {
        using ThisType = MultiplyNode;
        using node_instance_type = NodeInterface::node_instance_type;
    private:
        T evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            auto [a1, a2] = evaluateChildren(this->children, evaluationContext);
            return a1 * a2;
        }
    public:
        std::string getNodeName()const override {
            return std::string("Mult[") + utility::typeInfo<T>().name() + std::string("]");
        }
        node_instance_type clone()const override {return NodeInterface::createInstance<ThisType>();}
    };

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    class DivisionNode: public NodeBase<T(T, T)> {
        using ThisType = DivisionNode;
        using node_instance_type = NodeInterface::node_instance_type;
    private:
        T evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            auto [c1, c2] = evaluateChildren(this->children, evaluationContext);
            if(c2 == 0){
                evaluationContext.setEvaluationStatusWithoutUpdate(utility::EvaluationStatus::InvalidValue);
                return utility::getDefaultValue<T>();
            }
            return c1 / c2;
        }
    public:
        std::string getNodeName()const override {
            return std::string("Div[") + utility::typeInfo<T>().name() + std::string("]");
        }
        node_instance_type clone()const override {return NodeInterface::createInstance<ThisType>();}
    };

    class AndNode: public NodeBase<bool(bool, bool)> {
        using ThisType = AndNode;
        using node_instance_type = NodeInterface::node_instance_type;
    private:
        bool evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            auto [b1, b2] = evaluateChildren(this->children, evaluationContext);
            return b1 && b2;
        }
    public:
        std::string getNodeName()const override {return std::string("AND");}
        node_instance_type clone()const override {return NodeInterface::createInstance<ThisType>();}
    };

    class OrNode: public NodeBase<bool(bool, bool)> {
        using ThisType = OrNode;
        using node_instance_type = NodeInterface::node_instance_type;
    private:
        bool evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            auto [b1, b2] = evaluateChildren(this->children, evaluationContext);
            return b1 || b2;
        }
    public:
        std::string getNodeName()const override {return std::string("OR");}
        node_instance_type clone()const override {return NodeInterface::createInstance<ThisType>();}
    };

    class NotNode: public NodeBase<bool(bool)> {
        using ThisType = NotNode;
        using node_instance_type = NodeInterface::node_instance_type;
    private:
        bool evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            return !std::get<0>(this->children)->evaluate(evaluationContext);
        }
    public:
        std::string getNodeName()const override {return std::string("NOT");}
        node_instance_type clone()const override {return NodeInterface::createInstance<ThisType>();}
    };

    template <typename T>
    class IfNode: public NodeBase<T(bool, T, T)> {
        using ThisType = IfNode;
        using node_instance_type = NodeInterface::node_instance_type;
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
            return std::string("If[") + utility::typeInfo<T>().name() + std::string("]");
        }
        node_instance_type clone()const override {return NodeInterface::createInstance<ThisType>();}
    };

    template <typename T,
            typename = std::enable_if_t<
                    std::is_convertible_v<bool, decltype(std::declval<T>() > std::declval<T>())>
            >
    >
    class GreaterNode: public NodeBase<bool(T, T)> {
        using ThisType = GreaterNode;
        using node_instance_type = NodeInterface::node_instance_type;
    private:
        bool evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            auto [a1, a2] = evaluateChildren(this->children, evaluationContext);
            return a1 > a2;
        }
    public:
        std::string getNodeName()const override {
            return std::string("Greater[") + utility::typeInfo<T>().name() + std::string("]");
        }
        node_instance_type clone()const override {return NodeInterface::createInstance<ThisType>();}
    };

    template <typename T,
            typename = std::enable_if_t<
                    std::is_convertible_v<bool, decltype(std::declval<T>() >= std::declval<T>())>
            >
    >
    class GreaterEqNode: public NodeBase<bool(T, T)> {
        using ThisType = GreaterEqNode;
        using node_instance_type = NodeInterface::node_instance_type;
    private:
        bool evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            auto [a1, a2] = evaluateChildren(this->children, evaluationContext);
            return a1 >= a2;
        }
    public:
        std::string getNodeName()const override {
            return std::string("GreaterEq[") + utility::typeInfo<T>().name() + std::string("]");
        }
        node_instance_type clone()const override {return NodeInterface::createInstance<ThisType>();}
    };

    template <typename T,
            typename = std::enable_if_t<
                    std::is_convertible_v<bool, decltype(std::declval<T>() < std::declval<T>())>
            >
    >
    class LessThanNode: public NodeBase<bool(T, T)> {
        using ThisType = LessThanNode;
        using node_instance_type = NodeInterface::node_instance_type;
    private:
        bool evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            auto [a1, a2] = evaluateChildren(this->children, evaluationContext);
            return a1 < a2;
        }
    public:
        std::string getNodeName()const override {
            return std::string("Less[") + utility::typeInfo<T>().name() + std::string("]");
        }
        node_instance_type clone()const override {return NodeInterface::createInstance<ThisType>();}
    };

    template <typename T,
            typename = std::enable_if_t<
                    std::is_convertible_v<bool, decltype(std::declval<T>() <= std::declval<T>())>
            >
    >
    class LessThanEqNode: public NodeBase<bool(T, T)> {
        using ThisType = LessThanEqNode;
        using node_instance_type = NodeInterface::node_instance_type;
    private:
        bool evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            auto [a1, a2] = evaluateChildren(this->children, evaluationContext);
            return a1 <= a2;
        }
    public:
        std::string getNodeName()const override {
            return std::string("LessEq[") + utility::typeInfo<T>().name() + std::string("]");
        }
        node_instance_type clone()const override {return NodeInterface::createInstance<ThisType>();}
    };

    template <typename T,
            typename = std::enable_if_t<
                    std::is_convertible_v<bool, decltype(std::declval<T>() == std::declval<T>())>
            >
    >
    class EqualNode: public NodeBase<bool(T, T)> {
        using ThisType = EqualNode;
        using node_instance_type = NodeInterface::node_instance_type;
    private:
        bool evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            auto [a1, a2] = evaluateChildren(this->children, evaluationContext);
            return a1 == a2;
        }
    public:
        std::string getNodeName()const override {
            return std::string("Eq[") + utility::typeInfo<T>().name() + std::string("]");
        }
        node_instance_type clone()const override {return NodeInterface::createInstance<ThisType>();}
    };

    template <typename T,
            typename = std::enable_if_t<
                    std::is_convertible_v<bool, decltype(std::declval<T>() != std::declval<T>())>
            >
    >
    class NotEqualNode: public NodeBase<bool(T, T)> {
        using ThisType = NotEqualNode;
        using node_instance_type = NodeInterface::node_instance_type;
    private:
        bool evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            auto [a1, a2] = evaluateChildren(this->children, evaluationContext);
            return a1 != a2;
        }
    public:
        std::string getNodeName()const override {
            return std::string("NotEq[") + utility::typeInfo<T>().name() + std::string("]");
        }
        node_instance_type clone()const override {return NodeInterface::createInstance<ThisType>();}
    };

    template <typename T>
    class NopNode: public NodeBase<T(void)> {
        using ThisType = NopNode;
        using node_instance_type = NodeInterface::node_instance_type;
    private:
        T evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            return utility::getDefaultValue<T>();
        }
    public:
        std::string getNodeName()const override {
            return std::string("Nop[") + utility::typeInfo<T>().name() + std::string("]");
        }
        node_instance_type clone()const override {return NodeInterface::createInstance<ThisType>();}
    };

    //0th child: number of repeat, 1st child: evaluated repeatedly
    template <typename T1, typename T2, typename = std::enable_if_t<std::is_integral_v<T2>>>
    class RepeatNode: public NodeBase<T1(T2, T1)> {
        using ThisType = RepeatNode;
        using node_instance_type = NodeInterface::node_instance_type;
    private:
        T1 evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            auto num = std::get<0>(this->children)->evaluate(evaluationContext);
            if(num < 0) {
                evaluationContext.setEvaluationStatusWithoutUpdate(utility::EvaluationStatus::InvalidValue);
                return utility::getDefaultValue<T1>();
            }
            auto ans = utility::getDefaultValue<T1>();
            for(T2 i = 0; i < num; ++i) {
                auto ans = std::get<1>(this->children)->evaluate(evaluationContext);
                if(evaluationContext.getEvaluationStatus() == utility::EvaluationStatus::BreakCalled) {
                    evaluationContext.clearEvaluationStatus();
                    break;
                } else if (evaluationContext.getEvaluationStatus() == utility::EvaluationStatus::ContinueCalled) {
                    evaluationContext.clearEvaluationStatus();
                    continue;
                } else if (evaluationContext.getEvaluationStatus() != utility::EvaluationStatus::Evaluating) {
                    return utility::getDefaultValue<T1>();
                }
            }
            return ans;
        }
    public:
        std::string getNodeName()const override {
            return std::string("Repeat[") + utility::typeInfo<T1>().name() + "," + utility::typeInfo<T2>().name() + std::string("]");
        }
        node_instance_type clone()const override {return NodeInterface::createInstance<ThisType>();}
    };

    template <typename T>
    class IncrementNode: public NodeBase<T(utility::LeftHandValue<T>)> {
        using ThisType = IncrementNode;
        using node_instance_type = NodeInterface::node_instance_type;
    private:
        T evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            auto lvalue = std::get<0>(this->children)->evaluate(evaluationContext);
            if(!lvalue) {
                evaluationContext.setEvaluationStatusWithoutUpdate(utility::EvaluationStatus::InvalidValue);
                return utility::getDefaultValue<T>();
            }
            return ++(lvalue.getRef());
        }
    public:
        std::string getNodeName()const override {
            return std::string("Increment[") + utility::typeInfo<T>().name() + "]";
        }
        node_instance_type clone()const override {return NodeInterface::createInstance<ThisType>();}
    };
}

#endif
