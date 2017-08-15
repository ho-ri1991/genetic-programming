#ifndef GP_UTILITY_EVALUATION_CONTEXT
#define GP_UTILITY_EVALUATION_CONTEXT

#include <vector>
#include <any>
#include <cassert>
#include <exception>

namespace gp::utility {
    enum class EvaluationStatus {
        Evaluating,
        EvaluationCountExceeded,
        StackCountExceeded,
        ValueReturned,
        BreakCalled,
        ContinueCalled
    };

    class EvaluationContext {
    public:
        using VariableTable = std::vector<std::any>;
    private:
        using EvaluationCount = long long;
        using StackCount = long long;
        static constexpr EvaluationCount defaultMaxEvaluationCount = 10000;
        static constexpr StackCount defaultMaxStackCount = 10000;
    private:
        EvaluationStatus evaluationStatus;
        VariableTable arguments;
        VariableTable localVariables;
        EvaluationCount evaluationCount;
        StackCount stackCount;
        const EvaluationCount maxEvaluationCount;
        const StackCount maxStackCount;
        std::any returnValue;
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
        std::any& getArgument(std::size_t n){
            assert(n < std::size(arguments));
            return arguments[n];
        }
        const std::any& getArgument(std::size_t n)const {
            assert(n < std::size(arguments));
            return arguments[n];
        }
        std::any& getLocalVariable(std::size_t n) {
            assert(n < std::size(localVariables));
            return localVariables[n];
        }
        const std::any& getLocalVariable(std::size_t n)const {
            assert(n < std::size(localVariables));
            return localVariables[n];
        }
        template <typename T>
        void setArgument(std::size_t n, T&& val) {
            assert(n < std::size(arguments));
            arguments[n] = std::forward<T>(val);
        }
        template <typename T>
        void setLocalVariable(std::size_t n, T&& val){
            assert(n < std::size(localVariables));
            localVariables[n] = std::forward<T>(val);
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
                          EvaluationCount maxEvaluationCount_ = defaultMaxEvaluationCount,
                          StackCount maxStackCount_ = defaultMaxStackCount)
                : arguments(std::forward<Arguments>(arguments_))
                , localVariables(std::forward<LocalVariables>(localVariables_))
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