#ifndef GP_NODE_SUBROUTINE_NODE
#define GP_NODE_SUBROUTINE_NODE

#include "node_base.hpp"
#include <tuple>
#include <unordered_map>

namespace gp::node {
    class SubroutineEntitySet {
    private:
        using node_instance_type = NodeInterface::const_node_instance_type;
        using LocalVariableTypes = std::vector<const NodeInterface::type_info*>;
        using SubroutineEntity = std::pair<node_instance_type, const LocalVariableTypes>;
        std::unordered_map<std::string, SubroutineEntity> subroutineEntities;
    public:
        template <typename Key>
        decltype(auto) find(Key&& key) {return subroutineEntities.find(std::forward<Key>(key));}
        template <typename Key>
        decltype(auto) find(Key&& key)const {return subroutineEntities.find(std::forward<Key>(key));}
        template <typename Key>
        void insert(Key&& key, SubroutineEntity&& val) {subroutineEntities.insert(std::make_pair(std::forward<Key>(key), std::move(val)));}
        template <typename Key>
        void deleteEntity(Key&& key)noexcept {
            auto itr = subroutineEntities.find(std::forward<Key>(key));
            if(itr != std::end(subroutineEntities)){
                subroutineEntities.erase(itr);
            }
        }
        decltype(auto) begin(){return std::begin(subroutineEntities);}
        decltype(auto) begin()const{return std::begin(subroutineEntities);}
        decltype(auto) end(){return std::end(subroutineEntities);}
        decltype(auto) end()const{return std::end(subroutineEntities);}
    };

    template <typename>
    class SubroutineNode;

    template <typename T, typename ...Args>
    class SubroutineNode<T(Args...)>: public NodeBase<T(Args...)> {
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
        T evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            auto childReturnVal = evaluateChildren(this->children, evaluationContext);
            if(evaluationContext.getEvaluationStatus() != utility::EvaluationStatus::Evaluating)return utility::getDefaultValue<T>();
            auto itr = subroutineEntitySet.find(name);
            if(itr == std::end(subroutineEntitySet))throw std::runtime_error("subroutine entity not found");
            auto& [entity, localVariebleTypes] = itr->second;
            auto subroutineEvaluationContext = utility::EvaluationContext(childReturnVal,
                                                                          std::vector<utility::Variable>(std::size(localVariebleTypes)),
                                                                          tree::defaultMaxEvaluationCount,
                                                                          tree::defaultMaxStackCount);

            auto ans = entity->evaluateByAny(subroutineEvaluationContext);
            if(subroutineEvaluationContext.getEvaluationStatus() == utility::EvaluationStatus::ValueReturned){
                return std::any_cast<T>(subroutineEvaluationContext.getReturnValue());
            }else if(subroutineEvaluationContext.getEvaluationStatus() == utility::EvaluationStatus::Evaluating){
                return std::any_cast<T>(ans);
            }else {
                evaluationContext.setEvaluationStatusWithoutUpdate(subroutineEvaluationContext.getEvaluationStatus());
                return utility::getDefaultValue<T>();
            }
        }
    public:
        std::string getNodeName()const override {return name;}
        node_instance_type clone()const override {return NodeInterface::createInstance<ThisType>(name, subroutineEntitySet);}
        NodeType getNodeType()const noexcept override final {return NodeType::Subroutine;}
    };
}

#endif
