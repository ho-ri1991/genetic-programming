#ifndef TREE_NODE_TYPED_NODE_INTERFACE
#define TREE_NODE_TYPED_NODE_INTERFACE

#include "node_interface.hpp"

namespace tree::node{
    template<typename T>
    class TypedNodeInterface: public NodeInterface {
    public:
        virtual T evaluate() = 0;
    public:
        const std::type_info * const getReturnType()const noexcept override {return &typeid(T);}
    public:
        virtual ~TypedNodeInterface(){}
    };
}

#endif