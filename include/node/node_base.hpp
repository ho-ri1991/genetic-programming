#ifndef GP_NODE_NODE_BASE
#define GP_NODE_NODE_BASE

#include "typed_node_interface.hpp"
#include <tuple>
#include <memory>
#include <exception>
#include <cassert>
#include <type_traits>
#include <any>

namespace gp::node{
    template <typename T, typename ...Ts>
    const std::type_info& getRTTI(std::size_t n)noexcept {
        if constexpr (sizeof...(Ts) > 0) {
            if(n == 0)return typeid(T);
            else return getRTTI<Ts...>(n - 1);
        } else {
            if(n != 0) return typeid(void);
            else return typeid(T);
        }
    }

    template <size_t offset, typename ...Ts>
    std::shared_ptr<NodeInterface> getDynamicHelper(std::size_t n, std::tuple<Ts...>& t)noexcept {
        if constexpr(offset < sizeof...(Ts)){
            if(n == 0) return std::get<offset>(t);
            else return getDynamicHelper<offset + 1, Ts...>(n - 1, t);
        }else {
            return nullptr;
        }
    }

    template <typename ...Ts>
    std::shared_ptr<NodeInterface> getDynamic(std::size_t n, std::tuple<Ts...>& t)noexcept {
        return getDynamicHelper<0, Ts...>(n, t);
    }

    template <typename ...Ts>
    std::shared_ptr<NodeInterface> getDynamic(std::size_t n, const std::tuple<Ts...>& t)noexcept {
        return getDynamicHelper<0, Ts...>(n, const_cast<std::tuple<Ts...>&>(t));
    }

    template <std::size_t offset, typename ...Ts>
    void setDynamicHelper(std::size_t n, std::shared_ptr<NodeInterface> node, std::tuple<std::shared_ptr<TypedNodeInterface<Ts>>...>& t){
        if constexpr (offset < sizeof...(Ts)){
            if(n == 0){
                if(auto typed_node = std::dynamic_pointer_cast<typename std::tuple_element<offset, std::tuple<TypedNodeInterface<Ts>...>>::type>(node)){
                    std::get<offset>(t) = typed_node;
                }else{
                    throw std::invalid_argument("invalied type node was set as a child");
                }
            }else{
                setDynamicHelper<offset + 1, Ts...>(n - 1, node, t);
            }
        } else {
            throw std::invalid_argument("invalid child index");
        }
    };

    template <typename ...Ts>
    void setDynamic(std::size_t n, std::shared_ptr<NodeInterface> node, std::tuple<std::shared_ptr<TypedNodeInterface<Ts>>...>& t){
        setDynamicHelper<0, Ts...>(n, node, t);
    }

    template <typename T>
    class NodeBase;

    template <typename T, typename ...Args>
    class NodeBase<T(Args...)>: public TypedNodeInterface<T> {
    protected:
        std::weak_ptr<NodeInterface> parent;
        std::tuple<std::shared_ptr<TypedNodeInterface<Args>>...> children;
    public:
        std::size_t getChildNum()const override {return std::tuple_size<decltype(children)>::value;}
        const std::type_info& getChildReturnType(std::size_t n)const noexcept override {
            return getRTTI<Args...>(n);
        }
        std::shared_ptr<NodeInterface> getChildNode(std::size_t n)noexcept override {
            if(sizeof...(Args) <= n)return nullptr;
            return getDynamic(n, children);
        }
        std::shared_ptr<const NodeInterface> getChildNode(std::size_t n)const noexcept override {
            return getDynamic(n, children);
        }
        void setChild(std::size_t n, std::shared_ptr<NodeInterface> node)override {
            assert((n < sizeof...(Args)) && "the child index must be smaller than the number of children of the node");
            assert(node->getReturnType() == getRTTI<Args...>(n) && "the return type of child must equal to the argument type of the node");
            setDynamic(n, node, children);
        }
        std::shared_ptr<NodeInterface> getParent()noexcept override {return parent.lock();}
        std::shared_ptr<const NodeInterface> getParent()const noexcept override {return parent.lock();}
        void setParent(std::shared_ptr<NodeInterface> node)noexcept override { parent = node; }
    public:
        virtual ~NodeBase()noexcept {}
    };

    template<typename T>
    class NodeBase<T(void)>: public TypedNodeInterface<T> {
    private:
        std::weak_ptr<NodeInterface> parent;
    public:
        std::size_t getChildNum()const override {return 0;}
        const std::type_info& getChildReturnType(std::size_t)const noexcept override{return typeid(void);}
        std::shared_ptr<NodeInterface> getChildNode(std::size_t)noexcept override {return nullptr;}
        std::shared_ptr<const NodeInterface> getChildNode(std::size_t)const noexcept override {return nullptr;}
        void setChild(std::size_t, std::shared_ptr<NodeInterface>)override {
            assert("this node takes no child");
            throw std::invalid_argument("the child index must be smaller than the number of children of the node");
        }
        std::shared_ptr<NodeInterface> getParent()noexcept override {return parent.lock();}
        std::shared_ptr<const NodeInterface> getParent()const noexcept override {return parent.lock();}
        void setParent(std::shared_ptr<NodeInterface> node)noexcept override {parent = node;}
    public:
        virtual ~NodeBase(){}
    };
}

#endif