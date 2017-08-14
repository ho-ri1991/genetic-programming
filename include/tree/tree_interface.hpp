#ifndef GP_TREE_INTERFACE
#define GP_TREE_INTERFACE

#include <typeinfo>
#include <memory>
#include <node/node_interface.hpp>

namespace gp::tree {
    class TreeInterface {
    public:
        virtual const std::type_info* const getReturnType()const noexcept = 0;
        virtual const std::type_info* const getArgumentType(std::size_t)const = 0;
        virtual std::size_t getArgumentNum()const = 0;
        virtual const std::type_info* const getLocalVariableType(std::size_t)const = 0;
        virtual std::size_t getLocalVariableNum()const = 0;
        virtual std::shared_ptr<node::NodeInterface> getRootNode()noexcept = 0;
        virtual std::shared_ptr<const node::NodeInterface> getRootNode()const noexcept = 0;
        virtual void setRootNode(std::shared_ptr<node::NodeInterface> root)noexcept = 0;

    public:
        virtual ~TreeInterface() = default;
        TreeInterface() = default;
        TreeInterface(const TreeInterface&) = default;
        TreeInterface(TreeInterface&&) = default;
        TreeInterface& operator=(const TreeInterface&) = default;
        TreeInterface& operator=(TreeInterface&&) = default;
    };
}

#endif