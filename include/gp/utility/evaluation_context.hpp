#ifndef GP_UTILITY_EVALUATION_CONTEXT
#define GP_UTILITY_EVALUATION_CONTEXT

#include <vector>
#include <any>
#include <cassert>
#include <exception>
#include <tuple>
#include "variable.hpp"
#include "reference.hpp"
#include "left_hand_value.hpp"

namespace gp::utility {
    enum class EvaluationStatus {
        Evaluating,
        EvaluationCountExceeded,
        StackCountExceeded,
        ValueReturned,
        BreakCalled,
        ContinueCalled,
        InvalidLeftHandValue,
        InvalidReference,
        InvalidValue
    };

    class EvaluationContext {
    public:
        using VariableTable = std::vector<Variable>;
        using EvaluationCount = long long;
        using StackCount = long long;
    private:
        EvaluationStatus evaluationStatus;
        VariableTable arguments;
        VariableTable localVariables;
        EvaluationCount evaluationCount;
        StackCount stackCount;
        const EvaluationCount maxEvaluationCount;
        const StackCount maxStackCount;
        std::any returnValue;
    private:
        template <typename T>
        struct SetArgumentHelper {
            static_assert(!std::is_same_v<T, LeftHandValue<T>>);
            static void set(T& val, Variable& var) {var.set(val);}
        };
        template <typename T>
        struct SetArgumentHelper<Reference<T>>{
            static void set(Reference<T>& val, Variable& var) {var.set(&val.getRef());}
        };
        template <std::size_t, typename ...>
        struct SetArgumentsHelper;
        template <std::size_t offset,
                  template <typename ...> typename Tpl,
                  typename ...Args>
        struct SetArgumentsHelper<offset, Tpl<Args...>> {
            static void set(Tpl<Args...>& tpl, VariableTable& argumentTable) {
                if(std::size(argumentTable) != sizeof...(Args))throw std::runtime_error("the number of arguments mismatch");
                if constexpr (sizeof...(Args) <= offset) return;
                else {
                    SetArgumentHelper<std::tuple_element_t<offset, Tpl<Args...>>>::set(std::get<offset>(tpl), argumentTable[offset]);
                    return SetArgumentsHelper<offset + 1, Tpl<Args...>>::set(tpl, argumentTable);
                }
            }
        };

        template <typename T>
        struct CreateVariableTableHelper {
            template <typename Arguments>
            static VariableTable create(Arguments&& vars) {
                return VariableTable(std::forward<Arguments>(vars));
            }
        };

        template <typename ...Args>
        struct CreateVariableTableHelper<std::tuple<Args...>> {
            static VariableTable create(std::tuple<Args...>& tpl) {
                auto variableTable = VariableTable(sizeof...(Args));
                SetArgumentsHelper<0, std::tuple<Args...>>::set(tpl, variableTable);
                return variableTable;
            }
            static VariableTable create(std::tuple<Args...>&& tpl) {
                auto variableTable = VariableTable(sizeof...(Args));
                SetArgumentsHelper<0, std::tuple<Args...>>::set(tpl, variableTable);
                return variableTable;
            }
        };

        template <typename VariableTable_>
        static VariableTable createVariableTable(VariableTable_&& variableTable) {
            return CreateVariableTableHelper<std::decay_t<VariableTable_>>::create(std::forward<VariableTable_>(variableTable));
        }
    public:
        void incrementEvaluationCount()noexcept {
            if(evaluationCount < maxEvaluationCount) {
                ++evaluationCount;
            } else {
               if(evaluationStatus == EvaluationStatus::Evaluating){
                   evaluationStatus = EvaluationStatus::EvaluationCountExceeded;
               }
            }
        }
        void incrementStackCount()noexcept {
            if(stackCount < maxStackCount) {
                ++stackCount;
            }else {
                if(evaluationStatus == EvaluationStatus::Evaluating) {
                    evaluationStatus = EvaluationStatus::StackCountExceeded;
                }
            }
        }
        void decrementStackCount()noexcept {
            if(stackCount > 0)--stackCount;
        }
        auto getEvaluationStatus()const noexcept {return evaluationStatus;}
        void setEvaluationStatusWithoutUpdate(EvaluationStatus status){
            if(evaluationStatus == EvaluationStatus::Evaluating) {
                evaluationStatus = status;
            }
        }
        void clearEvaluationStatus(){evaluationStatus = EvaluationStatus::Evaluating;}
        auto getEvaluationCount()const noexcept {return evaluationCount;}
        auto getStackCount()const noexcept {return stackCount;}
        auto getArgumentNum()const noexcept {return std::size(arguments);}
        auto getLocalVariableNum()const noexcept {return std::size(localVariables);}
        decltype(auto) getArgument(std::size_t n){
            assert(n < std::size(arguments));
            return arguments[n];
        }
        decltype(auto) getArgument(std::size_t n)const {
            assert(n < std::size(arguments));
            return arguments[n];
        }
        decltype(auto) getLocalVariable(std::size_t n) {
            assert(n < std::size(localVariables));
            return localVariables[n];
        }
        decltype(auto) getLocalVariable(std::size_t n)const {
            assert(n < std::size(localVariables));
            return localVariables[n];
        }
        template <typename T>
        void setLocalVariable(std::size_t n, T&& val){
            assert(n < std::size(localVariables));
            localVariables[n].set(std::forward<T>(val));
        }
        template <typename T>
        void setReturnValue(T&& val){
            if(evaluationStatus == EvaluationStatus::Evaluating){
                evaluationStatus = EvaluationStatus::ValueReturned;
                returnValue = std::forward<T>(val);
            }
        }
        std::any getReturnValue()const {return returnValue;}
    public:
        template <typename Arguments, typename LocalVariables>
        EvaluationContext(Arguments&& arguments_,
                          LocalVariables&& localVariables_,
                          EvaluationCount maxEvaluationCount_,
                          StackCount maxStackCount_)
                : arguments(createVariableTable(std::forward<Arguments>(arguments_)))
                , localVariables(createVariableTable(std::forward<LocalVariables>(localVariables_)))
                , evaluationCount(0)
                , stackCount(0)
                , maxEvaluationCount(maxEvaluationCount_)
                , maxStackCount(maxStackCount_)
                , evaluationStatus(EvaluationStatus::Evaluating){}
        EvaluationContext() = delete;
        ~EvaluationContext() = default;
        EvaluationContext(const EvaluationContext&) = default;
        EvaluationContext(EvaluationContext&&) = default;
        EvaluationContext& operator=(const EvaluationContext&) = default;
        EvaluationContext& operator=(EvaluationContext&&) = default;
    };
}

#endif
