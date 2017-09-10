#ifndef GP_TREE_TREE
#define GP_TREE_TREE

#include <gp/utility/evaluation_context.hpp>
#include <gp/node/node_interface.hpp>
#include <exception>
#include <gp/gp_config.hpp>

namespace gp::tree {
    struct TreeProperty {
        using type_info = node::NodeInterface::type_info;
        using container_type = std::vector<const type_info*>;
        container_type argumentTypes;
        container_type localVariableTypes;
        const type_info* returnType;
        std::string name;
    };

    class Tree {
    public:
        using NodeType = node::NodeInterface;
        using ArgumentTypes = std::vector<const std::type_info*>;
        using LocalVariableTypes = std::vector<const std::type_info*>;
    private:
        using EvaluationCount = utility::EvaluationContext::EvaluationCount;
        using StackCount = utility::EvaluationContext::StackCount;
    private:
        std::unique_ptr<node::NodeInterface> rootNode;
        const std::type_info* returnType;
        ArgumentTypes argumentTypes;
        LocalVariableTypes localVariableTypes;
    private:
        static std::unique_ptr<node::NodeInterface> copyTreeStructure(const node::NodeInterface& rootNode) {
            std::unique_ptr<node::NodeInterface> targetRootNode = rootNode.clone();
            for(int i = 0; i < rootNode.getChildNum(); ++i) {
                if(!rootNode.hasChild(i)) throw std::runtime_error("the copy node must not be null");
                targetRootNode->setChild(i, copyTreeStructure(rootNode.getChildNode(i)));
            }
            return targetRootNode;
        }
    public:
        template <typename ArgumentTypes_, typename LocalVariableTypes_>
        Tree(const std::type_info& returnType_,
             ArgumentTypes_&& argumentTypes_,
             LocalVariableTypes_&& localVariableTypes_,
             std::unique_ptr<node::NodeInterface> rootNode_):
                rootNode(std::move(rootNode_)),
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
        node::NodeInterface& getRootNode(){
            assert(rootNode);
            if(!rootNode) throw std::runtime_error("the root node of tree is nullptr");
            else return *rootNode;
        }
        const node::NodeInterface& getRootNode()const{
            assert(rootNode);
            if(!rootNode) throw std::runtime_error("the root node of tree is nullptr");
            else return *rootNode;
        }
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
