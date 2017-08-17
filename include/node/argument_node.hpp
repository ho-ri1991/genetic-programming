#ifndef GP_NODE_ARGUMENT_NODE
#define GP_NODE_ARGUMENT_NODE

#include "node_base.hpp"
#include <utility/type_name.hpp>

namespace gp::node {
    template <typename T, std::size_t n>
    class ArgumentNode: public NodeBase<T(void)> {
        using ThisType = ArgumentNode;
    private:
        T evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
            return evaluationContext.getArgument(n).get<T>();
        }
    public:
        std::string getNodeName()const override {
            return std::string("Argument<") + utility::typeName<T>() + std::string(",") + std::to_string(n) + std::string(">");
        }
        std::shared_ptr<NodeInterface> clone()const override {return std::make_shared<ThisType>();}
        NodeType getNodeType()const noexcept override final {return NodeType::Argument;}
    };
}

#endif