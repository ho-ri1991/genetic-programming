#define BOOST_TEST_NO_LIB
#define BOOST_TEST_MAIN

#include <gp/node/node.hpp>
#include <gp/node/array_nodes.hpp>
#include <gp/tree/tree.hpp>
#include <boost/test/unit_test.hpp>

using namespace gp;
using namespace gp::node;

BOOST_AUTO_TEST_SUITE(array_node)
    BOOST_AUTO_TEST_CASE(array_index_access) {
        auto argNode = NodeInterface::createInstance<ArgumentNode<GpArray<int>, 0>>();
        auto atNode = NodeInterface::createInstance<AtNode<GpArray<int>>>();
        auto idxNode = NodeInterface::createInstance<ConstNode<int>>(1);

        atNode->setChild(0, std::move(argNode));
        atNode->setChild(1, std::move(idxNode));

        utility::EvaluationContext evaluationContext1(std::make_tuple(GpArray{1, 2, 3}),
                                                      utility::EvaluationContext::VariableTable{},
                                                      100, 100);
        auto ans1 = atNode->evaluateByAny(evaluationContext1);
        BOOST_CHECK(evaluationContext1.getEvaluationStatus() == utility::EvaluationStatus::Evaluating);
        BOOST_CHECK_EQUAL(std::any_cast<int>(ans1), 2);

        utility::EvaluationContext evaluationContext2(std::make_tuple(GpArray<int>{1}),
                                                      utility::EvaluationContext::VariableTable{},
                                                      100, 100);
        auto ans2 = atNode->evaluateByAny(evaluationContext2);
        BOOST_CHECK(evaluationContext2.getEvaluationStatus() != utility::EvaluationStatus::Evaluating);
    }
    BOOST_AUTO_TEST_CASE(lvalue_array_index_access) {
        auto argNode = NodeInterface::createInstance<ArgumentNode<utility::LeftHandValue<GpArray<int>>, 0>>();
        auto atNode = NodeInterface::createInstance<AtNode<utility::LeftHandValue<GpArray<int>>>>();
        auto idxNode = NodeInterface::createInstance<ConstNode<int>>(1);

        atNode->setChild(0, std::move(argNode));
        atNode->setChild(1, std::move(idxNode));

        utility::EvaluationContext evaluationContext1(std::make_tuple(GpArray{1, 2, 3}),
                                                      utility::EvaluationContext::VariableTable{},
                                                      100, 100);
        auto ans1 = atNode->evaluateByAny(evaluationContext1);
        BOOST_CHECK(evaluationContext1.getEvaluationStatus() == utility::EvaluationStatus::Evaluating);
        BOOST_CHECK(std::any_cast<utility::LeftHandValue<int>>(ans1));
        BOOST_CHECK_EQUAL(std::any_cast<utility::LeftHandValue<int>>(ans1).getRef(), 2);

        utility::EvaluationContext evaluationContext2(std::make_tuple(GpArray<int>{1}),
                                                      utility::EvaluationContext::VariableTable{},
                                                      100, 100);
        auto ans2 = atNode->evaluateByAny(evaluationContext2);
        BOOST_CHECK(evaluationContext2.getEvaluationStatus() != utility::EvaluationStatus::Evaluating);
    }
    BOOST_AUTO_TEST_CASE(array_element_substitution) {
        auto prognNode = NodeInterface::createInstance<PrognNode<int, 2>>();

        auto atNode0 = NodeInterface::createInstance<AtNode<GpArray<int>>>();
        auto idxNode0 = NodeInterface::createInstance<ConstNode<int>>(1);
        auto argNode0 = NodeInterface::createInstance<ArgumentNode<GpArray<int>, 0>>();

        auto substNode = NodeInterface::createInstance<SubstitutionNode<int>>();
        auto argNode = NodeInterface::createInstance<ArgumentNode<utility::LeftHandValue<GpArray<int>>, 0>>();
        auto atNode = NodeInterface::createInstance<AtNode<utility::LeftHandValue<GpArray<int>>>>();
        auto idxNode = NodeInterface::createInstance<ConstNode<int>>(1);
        auto constNode = NodeInterface::createInstance<ConstNode<int>>(-2);

        atNode0->setChild(0, std::move(argNode0));
        atNode0->setChild(1, std::move(idxNode0));

        prognNode->setChild(1, std::move(atNode0));

        atNode->setChild(0, std::move(argNode));
        atNode->setChild(1, std::move(idxNode));

        substNode->setChild(0, std::move(atNode));
        substNode->setChild(1, std::move(constNode));

        prognNode->setChild(0, std::move(substNode));

        utility::EvaluationContext evaluationContext1(std::make_tuple(GpArray{1, 2, 3}),
                                                      utility::EvaluationContext::VariableTable{},
                                                      100, 100);
        auto ans1 = prognNode->evaluateByAny(evaluationContext1);
        BOOST_CHECK(evaluationContext1.getEvaluationStatus() == utility::EvaluationStatus::Evaluating);
        BOOST_CHECK_EQUAL(std::any_cast<int>(ans1), -2);

        utility::EvaluationContext evaluationContext2(std::make_tuple(GpArray<int>{1}),
                                                      utility::EvaluationContext::VariableTable{},
                                                      100, 100);
        auto ans2 = prognNode->evaluateByAny(evaluationContext2);
        BOOST_CHECK(evaluationContext2.getEvaluationStatus() != utility::EvaluationStatus::Evaluating);
    }
    BOOST_AUTO_TEST_CASE(array_size) {
        auto argNode = NodeInterface::createInstance<ArgumentNode<GpArray<int>, 0>>();
        auto sizeOfArrNode = NodeInterface::createInstance<SizeOfArray<GpArray<int>>>();

        sizeOfArrNode->setChild(0, std::move(argNode));

        utility::EvaluationContext evaluationContext1(std::make_tuple(GpArray{1, 2, 3}),
                                                      utility::EvaluationContext::VariableTable{},
                                                      100, 100);
        auto ans1 = sizeOfArrNode->evaluateByAny(evaluationContext1);
        BOOST_CHECK(evaluationContext1.getEvaluationStatus() == utility::EvaluationStatus::Evaluating);
        BOOST_CHECK_EQUAL(std::any_cast<int>(ans1), 3);

        utility::EvaluationContext evaluationContext2(std::make_tuple(GpArray<int>{}),
                                                      utility::EvaluationContext::VariableTable{},
                                                      100, 100);
        auto ans2 = sizeOfArrNode->evaluateByAny(evaluationContext2);
        BOOST_CHECK_EQUAL(std::any_cast<int>(ans2), 0);
    }
    BOOST_AUTO_TEST_CASE(lvalue_array_size) {
        auto argNode = NodeInterface::createInstance<ArgumentNode<utility::LeftHandValue<GpArray<int>>, 0>>();
        auto sizeOfArrNode = NodeInterface::createInstance<SizeOfArray<utility::LeftHandValue<GpArray<int>>>>();

        sizeOfArrNode->setChild(0, std::move(argNode));

        utility::EvaluationContext evaluationContext1(std::make_tuple(GpArray{1, 2, 3}),
                                                      utility::EvaluationContext::VariableTable{},
                                                      100, 100);
        auto ans1 = sizeOfArrNode->evaluateByAny(evaluationContext1);
        BOOST_CHECK(evaluationContext1.getEvaluationStatus() == utility::EvaluationStatus::Evaluating);
        BOOST_CHECK_EQUAL(std::any_cast<int>(ans1), 3);

        utility::EvaluationContext evaluationContext2(std::make_tuple(GpArray<int>{}),
                                                      utility::EvaluationContext::VariableTable{},
                                                      100, 100);
        auto ans2 = sizeOfArrNode->evaluateByAny(evaluationContext2);
        BOOST_CHECK_EQUAL(std::any_cast<int>(ans2), 0);
    }
BOOST_AUTO_TEST_SUITE_END()
