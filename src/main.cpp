#include <iostream>
#include <cassert>
#include <exception>
#include <tuple>
#include <utility/evaluation_context.hpp>
#include <node/node_base.hpp>
#include <tree/tree.hpp>
#include <node/argument_node.hpp>
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
    auto c1 = std::make_shared<node::ArgumentNode<int, 0>>();
    auto c2 = std::make_shared<node::ArgumentNode<int, 1>>();
    add->setChild(0, c1);
    add->setChild(1, c2);
    tree::Tree tree(typeid(int), std::vector<const std::type_info*>{&typeid(int), &typeid(int)}, std::vector<const std::type_info*>{}, add);
    auto ans = tree.evaluate(std::vector<utility::Variable>{1, 2});
    std::cout<<std::any_cast<int>(ans.getReturnValue())<<std::endl;
    std::cout<<c1->getNodeName()<<std::endl;
    func<5, int, double, bool>();

    utility::Variable v(1);
    std::cout<< v.get<int>() << std::endl;
    v.get<int&>() = 2;
    std::cout<< v.get<int>() << std::endl;
    *v.get<int*>() = 3;
    std::cout<< v.get<const int&>() <<std::endl;
    std::unique_ptr<int> p = std::make_unique<int>(10);
    utility::Variable v1(p.get());
    std::cout<< v1.get<int>() <<std::endl;
    *v1.get<int*>() = 50;
    std::cout<< v1.get<const int&>() <<std::endl;

    return 0;
}