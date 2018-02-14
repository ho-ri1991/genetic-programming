#ifndef GP_UTILITY_VARIABLE
#define GP_UTILITY_VARIABLE

#include <any>
#include <type_traits>
#include <cassert>
#include <exception>
#include <typeinfo>

namespace gp::utility {
    class Variable {
    private:
        bool hasPointer;
        std::any variable;
        const std::type_info* type;
    public:
        bool hasValue()const noexcept {return *type != typeid(void);}
        explicit operator bool()const noexcept {return variable.has_value();}
        template <typename T>
        T get() {
            if constexpr (std::is_pointer_v<T>) {
                assert(typeid(std::decay_t<std::remove_pointer_t<T>>) == *type);
                if (typeid(std::decay_t<std::remove_pointer_t<T>>) != *type) return nullptr;
                if (hasPointer) {
                    return std::any_cast<std::add_pointer_t<std::decay_t<std::remove_pointer_t<T>>>>(variable);
                }else {
                    return &std::any_cast<std::add_lvalue_reference_t<std::remove_pointer_t<T>>>(variable);
                }
            } else if constexpr (std::is_reference_v<T>) {
                assert(typeid(std::decay_t<T>) == *type);
                if (hasPointer) {
                    return *std::any_cast<std::add_pointer_t<std::decay_t<T>>>(variable);
                } else {
                    return std::any_cast<T>(variable);
                }
            } else {
                assert(typeid(std::decay_t<T>) == *type);
                if (hasPointer) {
                    return *std::any_cast<std::add_pointer_t<std::decay_t<T>>>(variable);
                } else {
                    return std::any_cast<std::decay_t<T>>(variable);
                }
            }
        }
        template <typename T>
        decltype(auto) get() const {
            if constexpr (std::is_pointer_v<T>){
                return static_cast<std::add_pointer_t<std::add_const_t<std::remove_pointer_t<T>>>>((const_cast<Variable &>(*this)).get<T>());
            } else if constexpr (std::is_lvalue_reference_v<T>) {
                return static_cast<std::add_lvalue_reference_t<std::add_const_t<std::decay_t<T>>>>((const_cast<Variable &>(*this)).get<T>());
            } else if constexpr (std::is_rvalue_reference_v<T>) {
                return static_cast<std::add_rvalue_reference_t<std::add_const_t<std::decay_t<T>>>>((const_cast<Variable &>(*this)).get<T>());
            } else {
                return (const_cast<Variable &>(*this)).get<T>();
            }
        }
        template <typename T>
        void set(T&& val) {
            variable = std::forward<T>(val);
            hasPointer = std::is_pointer_v<T>;
            type = &typeid(std::remove_pointer_t<std::remove_cv_t<T>>);
        }
        const std::type_info& getType()const noexcept{return *type;}
    public:
        template <typename T,
                  typename = std::enable_if_t<
                         !std::is_same_v<
                                 Variable,
                                 std::decay_t<T>
                         >
                  >
        >
        explicit Variable(T&& val):
                hasPointer(std::is_pointer_v<T>),
                variable(std::forward<T>(val)),
                type(&typeid(std::remove_pointer_t<std::remove_cv_t<T>>)){}

        Variable(): hasPointer(false), variable(), type(&typeid(void)){};
        ~Variable() = default;
        Variable(const Variable&) = default;
        Variable(Variable&&) = default;
        Variable& operator=(const Variable&) = default;
        Variable& operator=(Variable&&) = default;
    };
}

#endif
