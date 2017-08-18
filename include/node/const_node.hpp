#ifndef GP_NODE_CONST_NODE
#define GP_NODE_CONST_NODE

#include "node_base.hpp"
#include <sstream>

namespace gp::node {
    template <typename T>
    class ConstNode: public NodeBase<T(void)> {
    private:
        using ThisType = ConstNode;
        T data;
    private:
        T evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            return data;
        }
    public:
        std::string getNodeName()const override {
            return std::string("Const<") + utility::typeName<T>() + std::string(">");
        }
        std::shared_ptr<NodeInterface> clone()const override {return std::make_shared<ThisType>(data);}
        NodeType getNodeType()const noexcept override final {return NodeType::Const;}
        void setNodePropertyByString(const std::string& prop) override {
            std::stringstream sstream;
            sstream << prop;
            sstream >> data;
        }
        std::string getNodePropertyByString()const override {
            std::stringstream sstream;
            sstream << data;
            return sstream.str();
        }
        void setNodePropertyByAny(const std::any& prop) override {
            assert(prop.type() == typeid(T));
            data = std::any_cast<T>(prop);
        }
        std::any getNodePropertyByAny()const override {return data;}
    public:
        ConstNode(T&& val): data(std::move(val)){}
        ConstNode(const T& val): data(val){}
        ConstNode() = default;
        ConstNode(const ConstNode&) = default;
        ConstNode(ConstNode&&) = default;
        ConstNode& operator=(const ConstNode&) = default;
        ConstNode& operator=(ConstNode&&) = default;
        ~ConstNode() = default;
    };
}

#endif