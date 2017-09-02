#ifndef GP_UTILITY_REFERENCE
#define GP_UTILITY_REFERENCE

#include "variable.hpp"
#include <optional>
#include <exception>

namespace gp::utility {
    template <typename T>
    class Reference {
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
        Reference(): variableRef(nullptr){}
        Reference(Variable& var): variableRef(&var){}
        ~Reference() = default;
        Reference(const Reference&) = default;
        Reference(Reference&&) = default;
        Reference& operator=(const Reference&) = default;
        Reference& operator=(Reference&&) = default;
    };
}

#endif