#ifndef GP_NODE_SUBROUTINE_NODE
#define GP_NODE_SUBROUTINE_NODE

#include "node_base.hpp"
#include <tuple>
#include <unordered_map>

namespace gp::node {
    class SubroutineEntitySet {
    private:
        using LocalVariableTypes = std::vector<const std::type_info*>;
        using SubroutineEntity = std::pair<std::shared_ptr<const NodeInterface>, const LocalVariableTypes>;
        std::unordered_map<std::string, SubroutineEntity> subroutineEntities;
    public:
        template <typename Key>
        decltype(auto) find(Key&& key) {return subroutineEntities.find(std::forward<Key>(key));}
        template <typename Key>
        decltype(auto) find(Key&& key)const {return subroutineEntities.find(std::forward<Key>(key));}
        template <typename Key, typename Val>
        void insert(Key&& key, Val&& val) {subroutineEntities.insert(std::forward<Key>(key), std::forward<Val>(val));}
    };

    template <typename T, typename ...Args>
    class SubroutineNode: public NodeBase<T(Args...)> {
    private:
        using ThisType = SubroutineNode;
        using node_instance_type = NodeInterface::node_instance_type;
        const SubroutineEntitySet& subroutineEntitySet;
        const std::string name;
    public:
        template <typename Name>
        SubroutineNode(Name&& nodeName, const SubroutineEntitySet& subroutineEntitySet_)
                : subroutineEntitySet(subroutineEntitySet_)
                , name(std::forward<Name>(nodeName)){}
    private:
        T evaluationDefinition(utility::EvaluationContext* evaluationContext)const override;
    public:
        std::string getNodeName()const override {return name;}
        node_instance_type clone()const override {return NodeInterface::createInstance<ThisType>(name, subroutineEntitySet);}
        NodeType getNodeType()const noexcept override final {return NodeType::Subroutine;}
    };
}

#endif