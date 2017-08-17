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
}

#endif