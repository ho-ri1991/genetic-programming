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
            std::stringstream sstream;
            sstream <<',' << data;
            return std::string("Const<") + utility::typeName<T>() + sstream.str() + std::string(">");
        }
        std::unique_ptr<NodeInterface> clone()const override {return std::make_unique<ThisType>(data);}
        NodeType getNodeType()const noexcept override final {return NodeType::Const;}
        void setNodePropertyByNodeName(const std::string& name) override {
            auto beg = name.find('<');
            auto end = name.find('>');
            if(beg == std::string::npos || end == std::string::npos || end < beg) throw std::invalid_argument("invalid node name of const node");
            auto sep = name.find(',', beg);
            if(sep == std::string::npos || end < sep) throw std::invalid_argument("invalid node name of const node");
            std::stringstream sstream;
            sstream << name.substr(sep + 1, end - sep - 1);
            sstream >> data;
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