#ifndef GP_GENETIC_OPERATIONS_DEFAULT_MODULES
#define GP_GENETIC_OPERATIONS_DEFAULT_MODULES

#include <gp/node/node_interface.hpp>
#include <gp/tree/random_node_generator.hpp>
#include <gp/tree/tree.hpp>
#include <gp/tree_operations/tree_operations.hpp>
#include <random>

namespace gp::genetic_operations {
    template <typename RandomEngine, typename RandomNodeGenerator>
    class DefaultRandomTreeGenerator {
    private:
        RandomEngine& rnd;
        RandomNodeGenerator& randomNodeGenerator;
    public:
        DefaultRandomTreeGenerator(RandomEngine& rnd_, RandomNodeGenerator& randomNodeGenerator_): rnd(rnd_), randomNodeGenerator(randomNodeGenerator_){}
    public:
        node::NodeInterface::node_instance_type operator()(const tree::TreeProperty& treeProperty, std::size_t maxTreeDepth){
            return tree_operations::generateTreeRandom(treeProperty, randomNodeGenerator, rnd, maxTreeDepth);
        }
    };

    template <typename RandomEngine>
    class DefaultNodeSelector {
    private:
        RandomEngine& rnd;
    public:
        DefaultNodeSelector(RandomEngine& rnd_): rnd(rnd_){}
    private:
        const node::NodeInterface* selectHelper(const node::NodeInterface& rootNode, std::size_t& num)const {
            if(num == 0) return &rootNode;
            else {
                for(int i = 0; i < rootNode.getChildNum(); ++i) {
                    --num;
                    const node::NodeInterface* ans = selectHelper(rootNode.getChild(i), num);
                    if(ans != nullptr) return ans;
                }
                return nullptr;
            }
        }
    public:
        const node::NodeInterface& operator()(const node::NodeInterface& rootNode) {
            auto nodeNum = tree_operations::getSubtreeNodeNum(rootNode);
            std::uniform_int_distribution<int> dist(0, nodeNum - 1);
            std::size_t num = dist(rnd);
            const node::NodeInterface* ans = selectHelper(rootNode, num);
            assert(ans != nullptr);
            return *ans;
        }
    };
}

#endif
