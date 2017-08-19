#include <iostream>
#include <tree/tree.hpp>
#include <boost/type_index.hpp>
#include <node/node.hpp>

using namespace gp;

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
    auto progn = std::make_shared<node::PrognNode<int, 2>>();
    auto add = std::make_shared<node::AddNode<int>>();
    auto add1 = std::make_shared<node::AddNode<int>>();
    auto subst = std::make_shared<node::SubstitutionNode<int>>();
    auto lovalVar = std::make_shared<node::LocalVariableNode<int, 0>>();
    auto localVarL = std::make_shared<node::LocalVariableNode<utility::LeftHandValue<int>, 0>>();
    auto a1 = std::make_shared<node::ArgumentNode<int, 0>>();
    auto a2 = std::make_shared<node::ArgumentNode<int, 1>>();
    auto c1 = std::make_shared<node::ConstNode<int>>(10);
    progn->setChild(0, subst);
    progn->setChild(1, add1);
    subst->setChild(0, localVarL);
    subst->setChild(1, a1);
    add1->setChild(0, c1);
    add1->setChild(1, add);
    add->setChild(0, a2);
    add->setChild(1, lovalVar);
//    add->setChild(0, c1);
//    add->setChild(1, c2);
    tree::Tree tree(typeid(int), std::vector<const std::type_info*>{&typeid(int), &typeid(int)}, std::vector<const std::type_info*>{&typeid(int)}, progn);
    auto ans = tree.evaluate(std::vector<utility::Variable>{1, 2});
    std::cout<<std::any_cast<int>(ans.getReturnValue())<<std::endl;
//    std::cout<<ans.getArgument(0).get<int>()<<std::endl;
//    std::cout<<ans.getArgument(1).get<int>()<<std::endl;
//    std::cout<<ans.getLocalVariable(0).get<int>()<<std::endl;

//    func<5, int, double, bool>();

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