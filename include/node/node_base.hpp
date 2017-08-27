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
    NodeInterface& getDynamicHelper(std::size_t n, std::tuple<Ts...>& t) {
        if constexpr(offset < sizeof...(Ts)){
            if(n == 0) {
                if(std::get<offset>(t))return *std::get<offset>(t);
                else throw std::runtime_error("the specified child is nullptr");
            }
            else return getDynamicHelper<offset + 1, Ts...>(n - 1, t);
        }else {
            throw std::invalid_argument("index exceeds the number of children");
        }
    }

    template <typename ...Ts>
    NodeInterface& getDynamic(std::size_t n, std::tuple<Ts...>& t) {
        return getDynamicHelper<0, Ts...>(n, t);
    }

    template <typename ...Ts>
    const NodeInterface& getDynamic(std::size_t n, const std::tuple<Ts...>& t) {
        return getDynamicHelper<0, Ts...>(n, const_cast<std::tuple<Ts...>&>(t));
    }

    template <std::size_t offset, typename ...Ts>
    void setDynamicHelper(std::size_t n, std::unique_ptr<NodeInterface> node, std::tuple<std::unique_ptr<TypedNodeInterface<Ts>>...>& t){
        if constexpr (offset < sizeof...(Ts)){
            if(n == 0){
                if(auto typed_node = dynamic_cast<typename std::tuple_element<offset, std::tuple<TypedNodeInterface<Ts>...>>::type*>(node.get())){
                    std::get<offset>(t).reset(typed_node);
                    node.release();
                    return;
                }else{
                    throw std::invalid_argument("invalied type node was set as a child");
                }
            }else{
                setDynamicHelper<offset + 1, Ts...>(n - 1, std::move(node), t);
            }
        } else {
            throw std::invalid_argument("invalid child index");
        }
    };

    template <typename ...Ts>
    void setDynamic(std::size_t n, std::unique_ptr<NodeInterface> node, std::tuple<std::unique_ptr<TypedNodeInterface<Ts>>...>& t){
        setDynamicHelper<0, Ts...>(n, std::move(node), t);
    }

    template <std::size_t n, typename> struct EvaluateChildrenHelper;

    template <std::size_t n,
              template <typename ...> class Tpl,
              typename ...Ts>
    struct EvaluateChildrenHelper<n, Tpl<Ts...>> {
        static auto eval(const std::tuple<std::unique_ptr<TypedNodeInterface<Ts>>...>& children, utility::EvaluationContext& evaluationContext) {
            if constexpr (sizeof...(Ts) - 1 == n) {
                return std::make_tuple(std::get<n>(children)->evaluate(evaluationContext));
            } else {
                return std::tuple_cat(std::make_tuple(std::get<n>(children)->evaluate(evaluationContext)),
                                        EvaluateChildrenHelper<n + 1, Tpl<Ts...>>::eval(children, evaluationContext));
            }
        }
    };


    template <typename ...Ts>
    std::tuple<Ts...> evaluateChildren(const std::tuple<std::unique_ptr<TypedNodeInterface<Ts>>...>& children, utility::EvaluationContext& evaluationContext) {
        return EvaluateChildrenHelper<0, std::tuple<Ts...>>::eval(children, evaluationContext);
    }

    template <typename T>
    class NodeBase;

    template <typename T, typename ...Args>
    class NodeBase<T(Args...)>: public TypedNodeInterface<T> {
    protected:
        NodeInterface* parent;
        std::tuple<std::unique_ptr<TypedNodeInterface<Args>>...> children;
    protected:
        void setParent(NodeInterface* node)override {parent = node;}
    public:
        std::size_t getChildNum()const noexcept override {return std::tuple_size<decltype(children)>::value;}
        const std::type_info& getChildReturnType(std::size_t n)const noexcept override {
            return getRTTI<Args...>(n);
        }
        NodeInterface& getChildNode(std::size_t n)override {
            assert(n < sizeof...(Args));
            return getDynamic(n, children);
        }
        const NodeInterface& getChildNode(std::size_t n)const override {
            assert(n < sizeof...(Args));
            return getDynamic(n, children);
        }
        void setChild(std::size_t n, std::unique_ptr<NodeInterface> node)override {
            assert((n < sizeof...(Args)) && "the child index must be smaller than the number of children of the node");
            assert(node->getReturnType() == getRTTI<Args...>(n) && "the return type of child must equal to the argument type of the node");
            setDynamic(n, std::move(node), children);
            getChildNode(n).setParent(this);
        }
        NodeInterface& getParent()override {
            if(parent == nullptr) throw std::runtime_error("tried to get paren, but the parent is not set");
            return *parent;
        }
        const NodeInterface& getParent()const override {
            if(parent == nullptr) throw std::runtime_error("tried to get paren, but the parent is not set");
            return *parent;
        }
        virtual NodeType getNodeType()const noexcept override {return NodeType::Normal;}
        bool hasChild(std::size_t n)const noexcept override {
            return false;
        }
        bool hasParent()const noexcept override {return parent != nullptr;}
    public:
        NodeBase(): parent(nullptr){}
        virtual ~NodeBase()noexcept {}
    };

    template<typename T>
    class NodeBase<T(void)>: public TypedNodeInterface<T> {
    private:
        NodeInterface* parent;
    protected:
        void setParent(NodeInterface* node)override {parent = node;}
    public:
        std::size_t getChildNum()const noexcept override {return 0;}
        const std::type_info& getChildReturnType(std::size_t)const noexcept override{return typeid(void);}
        NodeInterface& getChildNode(std::size_t)override {throw std::invalid_argument("tried to get child, but this child takes no child");}
        const NodeInterface& getChildNode(std::size_t)const override {throw std::invalid_argument("tried to get child, but this child takes no child");}
        void setChild(std::size_t, std::unique_ptr<NodeInterface>)override {
            assert("this node takes no child");
            throw std::invalid_argument("the child index must be smaller than the number of children of the node");
        }
        NodeInterface& getParent()override {
            if(parent == nullptr) throw std::runtime_error("tried to get paren, but the parent is not set");
            return *parent;
        }
        const NodeInterface& getParent()const override {
            if(parent == nullptr) throw std::runtime_error("tried to get paren, but the parent is not set");
            return *parent;
        }
        virtual NodeType getNodeType()const noexcept override {return NodeType::Normal;}
        bool hasChild(std::size_t)const noexcept override {return false;}
        bool hasParent()const noexcept override {return parent != nullptr;}
    public:
        NodeBase(): parent(nullptr){}
        virtual ~NodeBase(){}
    };
}

#endif