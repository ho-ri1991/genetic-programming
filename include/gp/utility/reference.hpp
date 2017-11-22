#ifndef GP_UTILITY_REFERENCE
#define GP_UTILITY_REFERENCE

#include "left_hand_value.hpp"

namespace gp::utility {
    template <typename T>
    class Reference {
    private:
        LeftHandValue<T> lvalue;
    public:
        explicit operator bool()const noexcept { return static_cast<bool>(lvalue);}
        T& getRef(){return lvalue.getRef();}
        void setVariable(Variable& var){lvalue.setVariable(var);}
        void setVariable(T& var){lvalue.setVariable(var);}
    public:
        Reference():lvalue(){}
        Reference(Variable& var):lvalue(var){}
        Reference(T& var):lvalue(var){}
        ~Reference() = default;
        Reference(const Reference&) = default;
        Reference(Reference&&) = default;
        Reference& operator=(const Reference&) = default;
        Reference& operator=(Reference&&) = default;
    };
}

#endif
