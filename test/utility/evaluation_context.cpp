#define BOOST_TEST_NO_LIB
#define BOOST_TEST_MAIN
#include <gp/utility/evaluation_context.hpp>
#include <boost/test/unit_test.hpp>

using namespace gp;

BOOST_AUTO_TEST_SUITE(evaluation_context)
    BOOST_AUTO_TEST_CASE(evaluation_context) {
        {
            utility::EvaluationContext evaluationContext1(std::make_tuple(1, false),
                                                          utility::EvaluationContext::VariableTable(2),
                                                          3, 3);
            BOOST_CHECK_EQUAL(evaluationContext1.getEvaluationCount(), 0);
            BOOST_CHECK_EQUAL(evaluationContext1.getStackCount(), 0);
            BOOST_CHECK(evaluationContext1.getEvaluationStatus() == utility::EvaluationStatus::Evaluating);
            BOOST_CHECK_EQUAL(evaluationContext1.getArgument(0).get<int>(), 1);
            BOOST_CHECK_EQUAL(evaluationContext1.getArgument(1).get<bool>(), false);
            evaluationContext1.getArgument(0).get<int&>() = 2;
            evaluationContext1.getArgument(1).get<bool&>() = true;
            BOOST_CHECK_EQUAL(evaluationContext1.getArgument(0).get<int>(), 2);
            BOOST_CHECK_EQUAL(evaluationContext1.getArgument(1).get<bool>(), true);
            evaluationContext1.setLocalVariable(0, 1);
            evaluationContext1.setLocalVariable(1, false);
            BOOST_CHECK_EQUAL(evaluationContext1.getLocalVariable(0).get<int>(), 1);
            BOOST_CHECK_EQUAL(evaluationContext1.getLocalVariable(1).get<bool>(), false);
            evaluationContext1.setReturnValue(10.5);
            BOOST_CHECK(evaluationContext1.getEvaluationStatus() == utility::EvaluationStatus::ValueReturned);
            BOOST_CHECK_EQUAL(std::any_cast<double>(evaluationContext1.getReturnValue()), 10.5);
            evaluationContext1.clearEvaluationStatus();

            evaluationContext1.incrementEvaluationCount();
            evaluationContext1.incrementEvaluationCount();
            BOOST_CHECK(evaluationContext1.getEvaluationStatus() == utility::EvaluationStatus::Evaluating);
            BOOST_CHECK_EQUAL(evaluationContext1.getEvaluationCount(), 2);
            evaluationContext1.incrementEvaluationCount();
            evaluationContext1.incrementEvaluationCount();
            BOOST_CHECK(evaluationContext1.getEvaluationStatus() == utility::EvaluationStatus::EvaluationCountExceeded);

            evaluationContext1.incrementStackCount();
            evaluationContext1.incrementStackCount();
            evaluationContext1.incrementStackCount();
            evaluationContext1.incrementStackCount();
            BOOST_CHECK(evaluationContext1.getEvaluationStatus() == utility::EvaluationStatus::EvaluationCountExceeded);
            evaluationContext1.decrementStackCount();
            evaluationContext1.clearEvaluationStatus();
            evaluationContext1.incrementStackCount();
            evaluationContext1.incrementStackCount();
            BOOST_CHECK(evaluationContext1.getEvaluationStatus() == utility::EvaluationStatus::StackCountExceeded);
        }
        {
            utility::EvaluationContext evaluationContext2(utility::EvaluationContext::VariableTable{utility::Variable(1), utility::Variable(false)},
                                                          utility::EvaluationContext::VariableTable(2),
                                                          3, 3);
            BOOST_CHECK_EQUAL(evaluationContext2.getEvaluationCount(), 0);
            BOOST_CHECK_EQUAL(evaluationContext2.getStackCount(), 0);
            BOOST_CHECK(evaluationContext2.getEvaluationStatus() == utility::EvaluationStatus::Evaluating);
            BOOST_CHECK_EQUAL(evaluationContext2.getArgument(0).get<int>(), 1);
            BOOST_CHECK_EQUAL(evaluationContext2.getArgument(1).get<bool>(), false);
            evaluationContext2.getArgument(0).get<int&>() = 2;
            evaluationContext2.getArgument(1).get<bool&>() = true;
            BOOST_CHECK_EQUAL(evaluationContext2.getArgument(0).get<int>(), 2);
            BOOST_CHECK_EQUAL(evaluationContext2.getArgument(1).get<bool>(), true);
            evaluationContext2.setLocalVariable(0, 1);
            evaluationContext2.setLocalVariable(1, false);
            BOOST_CHECK_EQUAL(evaluationContext2.getLocalVariable(0).get<int>(), 1);
            BOOST_CHECK_EQUAL(evaluationContext2.getLocalVariable(1).get<bool>(), false);
            evaluationContext2.setReturnValue(10.5);
            BOOST_CHECK(evaluationContext2.getEvaluationStatus() == utility::EvaluationStatus::ValueReturned);
            BOOST_CHECK_EQUAL(std::any_cast<double>(evaluationContext2.getReturnValue()), 10.5);
        }
        {
            utility::Variable var(1);
            utility::EvaluationContext evaluationContext3(std::make_tuple(utility::Reference<int>(var), false),
                                                          utility::EvaluationContext::VariableTable(2),
                                                          3, 3);
            BOOST_CHECK_EQUAL(evaluationContext3.getEvaluationCount(), 0);
            BOOST_CHECK_EQUAL(evaluationContext3.getStackCount(), 0);
            BOOST_CHECK(evaluationContext3.getEvaluationStatus() == utility::EvaluationStatus::Evaluating);
            BOOST_CHECK_EQUAL(evaluationContext3.getArgument(0).get<int>(), 1);
            BOOST_CHECK_EQUAL(evaluationContext3.getArgument(1).get<bool>(), false);
            evaluationContext3.getArgument(0).get<int&>() = 2;
            evaluationContext3.getArgument(1).get<bool&>() = true;
            BOOST_CHECK_EQUAL(evaluationContext3.getArgument(0).get<int>(), 2);
            BOOST_CHECK_EQUAL(var.get<int>(), 2);
            BOOST_CHECK_EQUAL(evaluationContext3.getArgument(1).get<bool>(), true);
            evaluationContext3.setLocalVariable(0, 1);
            evaluationContext3.setLocalVariable(1, false);
            BOOST_CHECK_EQUAL(evaluationContext3.getLocalVariable(0).get<int>(), 1);
            BOOST_CHECK_EQUAL(evaluationContext3.getLocalVariable(1).get<bool>(), false);
            evaluationContext3.setReturnValue(10.5);
            BOOST_CHECK(evaluationContext3.getEvaluationStatus() == utility::EvaluationStatus::ValueReturned);
            BOOST_CHECK_EQUAL(std::any_cast<double>(evaluationContext3.getReturnValue()), 10.5);
        }
    }
BOOST_AUTO_TEST_SUITE_END()
