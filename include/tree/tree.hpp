#ifndef GP_TREE_TREE
#define GP_TREE_TREE

#include <utility/evaluation_context.hpp>
#include <node/node_interface.hpp>
#include <exception>
#include "tree_operations.hpp"

namespace gp::tree {
    class Tree {
    public:
        using ArgumentTypes = std::vector<const std::type_info*>;
        using LocalVariableTypes = std::vector<const std::type_info*>;
    private:
        using EvaluationCount = utility::EvaluationContext::EvaluationCount;
        using StackCount = utility::EvaluationContext::StackCount;
        static constexpr EvaluationCount defaultMaxEvaluationCount = 10000;
        static constexpr StackCount defaultMaxStackCount = 10000;
    private:
        std::shared_ptr<node::NodeInterface> rootNode;
        const std::type_info* returnType;
        ArgumentTypes argumentTypes;
        LocalVariableTypes localVariableTypes;
    public:
        template <typename ArgumentTypes_, typename LocalVariableTypes_>
        Tree(const std::type_info& returnType_,
             ArgumentTypes_&& argumentTypes_,
             LocalVariableTypes_&& localVariableTypes_,
             std::shared_ptr<node::NodeInterface> rootNode_):
                rootNode(rootNode_),
                returnType(&returnType_),
                argumentTypes(std::forward<ArgumentTypes_>(argumentTypes_)),
                localVariableTypes(std::forward<LocalVariableTypes_>(localVariableTypes_)){

            for(const auto ptr: argumentTypes) {
                if(ptr == nullptr) throw std::invalid_argument("the argument type must not be null");
            }

            for(const auto ptr: localVariableTypes) {
                if(ptr == nullptr) throw std::invalid_argument("the local variable type must not be null");
            }
        }
        virtual ~Tree() = default;
        Tree(const Tree& other): rootNode(copyTreeStructure(*other.rootNode))
                         , returnType(other.returnType)
                         , argumentTypes(other.argumentTypes)
                         , localVariableTypes(other.localVariableTypes) {}
        Tree(Tree&&) = default;
        Tree& operator=(const Tree& rhs) {
            Tree tmp(rhs);
            this->swap(tmp);
            return *this;
        }
        Tree& operator=(Tree&&) = default;
    public:
        auto getArgumentNum()const noexcept {return std::size(argumentTypes);}
        auto getLocalVariableNum()const noexcept {return std::size(localVariableTypes);}
        const std::type_info& getArgumentType(std::size_t n)const noexcept {
            if(std::size(argumentTypes) <= n || argumentTypes[n] == nullptr)return typeid(void);
            else return *argumentTypes[n];
        }
        const std::type_info& getLocalVariableType(std::size_t n)const noexcept {
            if(std::size(localVariableTypes) <= n || localVariableTypes[n] == nullptr)return typeid(void);
            else return *localVariableTypes[n];
        }
        void swap(Tree& other)noexcept {
            using std::swap;
            swap(this->rootNode, other.rootNode);
            swap(this->returnType, other.returnType);
            swap(this->argumentTypes, other.argumentTypes);
            swap(this->localVariableTypes, other.localVariableTypes);
        }
    private:
        bool isValidArguments(const std::vector<std::any>& arguments);
    public:
        template<typename Arguments>
        utility::EvaluationContext evaluate(Arguments&& arguments,
                                            EvaluationCount maxEvaluationCount = defaultMaxEvaluationCount,
                                            StackCount maxStackCount = defaultMaxStackCount)const {

            utility::EvaluationContext evaluationContext(std::forward<Arguments>(arguments),
                                                         utility::EvaluationContext::VariableTable(std::size(localVariableTypes)),
                                                         maxEvaluationCount,
                                                         maxStackCount);

            auto ans = rootNode->evaluateByAny(evaluationContext);
            if(evaluationContext.getEvaluationStatus() == utility::EvaluationStatus::Evaluating){
                evaluationContext.setReturnValue(std::move(ans));
            }
            return evaluationContext;
        }
    public:
        void pushBackLocalVariableType(const std::type_info& type) {localVariableTypes.push_back(&type);}
        void eraseRedundantLocalVariableType();
    };
}

namespace std {
    template <>
    void swap<gp::tree::Tree>(gp::tree::Tree& tree1, gp::tree::Tree& tree2) noexcept {
        tree1.swap(tree2);
    }
}

#endif