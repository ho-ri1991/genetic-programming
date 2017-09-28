#define BOOST_TEST_NO_LIB
#define BOOST_TEST_MAIN

#include <gp/node/node.hpp>
#include <gp/utility/reference.hpp>
#include <boost/test/unit_test.hpp>

using namespace gp;
using namespace gp::node;

BOOST_AUTO_TEST_SUITE(string_to_node)
    BOOST_AUTO_TEST_CASE(string_to_node) {
        StringToNode stringToNode;

        auto pAdd = NodeInterface::createInstance<AddNode<int>>();
        const auto& add = *pAdd;
        auto pSubst = NodeInterface::createInstance<SubstitutionNode<int>>();
        const auto& subst = *pSubst;
        auto pArg0 = NodeInterface::createInstance<ArgumentNode<int, 0>>();
        const auto& arg0 = *pArg0;
        auto pArgRef0 = NodeInterface::createInstance<ArgumentNode<utility::Reference<int>, 0>>();
        const auto& argRef0 = *pArgRef0;
        auto pLocalVar0 = NodeInterface::createInstance<LocalVariableNode<int, 0>>();
        const auto& localVar0 = *pLocalVar0;
        auto pLocalVarRef0 = NodeInterface::createInstance<LocalVariableNode<utility::Reference<int>, 0>>();
        const auto& localVarRef0 = *pLocalVarRef0;
        auto pProgn = NodeInterface::createInstance<PrognNode<int, 3>>();
        const auto& progn = *pProgn;
        auto pConst = NodeInterface::createInstance<ConstNode<int>>();
        const auto& constNode = *pConst;
        stringToNode.registerNode(std::move(pAdd));
        stringToNode.registerNode(std::move(pSubst));
        stringToNode.registerNode(std::move(pArg0));
        stringToNode.registerNode(std::move(pArgRef0));
        stringToNode.registerNode(std::move(pLocalVar0));
        stringToNode.registerNode(std::move(pLocalVarRef0));
        stringToNode.registerNode(std::move(pProgn));
        stringToNode.registerNode(std::move(pConst));

        {
            auto checker = [](const NodeInterface& node1, const NodeInterface& node2){
                if(node1.getReturnType() != node2.getReturnType())return false;
                if(node1.getChildNum() != node2.getChildNum())return false;
                for(int i = 0; i < node1.getChildNum(); ++i){
                    if(node1.getChildReturnType(i) != node2.getChildReturnType(i))return false;
                }
                if(node1.getNodeType() != node2.getNodeType())return false;
                if(node1.getNodeType() != NodeType::Const)return node1.getNodeName() == node2.getNodeName();
                else return true;
            };
            BOOST_CHECK(stringToNode.hasNode(add.getNodeName()));
            auto pAdd = stringToNode(add.getNodeName());
            BOOST_CHECK(checker(*pAdd, add));

            BOOST_CHECK(stringToNode.hasNode(subst.getNodeName()));
            auto pSubst = stringToNode(subst.getNodeName());
            BOOST_CHECK(checker(*pSubst, subst));

            BOOST_CHECK(stringToNode.hasNode(arg0.getNodeName()));
            auto pArg0 = stringToNode(arg0.getNodeName());
            BOOST_CHECK(checker(*pArg0, arg0));

            BOOST_CHECK(stringToNode.hasNode(argRef0.getNodeName()));
            auto pArgRef0 = stringToNode(argRef0.getNodeName());
            BOOST_CHECK(checker(*pArgRef0, argRef0));

            BOOST_CHECK(stringToNode.hasNode(localVar0.getNodeName()));
            auto pLocalVar0 = stringToNode(localVar0.getNodeName());
            BOOST_CHECK(checker(*pLocalVar0, localVar0));

            BOOST_CHECK(stringToNode.hasNode(localVarRef0.getNodeName()));
            auto pLocalVarRef0 = stringToNode(localVarRef0.getNodeName());
            BOOST_CHECK(checker(*pLocalVarRef0, localVarRef0));

            BOOST_CHECK(stringToNode.hasNode(progn.getNodeName()));
            auto pProgn = stringToNode(progn.getNodeName());
            BOOST_CHECK(checker(*pProgn, progn));

            BOOST_CHECK(stringToNode.hasNode(constNode.getNodeName()));
            auto pConst = stringToNode(constNode.getNodeName());
            BOOST_CHECK(checker(*pConst, constNode));
        }

        auto addName = add.getNodeName();
        stringToNode.deleteNode(addName);
        BOOST_CHECK(!stringToNode.hasNode(addName));
        BOOST_CHECK(stringToNode.hasNode(subst.getNodeName()));
    }
BOOST_AUTO_TEST_SUITE_END()
