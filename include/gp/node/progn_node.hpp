#ifndef GP_NODE_PROGN_NODE
#define GP_NODE_PROGN_NODE

#include "typed_node_interface.hpp"
#include <exception>
#include <array>

namespace gp::node {
    struct arbitrary{};

    template <typename T, std::size_t n>
    class PrognNode: public TypedNodeInterface<T> {
    public:
        using type_info = NodeInterface::type_info;
        using node_instance_type = NodeInterface::node_instance_type;
        static_assert(1 < n, "progn node whose child num is smaller than 2 is not supported");
    private:
        NodeInterface* parent;
        std::array<node_instance_type, n - 1> children;
        NodeInterface::typed_node_instance_type<T> lastChild;
    protected:
        void setParent(NodeInterface* node)override {parent = node;}
    private:
        T evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            for(const auto& node: children)
                node->evaluateByAny(evaluationContext);
            return lastChild->evaluate(evaluationContext);
        }
    public:
        std::size_t getChildNum()const noexcept override {return n;}
        const type_info& getChildReturnType(std::size_t m)const noexcept override {
            if(n <= m)return utility::typeInfo<utility::error>();
            else if(m == n - 1)return utility::typeInfo<T>();
            else return utility::typeInfo<utility::any>();
        }
        NodeInterface& getChild(std::size_t m)override {
            if(n <= m)throw std::invalid_argument("the index exceeded the number of children");
            else if(m == n - 1){
                if(!lastChild)throw std::runtime_error("the specified child is nullptr");
                return *lastChild;
            }
            else {
                if(!children[m])throw std::runtime_error("the specified child is nullptr");
                return *children[m];
            }
        }
        const NodeInterface& getChild(std::size_t m)const override {
            if(n <= m)throw std::invalid_argument("the index exceeded the number of children");
            else if(m == n - 1){
                if(!lastChild)throw std::runtime_error("the specified child is nullptr");
                return *lastChild;
            }
            else {
                if(!children[m])throw std::runtime_error("the specified child is nullptr");
                return *children[m];
            }
        }
        node_instance_type setChild(std::size_t m, node_instance_type node)override {
            assert((m < n) && "the child index must be smaller than the number of children of the node");
            if(n <= m) throw std::invalid_argument("invalid child index");
            if(m == n - 1){
                if(!node) return node::NodeInterface::node_instance_type(lastChild.release());
                assert(node->getReturnType() == utility::typeInfo<T>());
                if(utility::typeInfo<T>() != node->getReturnType()) throw std::invalid_argument("invalied type node was set as a child");
                auto typed_ptr = static_cast<TypedNodeInterface<T>*>(node.get());
                node.release();
                auto org = lastChild.release();
                lastChild.reset(typed_ptr);
                if(lastChild)lastChild->setParent(this);
                return node::NodeInterface::node_instance_type(org);
            }else {
                children[m].swap(node);
                if(children[m])children[m]->setParent(this);
                return node;
            }
        }
        NodeInterface& getParent()override {
            if(parent == nullptr)throw std::runtime_error("the parent is null");
            return *parent;
        }
        const NodeInterface& getParent()const override {
            if(parent == nullptr)throw std::runtime_error("the parent is null");
            return *parent;
        }
        NodeType getNodeType()const noexcept override final {return NodeType::Progn;}
        std::string getNodeName()const override {
            return std::string("Progn[") + utility::typeInfo<T>().name() + std::string(",") + std::to_string(n) + std::string("]");
        }
        node_instance_type clone()const override {return NodeInterface::createInstance<PrognNode>();}
        std::any getNodePropertyByAny()const override {return n;}
        bool hasChild(std::size_t m)const noexcept override {
            if(n <= m)return false;
            else if(n - 1 == m)return static_cast<bool>(lastChild);
            else return static_cast<bool>(children[m]);
        }
        bool hasParent()const noexcept override {return parent != nullptr;}
    public:
        PrognNode(): parent(nullptr){}
    };
}

#endif
