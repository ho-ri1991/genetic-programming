#ifndef GP_TREE_TREE
#define GP_TREE_TREE

#include <node/node_interface.hpp>

namespace gp::tree {
    class Tree {
    public:
        using ArgumentTypes = std::vector<const std::type_info* const>;
        using LocalVariableTypes = std::vector<const std::type_info* const>;
    private:
        std::shared_ptr<node::NodeInterface> rootNode;
        const std::type_info& returnType;
        ArgumentTypes argumentTypes;
        LocalVariableTypes localVariableTypes;
    public:
        template <typename ArgumentTypes_, typename LocalVariableTypes_>
        Tree(const std::type_info& returnType_,
             ArgumentTypes_&& argumentTypes_,
             LocalVariableTypes_&& localVariableTypes_,
             std::shared_ptr<node::NodeInterface> rootNode_):
                rootNode(rootNode_),
                returnType(returnType_),
                argumentTypeList(std::forward<ArgumentTypes_>(argumentTypes_)),
                localVariableTypeList(std::forward<LocalVariableTypes_>(localVariableTypes_)){}
        virtual ~Tree() = default;
        Tree(const Tree&);
        Tree(Tree&&);
        Tree& operator=(const Tree&);
        Tree& operator=(Tree&&);
    public:
        auto getArgumentNum()const noexcept {return std::size(argumentTypes);}
        auto getLocalVariableNum()const noexcept {return std::size(localVariableTypes);}
    };
}

#endif