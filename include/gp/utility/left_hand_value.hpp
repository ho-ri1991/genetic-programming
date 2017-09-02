#ifndef GP_UTILITY_LEFT_HAND_VALUE
#define GP_UTILITY_LEFT_HAND_VALUE

#include "variable.hpp"
#include <optional>
#include <exception>

namespace gp::utility {
    template <typename T>
    class LeftHandValue {
    private:
        Variable* variableRef;
    public:
        explicit operator bool()const noexcept { return variableRef != nullptr;}
        T& getRef(){
            assert(variableRef != nullptr);
            if(!variableRef) throw std::runtime_error("the vaiable must not be null");
            assert(variableRef->hasValue() && variableRef->getType() == typeid(T));
            return variableRef->get<T&>();
        }
        void setVariable(Variable& var){variableRef = &var;}
    public:
        LeftHandValue(): variableRef(nullptr){}
        LeftHandValue(Variable& var): variableRef(&var){}
        ~LeftHandValue() = default;
        LeftHandValue(const LeftHandValue&) = default;
        LeftHandValue(LeftHandValue&&) = default;
        LeftHandValue& operator=(const LeftHandValue&) = default;
        LeftHandValue& operator=(LeftHandValue&&) = default;
    };
}

#endif