#ifndef GP_TREE_TREE
#define GP_TREE_TREE

#include <gp/utility/evaluation_context.hpp>
#include <gp/node/node_interface.hpp>
#include <exception>
#include <gp/gp_config.hpp>

namespace gp::tree {
    struct TreeProperty {
        using type_info = node::NodeInterface::type_info;
        using container_type = std::vector<const type_info *>;
        const type_info *returnType;
        container_type argumentTypes;
        container_type localVariableTypes;
        std::string name;
    };
}

namespace std {
    template <>
    void swap<gp::tree::TreeProperty>(gp::tree::TreeProperty& treeProperty1, gp::tree::TreeProperty& treeProperty2) noexcept {
        using std::swap;
        swap(treeProperty1.returnType, treeProperty2.returnType);
        swap(treeProperty1.argumentTypes, treeProperty2.argumentTypes);
        swap(treeProperty1.localVariableTypes, treeProperty2.localVariableTypes);
        swap(treeProperty1.name, treeProperty2.name);
    }
}

namespace gp::tree{
    class Tree {
    public:
        using NodeType = node::NodeInterface;
        using type_info = node::NodeInterface::type_info;
        using node_instance_type = node::NodeInterface::node_instance_type;
    private:
        using EvaluationCount = utility::EvaluationContext::EvaluationCount;
        using StackCount = utility::EvaluationContext::StackCount;
    private:
        std::unique_ptr<node::NodeInterface> rootNode;
        TreeProperty treeProperty;
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
        template <typename TreeProperty_, typename = std::enable_if_t<std::is_same_v<TreeProperty, std::decay_t<TreeProperty_>>>>
        Tree(TreeProperty_&& treeProperty_, node_instance_type rootNode_):
                treeProperty(std::forward<TreeProperty_>(treeProperty_)),
                rootNode(std::move(rootNode_)){}
        virtual ~Tree() = default;
        Tree(const Tree& other):
                rootNode(copyTreeStructure(*other.rootNode)),
                treeProperty(other.treeProperty){}
        Tree(Tree&&) = default;
        Tree& operator=(const Tree& rhs) {
            Tree tmp(rhs);
            this->swap(tmp);
            return *this;
        }
        Tree& operator=(Tree&&) = default;
    public:
        node_instance_type getRootNodeInstance()&& noexcept{return std::move(rootNode);}
        TreeProperty& getTreeProperty()& noexcept {return treeProperty;}
        const TreeProperty& getTreeProperty()const& noexcept {return treeProperty;}
        TreeProperty&& getTreeProperty()&& noexcept { return std::move(treeProperty);}

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
        auto getArgumentNum()const noexcept {return std::size(treeProperty.argumentTypes);}
        auto getLocalVariableNum()const noexcept {return std::size(treeProperty.localVariableTypes);}
        const type_info& getArgumentType(std::size_t n)const noexcept {
            if(std::size(treeProperty.argumentTypes) <= n || treeProperty.argumentTypes[n] == nullptr)return utility::typeInfo<utility::error>();
            else return *treeProperty.argumentTypes[n];
        }
        const type_info& getLocalVariableType(std::size_t n)const noexcept {
            if(std::size(treeProperty.localVariableTypes) <= n || treeProperty.localVariableTypes[n] == nullptr)return utility::typeInfo<utility::error>();
            else return *treeProperty.localVariableTypes[n];
        }
        std::string getName()const {return treeProperty.name;}
        void swap(Tree& other)noexcept {
            using std::swap;
            swap(this->rootNode, other.rootNode);
            swap(this->treeProperty, other.treeProperty);
        }
    private:
        bool isValidArguments(const std::vector<std::any>& arguments);
    public:
        template<typename Arguments>
        utility::EvaluationContext evaluate(Arguments&& arguments,
                                            EvaluationCount maxEvaluationCount = defaultMaxEvaluationCount,
                                            StackCount maxStackCount = defaultMaxStackCount)const {

            utility::EvaluationContext evaluationContext(std::forward<Arguments>(arguments),
                                                         utility::EvaluationContext::VariableTable(std::size(treeProperty.localVariableTypes)),
                                                         maxEvaluationCount,
                                                         maxStackCount);

            auto ans = rootNode->evaluateByAny(evaluationContext);
            if(evaluationContext.getEvaluationStatus() == utility::EvaluationStatus::Evaluating){
                evaluationContext.setReturnValue(std::move(ans));
            }
            return evaluationContext;
        }
    public:
        void pushBackLocalVariableType(const type_info& type) {treeProperty.localVariableTypes.push_back(&type);}
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
