#ifndef GP_UTILITY_DEFAULT_INITIALIZER
#define GP_UTILITY_DEFAULT_INITIALIZER

#include <type_traits>

namespace gp::utility {
    template <typename T, bool is_arithmetic = std::is_arithmetic_v<T>>
    struct DefaultInitializer {
        static T getDefaultValue(){return T();}
    };

    template <typename T>
    struct DefaultInitializer<T, true> {
        static T getDefaultValue(){return T(0);}
    };

    template <>
    struct DefaultInitializer<bool> {
        static bool getDefaultValue(){return false;}
    };

    template <typename T>
    T getDefaultValue(){return DefaultInitializer<T>::getDefaultValue();}
}

#endif