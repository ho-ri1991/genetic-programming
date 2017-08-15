#include <iostream>
#include <cassert>
#include <exception>
#include <tuple>
#include <utility/evaluation_context.hpp>
#include <node/node_base.hpp>
#include <boost/type_index.hpp>
#include <any>

using namespace gp;

class Add: public node::NodeBase<int(int, int)> {
    using ThisType = Add;
private:
    int evaluationDefinition(utility::EvaluationContext& evaluationContext)const override {
        return std::get<0>(this->children)->evaluate(evaluationContext)+std::get<1>(this->children)->evaluate(evaluationContext);
    }
public:
    std::string getNodeName()const override {return "Add";}
    std::shared_ptr<node::NodeInterface> clone()const override {return std::make_shared<ThisType>();}
};

class Const: public node::NodeBase<int(void)> {
private:
    using ThisType = Const;
    const int x;
private:
    int evaluationDefinition(utility::EvaluationContext&)const override { return x; }
public:
    std::string getNodeName()const override {return "Const";}
    std::shared_ptr<node::NodeInterface> clone()const override {return std::make_shared<ThisType>(x);}
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


template <typename T1, typename T2>
struct type_accumulator {
    using type1 = T1;
    using type2 = T2;
};

template <typename ...Ts>
struct types{
    template <std::size_t n>
    using type = typename std::tuple_element<n, std::tuple<Ts...>>::type;
};

template <typename...> struct join;

template <template <typename...> class Tpl,
        typename ...Args1,
        typename ...Args2>
struct join<Tpl<Args1...>, Tpl<Args2...>>
{
    typedef Tpl<Args1..., Args2...> type;
};

template <std::size_t n, typename ...> struct acc;

template <std::size_t n,
          template <typename ...> class Tpl,
          typename T1,
          typename T2,
          typename ...Args1,
          typename ...Args2,
          typename ...Args>
struct acc<n, Tpl<T1, Args1...>, Tpl<T2, Args2...>, Tpl<Args...>>{
    static void func() {
        if constexpr (n == 0){
            std::cout<< boost::typeindex::type_id_with_cvr<Tpl<T1, Args1...>>().pretty_name() <<std::endl;
        } else {
//            acc<n - 1, Tpl<T2, T1, Args1...>, Tpl<Args...>, Tpl<Args...>>::func();
            acc<n - 1, Tpl<T1, Args1..., T2>, Tpl<Args...>, Tpl<Args...>>::func();
            if constexpr (sizeof...(Args2) > 0) {
                acc<n, Tpl<T1, Args1...>, Tpl<Args2...>, Tpl<Args...>>::func();
            }
        }
    }
};

template <std::size_t n, typename ...Ts>
void func(){
    if constexpr (n == 0){

    }else {
        func<n - 1, Ts...>();
        acc<n, std::tuple<int>, std::tuple<Ts...>, std::tuple<Ts...>>::func();
    }
}

int main() {
    auto add = std::make_shared<Add>();
    auto c1 = std::make_shared<Const>(1);
    auto c2 = std::make_shared<Const>(9);
    add->setChild(0, c1);
    add->setChild(1, c2);
    utility::EvaluationContext evaluationContext(utility::EvaluationContext::VariableTable{}, utility::EvaluationContext::VariableTable{});
    std::cout<<add->evaluate(evaluationContext)<<std::endl;
    typename join<std::tuple<int, int>, std::tuple<double, double>>::type t;
    std::cout<<typeid(typename join<std::tuple<int, int>, std::tuple<double, double>>::type).name()<<std::endl;
    func<5, int, double, bool>();

    std::any a = 1;
    std::cout<<std::any_cast<int>(a)<<std::endl;
    return 0;
}