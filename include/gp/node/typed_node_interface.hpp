#ifndef GP_NODE_TYPED_NODE_INTERFACE
#define GP_NODE_TYPED_NODE_INTERFACE

#include "node_interface.hpp"
#include <gp/utility/evaluation_context.hpp>
#include <gp/utility/default_initializer.hpp>
#include <cassert>

namespace gp::node{
    template<typename T>
    class TypedNodeInterface: public NodeInterface {
    public:
        using type_info = NodeInterface::type_info;
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
        const type_info& getReturnType()const noexcept override {return utility::typeInfo<T>();}
        std::any evaluateByAny(utility::EvaluationContext& evaluationContext)const final override {return evaluate(evaluationContext);}
    public:
        virtual ~TypedNodeInterface() = default;
        TypedNodeInterface() = default;
        TypedNodeInterface(const TypedNodeInterface&) = default;
        TypedNodeInterface(TypedNodeInterface&&) = default;
        TypedNodeInterface& operator=(const TypedNodeInterface&) = default;
        TypedNodeInterface& operator=(TypedNodeInterface&&) = default;
    };

    template <typename T>
    void swapTypedNodeInterfaceAndNodeInterface(NodeInterface::typed_node_instance_type<T>& typedNodePtr,
                                                NodeInterface::node_instance_type& nodeInterfacePtr) {

        if(nodeInterfacePtr) {
            assert(nodeInterfacePtr->getReturnType() == utility::typeInfo<T>());
            auto newTypedNodePtr = static_cast<TypedNodeInterface<T>*>(nodeInterfacePtr.release());
            NodeInterface* newNodeInterfacePtr = typedNodePtr.release();
            typedNodePtr.reset(newTypedNodePtr);
            nodeInterfacePtr.reset(newNodeInterfacePtr);
        } else {
            nodeInterfacePtr.reset(typedNodePtr.release());
        }
    }
}

#endif
