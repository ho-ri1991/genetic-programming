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
        template <typename T>
        T get() {
            if constexpr (std::is_pointer_v<T>) {
                assert(typeid(std::remove_pointer_t<std::remove_cv_t <T>>) == *type);
                if (typeid(std::remove_pointer_t<std::remove_cv_t <T>>) != *type) return nullptr;
                if (hasPointer) {
                    return std::any_cast<T>(variable);
                }else {
                    return &std::any_cast<std::add_lvalue_reference_t<std::remove_pointer_t<T>>>(variable);
                }
            } else if constexpr (std::is_reference_v<T>) {
                assert(typeid(std::remove_reference_t<std::remove_cv_t<T>>) == *type);
                if (hasPointer) {
                    return *std::any_cast<std::add_pointer_t<std::remove_cv_t<std::remove_reference_t<T>>>>(variable);
                } else {
                    return std::any_cast<T>(variable);
                }
            } else {
                assert(typeid(std::remove_cv_t<T>) == *type);
                if (hasPointer) {
                    return *std::any_cast<std::add_pointer_t<T>>(variable);
                } else {
                    return std::any_cast<T>(variable);
                }
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
                variable(std::forward<T>(val)),
                hasPointer(std::is_pointer_v<T>),
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
