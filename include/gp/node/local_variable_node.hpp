#ifndef GP_NODE_LOCAL_VARIABLE_NODE
#define GP_NODE_LOCAL_VARIABLE_NODE

#include "node_base.hpp"
#include <gp/utility/left_hand_value.hpp>
#include <gp/utility/reference.hpp>
#include <boost/lexical_cast.hpp>

namespace gp::node {
    namespace local_variable_node {
        constexpr const char *nameHeader = "LocalVariable[";
        constexpr char nameDelimiter = ']';
        constexpr char propertySeparator = ',';
    }

    template <typename T>
    class LocalVariableNode: public NodeBase<T(void)> {
        using ThisType = LocalVariableNode;
        using node_instance_type = NodeInterface::node_instance_type;
        using variable_index_type = node::NodeInterface::variable_index_type;
    private:
        variable_index_type n;
    private:
        T evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            auto& localVariable = evaluationContext.getLocalVariable(n);
            if(!localVariable.hasValue()) {
                localVariable.set(utility::getDefaultValue<T>());
            }
            return localVariable.template get<T>();
        }

    public:
        std::string getNodeName()const override {
            return std::string(local_variable_node::nameHeader)
                   + utility::typeInfo<T>().name()
                   + local_variable_node::propertySeparator
                   + std::to_string(n)
                   + local_variable_node::nameDelimiter;
        }
        node_instance_type clone()const override {return NodeInterface::createInstance<ThisType>(n);}
        NodeType getNodeType()const noexcept override final {return NodeType::LocalVariable;}
        std::any getNodePropertyByAny()const override final {return n;}
        void setNodePropertyByNodeName(const std::string& name) override {
            auto beg = name.find('[');
            auto end = name.rfind(']');
            if(beg == std::string::npos || end == std::string::npos || end < beg) throw std::invalid_argument("invalid node name of local variable node");
            auto sep = name.find(',', beg);
            if(sep == std::string::npos || end < sep) throw std::invalid_argument("invalid node name of local variable node");
            n = boost::lexical_cast<variable_index_type>(name.substr(sep + 1, end - sep - 1));
        }
        void setNodePropertyByAny(const std::any& prop) override {
            assert(prop.type() == typeid(variable_index_type));
            n = std::any_cast<variable_index_type>(prop);
        }
    public:
        LocalVariableNode():n(0){}
        LocalVariableNode(variable_index_type n_):n(n_){}
    };

    template <typename T>
    class LocalVariableNode<utility::LeftHandValue<T>>: public NodeBase<utility::LeftHandValue<T>(void)> {
        using ThisType = LocalVariableNode;
        using ReturnType = utility::LeftHandValue<T>;
        using node_instance_type = NodeInterface::node_instance_type;
        using variable_index_type = node::NodeInterface::variable_index_type;
    private:
        variable_index_type n;
    private:
        ReturnType evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            auto& localVariable = evaluationContext.getLocalVariable(n);
            if(!localVariable.hasValue()) {
                localVariable.set(utility::getDefaultValue<T>());
            }
            return ReturnType(localVariable);
        }
    public:
        std::string getNodeName()const override {
            return std::string(local_variable_node::nameHeader)
                   + utility::typeInfo<ReturnType>().name()
                   + local_variable_node::propertySeparator
                   + std::to_string(n)
                   + local_variable_node::nameDelimiter;
        }
        node_instance_type clone()const override {return NodeInterface::createInstance<ThisType>(n);}
        NodeType getNodeType()const noexcept override final {return NodeType::LocalVariable;}
        std::any getNodePropertyByAny()const override final {return n;}
        void setNodePropertyByNodeName(const std::string& name) override {
            auto beg = name.find('[');
            auto end = name.rfind(']');
            if(beg == std::string::npos || end == std::string::npos || end < beg) throw std::invalid_argument("invalid node name of local variable node");
            auto sep = name.find(',', beg);
            if(sep == std::string::npos || end < sep) throw std::invalid_argument("invalid node name of local variable node");
            n = boost::lexical_cast<variable_index_type>(name.substr(sep + 1, end - sep - 1));
        }
        void setNodePropertyByAny(const std::any& prop) override {
            assert(prop.type() == typeid(variable_index_type));
            n = std::any_cast<variable_index_type>(prop);
        }
    public:
        LocalVariableNode():n(0){}
        LocalVariableNode(variable_index_type n_):n(n_){}
    };

    template <typename T>
    class LocalVariableNode<utility::Reference<T>>: public NodeBase<utility::Reference<T>(void)> {
    private:
        using ThisType = LocalVariableNode;
        using ReturnType = utility::Reference<T>;
        using node_instance_type = NodeInterface::node_instance_type;
        using variable_index_type = node::NodeInterface::variable_index_type;
    private:
        variable_index_type n;
    private:
        ReturnType evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            auto& localVariable = evaluationContext.getLocalVariable(n);
            if(!localVariable.hasValue()) {
                localVariable.set(utility::getDefaultValue<T>());
            }
            return ReturnType(localVariable);
        }
    public:
        std::string getNodeName()const override {
            return std::string(local_variable_node::nameHeader)
                   + utility::typeInfo<ReturnType>().name()
                   + local_variable_node::propertySeparator
                   + std::to_string(n)
                   + local_variable_node::nameDelimiter;
        }
        node_instance_type clone()const override {return NodeInterface::createInstance<ThisType>(n);}
        NodeType getNodeType()const noexcept override final {return NodeType::LocalVariable;}
        std::any getNodePropertyByAny()const override final {return n;}
        void setNodePropertyByNodeName(const std::string& name) override {
            auto beg = name.find('[');
            auto end = name.rfind(']');
            if(beg == std::string::npos || end == std::string::npos || end < beg) throw std::invalid_argument("invalid node name of local variable node");
            auto sep = name.find(',', beg);
            if(sep == std::string::npos || end < sep) throw std::invalid_argument("invalid node name of local variable node");
            n = boost::lexical_cast<variable_index_type>(name.substr(sep + 1, end - sep - 1));
        }
        void setNodePropertyByAny(const std::any& prop) override {
            assert(prop.type() == typeid(variable_index_type));
            n = std::any_cast<variable_index_type>(prop);
        }
    public:
        LocalVariableNode():n(0){}
        LocalVariableNode(variable_index_type n_):n(n_){}
    };
}

#endif
