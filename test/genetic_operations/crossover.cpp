#define BOOST_TEST_NO_LIB
#define BOOST_TEST_MAIN

#include <gp/node/node.hpp>
#include <gp/tree/tree.hpp>
#include <gp/genetic_operations/default_modules.hpp>
#include <gp/genetic_operations/crossover.hpp>
#include <random>
#include <fstream>
#include <boost/test/unit_test.hpp>
using namespace gp;
using RND = std::mt19937;

BOOST_AUTO_TEST_SUITE(crossver)
    BOOST_AUTO_TEST_CASE(crossover) {
        utility::StringToType stringToType;
        stringToType.setTypeNamePair<int>("int");
        stringToType.setTypeNamePair<utility::LeftHandValue<int>>("lvalue[int]");
        stringToType.setTypeNamePair<utility::Reference<int>>("ref[int]");
        stringToType.setTypeNamePair<bool>("bool");

        auto treeGen1 = []() {
            /*
             * +--Progn[int,2]
             *     |
             *     +--If[int]
             *     |   |
             *     |   +--Greater[int]
             *     |   |   |
             *     |   |   +--Argument[int,0]
             *     |   |   |
             *     |   |   +--Const[int,0]
             *     |   |
             *     |   +--Subst[int]
             *     |   |   |
             *     |   |   +--LocalVariable[lvalue[int],0]
             *     |   |   |
             *     |   |   +--Argument[int,0]
             *     |   |
             *     |   +--Subst[int]
             *     |       |
             *     |       +--LocalVariable[lvalue[int],0]
             *     |       |
             *     |       +--Const[int,0]
             *     |
             *     +--LocalVariable[int,0]
             * */

            auto arg0 = node::NodeInterface::createInstance<node::ArgumentNode<int,0>>();
            auto constNode0 = node::NodeInterface::createInstance<node::ConstNode<int>>(0);
            auto greater = node::NodeInterface::createInstance<node::GreaterEqNode<int>>();
            greater->setChild(0, std::move(arg0));
            greater->setChild(1, std::move(constNode0));

            auto localVarL0 = node::NodeInterface::createInstance<node::LocalVariableNode<utility::LeftHandValue<int>>>(0);
            auto arg1 = node::NodeInterface::createInstance<node::ArgumentNode<int,0>>();
            auto subst0 = node::NodeInterface::createInstance<node::SubstitutionNode<int>>();
            subst0->setChild(0, std::move(localVarL0));
            subst0->setChild(1, std::move(arg1));

            auto localVarL1 = node::NodeInterface::createInstance<node::LocalVariableNode<utility::LeftHandValue<int>>>(0);
            auto constNode1 = node::NodeInterface::createInstance<node::ConstNode<int>>(0);
            auto subst1 = node::NodeInterface::createInstance<node::SubstitutionNode<int>>();
            subst1->setChild(0, std::move(localVarL1));
            subst1->setChild(1, std::move(constNode1));

            auto ifNode = node::NodeInterface::createInstance<node::IfNode<int>>();
            ifNode->setChild(0, std::move(greater));
            ifNode->setChild(1, std::move(subst0));
            ifNode->setChild(2, std::move(subst1));

            auto localVar0 = node::NodeInterface::createInstance<node::LocalVariableNode<int>>(0);

            auto progn = node::NodeInterface::createInstance<node::PrognNode<int,2>>();
            progn->setChild(0, std::move(ifNode));
            progn->setChild(1, std::move(localVar0));

            auto treeProperty = tree::TreeProperty{&utility::typeInfo<int>(),
                                                    {&utility::typeInfo<int>()},
                                                    {&utility::typeInfo<int>()}};
            return std::make_tuple(std::move(progn), std::move(treeProperty));
        };

        auto treeGen2 = [](){
            /*
             * +--Progn[int,3]
             *     |
             *     +--LocalVariable[bool,0]
             *     |
             *     +--LocalVariable[lvalue[int],1]
             *     |
             *     +--LocalVariable[int,1]
             * */
            auto localVarBool0 = node::NodeInterface::createInstance<node::LocalVariableNode<bool>>(0);
            auto localVarInt0 = node::NodeInterface::createInstance<node::LocalVariableNode<utility::LeftHandValue<int>>>(1);
            auto localVarInt1 = node::NodeInterface::createInstance<node::LocalVariableNode<int>>(1);

            auto progn = node::NodeInterface::createInstance<node::PrognNode<int, 3>>();
            progn->setChild(0, std::move(localVarBool0));
            progn->setChild(1, std::move(localVarInt0));
            progn->setChild(2, std::move(localVarInt1));

            auto treeProperty = tree::TreeProperty{&utility::typeInfo<int>(),
                                                   {&utility::typeInfo<int>()},
                                                   {&utility::typeInfo<bool>(), &utility::typeInfo<int>()}};
            return std::make_tuple(std::move(progn), std::move(treeProperty));
        };

        {//check tree evaluation
            auto [root1, prop1] = treeGen1();
            auto [root2, prop2] = treeGen2();

            auto tree1 = tree::Tree(std::move(prop1), std::move(root1));
            auto tree2 = tree::Tree(std::move(prop2), std::move(root2));

            {
                auto ans1 = tree1.evaluate(std::make_tuple(1));
                BOOST_CHECK(ans1.getEvaluationStatus() == utility::EvaluationStatus::ValueReturned);
                BOOST_CHECK_EQUAL(std::any_cast<int>(ans1.getReturnValue()), 1);
            }
            {
                auto ans1 = tree1.evaluate(std::make_tuple(-1));
                BOOST_CHECK(ans1.getEvaluationStatus() == utility::EvaluationStatus::ValueReturned);
                BOOST_CHECK_EQUAL(std::any_cast<int>(ans1.getReturnValue()), 0);
            }
            {
                auto ans2 = tree2.evaluate(std::make_tuple(1));
                BOOST_CHECK(ans2.getEvaluationStatus() == utility::EvaluationStatus::ValueReturned);
                BOOST_CHECK_EQUAL(std::any_cast<int>(ans2.getReturnValue()), 0);
            }
        }

        {//case 1
            RND rnd(0);
            auto [root1, prop1] = treeGen1();
            auto tree1 = tree::Tree(std::move(prop1), std::move(root1));
            auto [root2, prop2] = treeGen2();
            auto tree2 = tree::Tree(std::move(prop2), std::move(root2));

            genetic_operations::DefaultLocalVariableAdapter<RND> localVariableAdapter(rnd);
            auto nodeSelector = [](const node::NodeInterface& root1, const node::NodeInterface& root2){
                return std::pair<const node::NodeInterface&, const node::NodeInterface&>(root1.getChild(0), root2.getChild(2));
            };

            auto [cross1, cross2] = genetic_operations::crossover(tree1, tree2, nodeSelector, localVariableAdapter);

            BOOST_CHECK(cross1.getReturnType() == utility::typeInfo<int>());
            BOOST_CHECK_EQUAL(cross1.getArgumentNum() ,1);
            BOOST_CHECK(cross1.getArgumentType(0) == utility::typeInfo<int>());
            BOOST_CHECK_EQUAL(cross1.getLocalVariableNum(), 1);
            BOOST_CHECK(cross1.getLocalVariableType(0) == utility::typeInfo<int>());

            BOOST_CHECK(cross2.getReturnType() == utility::typeInfo<int>());
            BOOST_CHECK_EQUAL(cross2.getArgumentNum() ,1);
            BOOST_CHECK(cross2.getArgumentType(0) == utility::typeInfo<int>());
            BOOST_CHECK_EQUAL(cross2.getLocalVariableNum(), 2);
            BOOST_CHECK(cross2.getLocalVariableType(0) == utility::typeInfo<bool>());
            BOOST_CHECK(cross2.getLocalVariableType(1) == utility::typeInfo<int>());

            {
                auto ans = cross1.evaluate(std::make_tuple(1));
                BOOST_CHECK(ans.getEvaluationStatus() == utility::EvaluationStatus::ValueReturned);
                BOOST_CHECK_EQUAL(std::any_cast<int>(ans.getReturnValue()), 0);
            }

            {
                auto ans = cross2.evaluate(std::make_tuple(1));
                BOOST_CHECK(ans.getEvaluationStatus() == utility::EvaluationStatus::ValueReturned);
                BOOST_CHECK_EQUAL(std::any_cast<int>(ans.getReturnValue()), 1);
            }


            {
                auto ans = cross2.evaluate(std::make_tuple(-1));
                BOOST_CHECK(ans.getEvaluationStatus() == utility::EvaluationStatus::ValueReturned);
                BOOST_CHECK_EQUAL(std::any_cast<int>(ans.getReturnValue()), 0);
            }
        }
    }
BOOST_AUTO_TEST_SUITE_END()
