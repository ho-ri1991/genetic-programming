#ifndef GP_NODE_TYPED_NODE_INTERFACE
#define GP_NODE_TYPED_NODE_INTERFACE

#include "node_interface.hpp"
#include <utility/evaluation_context.hpp>

namespace gp::node{
    template<typename T>
    class TypedNodeInterface: public NodeInterface {
    public:
        virtual T evaluate(utility::EvaluationContext&) = 0;
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