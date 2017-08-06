#include <iostream>
#include <cassert>
#include <exception>
#include<tuple>

template <typename T, typename ...Ts>
const std::type_info* const getRTTI(std::size_t n)noexcept {
    if constexpr (sizeof...(Ts) > 0) {
        if(n == 0)return &typeid(T);
        else return getRTTI<Ts...>(n - 1);
    } else {
        if(n != 0) return nullptr;
        else return &typeid(T);
    }
}


int main() {
    return 0;
}