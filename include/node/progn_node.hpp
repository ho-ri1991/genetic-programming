#ifndef GP_NODE_PROGN_NODE
#define GP_NODE_PROGN_NODE

#include "typed_node_interface.hpp"
#include <exception>
#include <array>

namespace gp::node {
    struct arbitrary{};

    template <typename T, std::size_t n>
    class PrognNode: public TypedNodeInterface<T> {
        static_assert(1 < n, "progn node whose child num is smaller than 2 is not supported");
    private:
        std::weak_ptr<NodeInterface> parent;
        std::array<std::shared_ptr<NodeInterface>, n - 1> children;
        std::shared_ptr<TypedNodeInterface<T>> lastChild;
    private:
        T evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            for(const auto& node: children)
                node->evaluateByAny(evaluationContext);
            return lastChild->evaluate(evaluationContext);
        }
    public:
        std::size_t getChildNum()const noexcept override {return n;}
        const std::type_info& getChildReturnType(std::size_t m)const noexcept override {
            if(n <= m)return typeid(void);
            else if(m == n - 1)return typeid(T);
            else return typeid(arbitrary);
        }
        std::shared_ptr<NodeInterface> getChildNode(std::size_t m)noexcept override {
            if(n <= m)return nullptr;
            else if(m == n - 1)return lastChild;
            else return children[m];
        }
        std::shared_ptr<const NodeInterface> getChildNode(std::size_t m)const noexcept override {
            if(n <= m)return nullptr;
            else if(m == n - 1)return lastChild;
            else return children[m];
        }
        void setChild(std::size_t m, std::shared_ptr<NodeInterface> node)override {
            assert((m < n) && "the child index must be smaller than the number of children of the node");
            if(n <= m) throw std::invalid_argument("invalid child index");
            if(m == n - 1){
                assert(typeid(T) == node->getReturnType());
                if(typeid(T) != node->getReturnType()) throw std::invalid_argument("invalied type node was set as a child");
                lastChild = std::dynamic_pointer_cast<TypedNodeInterface<T>>(node);
            }else {
                children[m] = node;
            }
        }
        std::shared_ptr<NodeInterface> getParent()noexcept override {return parent.lock();}
        std::shared_ptr<const NodeInterface> getParent()const noexcept override {return parent.lock();}
        void setParent(std::shared_ptr<NodeInterface> node)noexcept override {parent = node;}
        NodeType getNodeType()const noexcept override final {return NodeType::Progn;}
        std::string getNodeName()const override {
            return std::string("Progn<") + utility::typeName<T>() + std::string(",") + std::to_string(n) + std::string(">");
        }
        std::shared_ptr<NodeInterface> clone()const override {return std::make_shared<PrognNode>();}
        std::string getNodePropertyByString()const override {return std::to_string(n);}
        std::any getNodePropertyByAny()const override {return n;}
    };
}

#endif