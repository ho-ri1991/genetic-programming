#ifndef GP_UTILITY_DEFAULT_INITIALIZER
#define GP_UTILITY_DEFAULT_INITIALIZER

#include <type_traits>

namespace gp::utility {
    //(partial) specialize this class if you want to use other default value
    template <typename T>
    struct DefaultInitializer {
        static T getDefaultValue(){return T{};}
    };

    template <typename T>
    T getDefaultValue(){return DefaultInitializer<T>::getDefaultValue();}
}

#endif
