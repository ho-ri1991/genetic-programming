#ifndef GP_UTILITY_LEFT_HAND_VALUE
#define GP_UTILITY_LEFT_HAND_VALUE

#include "variable.hpp"
#include <optional>
#include <exception>
#include <variant>

namespace gp::utility {
    template <typename T>
    class LeftHandValue {
        static_assert(!std::is_same_v<T, Variable>);
    private:
        struct None{};
        std::variant<None, Variable*, T*> var;
    public:
        explicit operator bool()const noexcept {
            return std::visit([](auto&& var){
                using U = std::decay_t<decltype(var)>;
                if constexpr (std::is_same_v<U, Variable*>) {
                    return var != nullptr && var;
                } else if constexpr (std::is_same_v<U, T*>) {
                    return var != nullptr;
                } else {
                    return false;
                }
            }, var);
        }
        T& getRef(){
            return std::visit([](auto&& var)->T&{
                using U = std::decay_t<decltype(var)>;
                if constexpr (std::is_same_v<U, Variable*>) {
                    if(var == nullptr) throw std::runtime_error("the vaiable must not be null");
                    return var->template get<T&>();
                } else if constexpr (std::is_same_v<U, T*>) {
                    if(var == nullptr) throw std::runtime_error("the vaiable must not be null");
                    return *var;
                } else {
                    throw std::runtime_error("the vaiable must not be null");
                }
            }, var);
        }
        void setVariable(Variable& var_){var = &var_;}
        void setVariable(T& var_){var = &var_;}
    public:
        LeftHandValue():var(None{}){}
        LeftHandValue(Variable& var): var(&var){}
        LeftHandValue(T& var): var(&var){}
        ~LeftHandValue() = default;
        LeftHandValue(const LeftHandValue&) = default;
        LeftHandValue(LeftHandValue&&) = default;
        LeftHandValue& operator=(const LeftHandValue&) = default;
        LeftHandValue& operator=(LeftHandValue&&) = default;
    };
}

#endif
