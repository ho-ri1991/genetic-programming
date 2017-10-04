#ifndef GP_GENETIC_OPERATIONS_CROSSOVER
#define GP_GENETIC_OPERATIONS_CROSSOVER

#include <gp/node/node_interface.hpp>
#include <gp/tree/tree.hpp>
#include <type_traits>
#include <optional>

namespace gp::genetic_operations {
    namespace detail {
        std::size_t getChildIndex(const node::NodeInterface& parent, const node::NodeInterface& child) noexcept {
            for(int i = 0; i < parent.getChildNum(); ++i) {
                if(parent.hasChild(i) && &parent.getChild(i) == &child) return i;
            }
            return parent.getChildNum();
        }
    }
    template <typename CrossoverNodeSelector,
              typename LocalVariableAdapter>
    auto crossover(tree::Tree tree1,
                   tree::Tree tree2,
                   CrossoverNodeSelector& crossoverNodeSelector,
                   LocalVariableAdapter& localVariableAdapter)->std::pair<tree::Tree, tree::Tree>{
        static_assert(
                std::is_same_v<
                        std::pair<const node::NodeInterface&, const node::NodeInterface&>,
                        decltype(std::declval<CrossoverNodeSelector>()(std::declval<const node::NodeInterface&>(), std::declval<node::NodeInterface>()))
                >
        );
        static_assert(
                std::is_same_v<
                        void,
                        decltype(std::declval<LocalVariableAdapter>()(std::declval<node::NodeInterface&>(), std::declval<tree::TreeProperty&>()))
                >
        );
        auto rootNode1 = std::move(tree1).getRootNodeInstance();
        auto treeProperty1 = std::move(tree1).getTreeProperty();
        auto rootNode2 = std::move(tree2).getRootNodeInstance();
        auto treeProperty2 = std::move(tree2).getTreeProperty();

        const auto& [cnode1, cnode2] = crossoverNodeSelector(*rootNode1, *rootNode2);
        auto& node1 = const_cast<node::NodeInterface&>(cnode1);
        auto& node2 = const_cast<node::NodeInterface&>(cnode2);
        std::cout<<node1.getNodeName()<<std::endl;
        std::cout<<node2.getNodeName()<<std::endl;
        if(node1.hasParent()){
            const auto idx1 = detail::getChildIndex(node1.getParent(), node1);
            if(node2.hasParent()){
                const auto idx2 = detail::getChildIndex(node2.getParent(), node2);
                auto& parent1 = node1.getParent();
                auto& parent2 = node2.getParent();
                auto org1 = parent1.setChild(idx1, nullptr);
                localVariableAdapter(*org1, treeProperty2);
                auto org2 = parent2.setChild(idx2, std::move(org1));
                localVariableAdapter(*org2, treeProperty1);
                parent1.setChild(idx1, std::move(org2));
            } else {
                localVariableAdapter(*rootNode2, treeProperty1);
                auto& parent1 = node1.getParent();
                auto org1 = parent1.setChild(idx1, std::move(rootNode2));
                localVariableAdapter(*org1, treeProperty2);
                rootNode2 = std::move(org1);
            }
        } else {
            if(node2.hasParent()){
                const auto idx2 = detail::getChildIndex(node2.getParent(), node2);
                localVariableAdapter(*rootNode1, treeProperty2);
                auto& parent2 = node2.getParent();
                auto org2 = parent2.setChild(idx2, std::move(rootNode1));
                localVariableAdapter(*org2, treeProperty1);
                rootNode1 = std::move(org2);
            } else {
                localVariableAdapter(*rootNode1, treeProperty2);
                localVariableAdapter(*rootNode2, treeProperty1);
                rootNode1.swap(rootNode2);
            }
        }
        return std::make_pair(tree::Tree(std::move(treeProperty1), std::move(rootNode1)), tree::Tree(std::move(treeProperty2), std::move(rootNode2)));
    }
}

#endif
