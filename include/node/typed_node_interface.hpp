#ifndef GP_NODE_TYPED_NODE_INTERFACE
#define GP_NODE_TYPED_NODE_INTERFACE

#include "node_interface.hpp"
#include <utility/evaluation_context.hpp>
#include <utility/default_initializer.hpp>

namespace gp::node{
    template<typename T>
    class TypedNodeInterface: public NodeInterface {
    private:
        virtual T evaluationDefinition(utility::EvaluationContext&)const = 0;
    public:
        T evaluate(utility::EvaluationContext& evaluationContext)const {
            if(evaluationContext.getEvaluationStatus() == utility::EvaluationStatus::Evaluating){
                evaluationContext.incrementEvaluationCount();
                return evaluationDefinition(evaluationContext);
            }else return utility::getDefaultValue<T>();
        }
        virtual void setNodePropertyByNodeName(const std::string&)override {}
        virtual void setNodePropertyByAny(const std::any&) override {}
        virtual std::any getNodePropertyByAny()const override {return std::any();}
    public:
        const std::type_info& getReturnType()const noexcept override {return typeid(T);}
        std::any evaluateByAny(utility::EvaluationContext& evaluationContext)const final override {return evaluate(evaluationContext);}
    public:
        virtual ~TypedNodeInterface() = default;
        TypedNodeInterface() = default;
        TypedNodeInterface(const TypedNodeInterface&) = default;
        TypedNodeInterface(TypedNodeInterface&&) = default;
        TypedNodeInterface& operator=(const TypedNodeInterface&) = default;
        TypedNodeInterface& operator=(TypedNodeInterface&&) = default;
    };
}

#endif