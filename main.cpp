#include <iostream>
#include <cassert>
#include <exception>
#include<tuple>
#include "tree/node/node_base.hpp"

class Add: public tree::node::NodeBase<int(int, int)> {
public:
    int evaluate()override {
        return std::get<0>(this->children)->evaluate()+std::get<1>(this->children)->evaluate();
    }
    std::string getNodeName()const override {return "Add";}
};

class Const: public tree::node::NodeBase<int(void)> {
private:
    int x = 0;
public:
    int evaluate()override { return x; }
    std::string getNodeName()const override {return "Const";}
public:
    Const(int x_):x(x_){}
};

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
    auto add = std::make_shared<Add>();
    auto c1 = std::make_shared<Const>(1);
    auto c2 = std::make_shared<Const>(9);
    add->setChild(0, c1);
    add->setChild(1, c2);
    std::cout<<add->evaluate()<<std::endl;
    return 0;
}