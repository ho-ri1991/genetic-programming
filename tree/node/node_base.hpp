#ifndef TREE_NODE_NODE_BASE
#define TREE_NODE_NODE_BASE

#include "typed_node_interface.hpp"
#include <tuple>
#include <memory>
#include <exception>
#include <cassert>
#include <type_traits>

namespace tree::node{
    template <typename T, typename ...Ts>
    const std::type_info* const getRTTI(std::size_t n)noexcept {
        if constexpr (sizeof...(Ts) > 0) {
            if(n == 0)return &typeid(T);
            else return getRTTI<Ts...>(n - 1);
        } else {
            if(n != 0) return nullptr;
            else return &typeid(T);
        }
    }

    template <size_t offset, typename ...Ts>
    std::shared_ptr<NodeInterface> getDynamic(std::size_t n, std::tuple<Ts...>& t){
        if constexpr(offset < sizeof...(Ts)){
            if(n == 0) return std::get<offset>(t);
            else return getDynamic<offset + 1, Ts...>(n - 1, t);
        }else {
            return nullptr;
        }
    }

    template <typename T>
    class NodeBase;

    template <typename T, typename ...Args>
    class NodeBase<T(Args...)>: public TypedNodeInterface<T> {
    private:
        std::weak_ptr<NodeInterface> parent;
        std::tuple<std::shared_ptr<Args>...> children;
    public:
        const std::type_info* const getChildReturnType(std::size_t n)const override {
            return getRTTI<Args...>(n);
        }
        std::shared_ptr<NodeInterface> getChildNode(std::size_t n)override {
            if(sizeof...(Args) <= n)return nullptr;
            return getFromTupleDynamic<0, Args...>(n, children);
        }
    public:
        virtual ~NodeBase(){}
    };
}

#endif