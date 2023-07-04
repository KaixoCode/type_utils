#pragma once
#include "kaixo/type_utils.hpp"

constexpr void functions_tests() {
    using namespace kaixo;
    using namespace kaixo::concepts;

    constexpr auto MyLambda = [](char, int) -> double { return 0.0; };

    struct MyFunctor {
        int a;
        char operator()(const char*, std::size_t) const volatile & noexcept { return ' '; }
    };

    struct MyStruct {
        void fun1() {}
        void fun2() const {}
        void fun3() volatile {}
        void fun4() noexcept {}
        void fun5() const noexcept {}
        void fun6() volatile noexcept {}
        void fun7() const volatile noexcept {}
        void fun8()& {}
        void fun9() const& {}
        void fun10() volatile& {}
        void fun11() & noexcept {}
        void fun12() const & noexcept {}
        void fun13() volatile & noexcept {}
        void fun14() const volatile & noexcept {}
        void fun15()&& {}
        void fun16() const&& {}
        void fun17() volatile&& {}
        void fun18() && noexcept {}
        void fun19() const && noexcept {}
        void fun20() volatile && noexcept {}
        void fun21() const volatile&& noexcept {}
        double fun22(int, double) { return 0.1; };
    };

    static_assert(same_as<info_v<MyLambda>::signature::type, double(char, int) const>);
    static_assert(same_as<info<MyFunctor>::signature::type, char(const char*, std::size_t) const volatile & noexcept>);
    static_assert(same_as<info<MyFunctor>::fun_decay::signature::type, char(const char*, std::size_t)>);
    
    static_assert(same_as<info_v<&MyStruct::fun22>::arguments, info<int, double>>);
    static_assert(same_as<info_v<&MyStruct::fun22>::pointer::type, double(*)(int, double)>);
    static_assert(same_as<info_v<&MyStruct::fun22>::signature::type, double(int, double)>);

    static_assert(void_type<info_v<&MyStruct::fun1>::result::type>);

    static_assert(!info_v<&MyStruct::fun1>::pointer::is_noexcept::value);
    static_assert(info_v<&MyStruct::fun4>::pointer::is_noexcept::value);

    static_assert(info_v<&MyStruct::fun1>::add_fun_const::is_fun_const::value);
    static_assert(info_v<&MyStruct::fun1>::add_fun_volatile::is_fun_volatile::value);
    static_assert(info_v<&MyStruct::fun1>::add_fun_lvalue_reference::is_fun_lvalue_reference::value);
    static_assert(info_v<&MyStruct::fun1>::add_fun_rvalue_reference::is_fun_rvalue_reference::value);
    static_assert(info_v<&MyStruct::fun1>::add_noexcept::is_noexcept::value);

    static_assert(!info_v<&MyStruct::fun14>::remove_fun_const::is_fun_const::value);
    static_assert(!info_v<&MyStruct::fun14>::remove_fun_volatile::is_fun_volatile::value);
    static_assert(!info_v<&MyStruct::fun14>::remove_fun_reference::is_fun_reference::value);
    static_assert(!info_v<&MyStruct::fun14>::remove_noexcept::is_noexcept::value);
    static_assert(same_as<info_v<&MyStruct::fun14>::fun_decay::signature::type, void()>);

    static_assert(same_as<info_v<&MyStruct::fun1>::object::type, MyStruct>);
    static_assert(!info_v<&MyStruct::fun1>::is_fun_const::value);
    static_assert(!info_v<&MyStruct::fun1>::is_fun_volatile::value);
    static_assert(!info_v<&MyStruct::fun1>::is_fun_lvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun1>::is_fun_rvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun1>::is_fun_reference::value);
    static_assert(!info_v<&MyStruct::fun1>::is_noexcept::value);
    static_assert(!info_v<&MyStruct::fun1>::signature::is_fun_const::value);
    static_assert(!info_v<&MyStruct::fun1>::signature::is_fun_volatile::value);
    static_assert(!info_v<&MyStruct::fun1>::signature::is_fun_lvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun1>::signature::is_fun_rvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun1>::signature::is_fun_reference::value);
    static_assert(!info_v<&MyStruct::fun1>::signature::is_noexcept::value);

    static_assert(same_as<info_v<&MyStruct::fun2>::object::type, const MyStruct>);
    static_assert( info_v<&MyStruct::fun2>::is_fun_const::value);
    static_assert(!info_v<&MyStruct::fun2>::is_fun_volatile::value);
    static_assert(!info_v<&MyStruct::fun2>::is_fun_lvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun2>::is_fun_rvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun2>::is_fun_reference::value);
    static_assert(!info_v<&MyStruct::fun2>::is_noexcept::value);
    static_assert( info_v<&MyStruct::fun2>::signature::is_fun_const::value);
    static_assert(!info_v<&MyStruct::fun2>::signature::is_fun_volatile::value);
    static_assert(!info_v<&MyStruct::fun2>::signature::is_fun_lvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun2>::signature::is_fun_rvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun2>::signature::is_fun_reference::value);
    static_assert(!info_v<&MyStruct::fun2>::signature::is_noexcept::value);

    static_assert(same_as<info_v<&MyStruct::fun3>::object::type, volatile MyStruct>);
    static_assert(!info_v<&MyStruct::fun3>::is_fun_const::value);
    static_assert( info_v<&MyStruct::fun3>::is_fun_volatile::value);
    static_assert(!info_v<&MyStruct::fun3>::is_fun_lvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun3>::is_fun_rvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun3>::is_fun_reference::value);
    static_assert(!info_v<&MyStruct::fun3>::is_noexcept::value);
    static_assert(!info_v<&MyStruct::fun3>::signature::is_fun_const::value);
    static_assert( info_v<&MyStruct::fun3>::signature::is_fun_volatile::value);
    static_assert(!info_v<&MyStruct::fun3>::signature::is_fun_lvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun3>::signature::is_fun_rvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun3>::signature::is_fun_reference::value);
    static_assert(!info_v<&MyStruct::fun3>::signature::is_noexcept::value);

    static_assert(same_as<info_v<&MyStruct::fun4>::object::type, MyStruct>);
    static_assert(!info_v<&MyStruct::fun4>::is_fun_const::value);
    static_assert(!info_v<&MyStruct::fun4>::is_fun_volatile::value);
    static_assert(!info_v<&MyStruct::fun4>::is_fun_lvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun4>::is_fun_rvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun4>::is_fun_reference::value);
    static_assert( info_v<&MyStruct::fun4>::is_noexcept::value);
    static_assert(!info_v<&MyStruct::fun4>::signature::is_fun_const::value);
    static_assert(!info_v<&MyStruct::fun4>::signature::is_fun_volatile::value);
    static_assert(!info_v<&MyStruct::fun4>::signature::is_fun_lvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun4>::signature::is_fun_rvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun4>::signature::is_fun_reference::value);
    static_assert( info_v<&MyStruct::fun4>::signature::is_noexcept::value);

    static_assert(same_as<info_v<&MyStruct::fun5>::object::type, const MyStruct>);
    static_assert( info_v<&MyStruct::fun5>::is_fun_const::value);
    static_assert(!info_v<&MyStruct::fun5>::is_fun_volatile::value);
    static_assert(!info_v<&MyStruct::fun5>::is_fun_lvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun5>::is_fun_rvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun5>::is_fun_reference::value);
    static_assert( info_v<&MyStruct::fun5>::is_noexcept::value);
    static_assert( info_v<&MyStruct::fun5>::signature::is_fun_const::value);
    static_assert(!info_v<&MyStruct::fun5>::signature::is_fun_volatile::value);
    static_assert(!info_v<&MyStruct::fun5>::signature::is_fun_lvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun5>::signature::is_fun_rvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun5>::signature::is_fun_reference::value);
    static_assert( info_v<&MyStruct::fun5>::signature::is_noexcept::value);

    static_assert(same_as<info_v<&MyStruct::fun6>::object::type, volatile MyStruct>);
    static_assert(!info_v<&MyStruct::fun6>::is_fun_const::value);
    static_assert( info_v<&MyStruct::fun6>::is_fun_volatile::value);
    static_assert(!info_v<&MyStruct::fun6>::is_fun_lvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun6>::is_fun_rvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun6>::is_fun_reference::value);
    static_assert( info_v<&MyStruct::fun6>::is_noexcept::value);
    static_assert(!info_v<&MyStruct::fun6>::signature::is_fun_const::value);
    static_assert( info_v<&MyStruct::fun6>::signature::is_fun_volatile::value);
    static_assert(!info_v<&MyStruct::fun6>::signature::is_fun_lvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun6>::signature::is_fun_rvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun6>::signature::is_fun_reference::value);
    static_assert( info_v<&MyStruct::fun6>::signature::is_noexcept::value);

    static_assert(same_as<info_v<&MyStruct::fun7>::object::type, const volatile MyStruct>);
    static_assert( info_v<&MyStruct::fun7>::is_fun_const::value);
    static_assert( info_v<&MyStruct::fun7>::is_fun_volatile::value);
    static_assert(!info_v<&MyStruct::fun7>::is_fun_lvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun7>::is_fun_rvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun7>::is_fun_reference::value);
    static_assert( info_v<&MyStruct::fun7>::is_noexcept::value);
    static_assert( info_v<&MyStruct::fun7>::signature::is_fun_const::value);
    static_assert( info_v<&MyStruct::fun7>::signature::is_fun_volatile::value);
    static_assert(!info_v<&MyStruct::fun7>::signature::is_fun_lvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun7>::signature::is_fun_rvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun7>::signature::is_fun_reference::value);
    static_assert( info_v<&MyStruct::fun7>::signature::is_noexcept::value);

    static_assert(same_as<info_v<&MyStruct::fun8>::object::type, MyStruct&>);
    static_assert(!info_v<&MyStruct::fun8>::is_fun_const::value);
    static_assert(!info_v<&MyStruct::fun8>::is_fun_volatile::value);
    static_assert( info_v<&MyStruct::fun8>::is_fun_lvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun8>::is_fun_rvalue_reference::value);
    static_assert( info_v<&MyStruct::fun8>::is_fun_reference::value);
    static_assert(!info_v<&MyStruct::fun8>::is_noexcept::value);
    static_assert(!info_v<&MyStruct::fun8>::signature::is_fun_const::value);
    static_assert(!info_v<&MyStruct::fun8>::signature::is_fun_volatile::value);
    static_assert( info_v<&MyStruct::fun8>::signature::is_fun_lvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun8>::signature::is_fun_rvalue_reference::value);
    static_assert( info_v<&MyStruct::fun8>::signature::is_fun_reference::value);
    static_assert(!info_v<&MyStruct::fun8>::signature::is_noexcept::value);

    static_assert(same_as<info_v<&MyStruct::fun9>::object::type, const MyStruct&>);
    static_assert( info_v<&MyStruct::fun9>::is_fun_const::value);
    static_assert(!info_v<&MyStruct::fun9>::is_fun_volatile::value);
    static_assert( info_v<&MyStruct::fun9>::is_fun_lvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun9>::is_fun_rvalue_reference::value);
    static_assert( info_v<&MyStruct::fun9>::is_fun_reference::value);
    static_assert(!info_v<&MyStruct::fun9>::is_noexcept::value);
    static_assert( info_v<&MyStruct::fun9>::signature::is_fun_const::value);
    static_assert(!info_v<&MyStruct::fun9>::signature::is_fun_volatile::value);
    static_assert( info_v<&MyStruct::fun9>::signature::is_fun_lvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun9>::signature::is_fun_rvalue_reference::value);
    static_assert( info_v<&MyStruct::fun9>::signature::is_fun_reference::value);
    static_assert(!info_v<&MyStruct::fun9>::signature::is_noexcept::value);

    static_assert(same_as<info_v<&MyStruct::fun10>::object::type, volatile MyStruct&>);
    static_assert(!info_v<&MyStruct::fun10>::is_fun_const::value);
    static_assert( info_v<&MyStruct::fun10>::is_fun_volatile::value);
    static_assert( info_v<&MyStruct::fun10>::is_fun_lvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun10>::is_fun_rvalue_reference::value);
    static_assert( info_v<&MyStruct::fun10>::is_fun_reference::value);
    static_assert(!info_v<&MyStruct::fun10>::is_noexcept::value);
    static_assert(!info_v<&MyStruct::fun10>::signature::is_fun_const::value);
    static_assert( info_v<&MyStruct::fun10>::signature::is_fun_volatile::value);
    static_assert( info_v<&MyStruct::fun10>::signature::is_fun_lvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun10>::signature::is_fun_rvalue_reference::value);
    static_assert( info_v<&MyStruct::fun10>::signature::is_fun_reference::value);
    static_assert(!info_v<&MyStruct::fun10>::signature::is_noexcept::value);

    static_assert(same_as<info_v<&MyStruct::fun11>::object::type, MyStruct&>);
    static_assert(!info_v<&MyStruct::fun11>::is_fun_const::value);
    static_assert(!info_v<&MyStruct::fun11>::is_fun_volatile::value);
    static_assert( info_v<&MyStruct::fun11>::is_fun_lvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun11>::is_fun_rvalue_reference::value);
    static_assert( info_v<&MyStruct::fun11>::is_fun_reference::value);
    static_assert( info_v<&MyStruct::fun11>::is_noexcept::value);
    static_assert(!info_v<&MyStruct::fun11>::signature::is_fun_const::value);
    static_assert(!info_v<&MyStruct::fun11>::signature::is_fun_volatile::value);
    static_assert( info_v<&MyStruct::fun11>::signature::is_fun_lvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun11>::signature::is_fun_rvalue_reference::value);
    static_assert( info_v<&MyStruct::fun11>::signature::is_fun_reference::value);
    static_assert( info_v<&MyStruct::fun11>::signature::is_noexcept::value);

    static_assert(same_as<info_v<&MyStruct::fun12>::object::type, const MyStruct&>);
    static_assert( info_v<&MyStruct::fun12>::is_fun_const::value);
    static_assert(!info_v<&MyStruct::fun12>::is_fun_volatile::value);
    static_assert( info_v<&MyStruct::fun12>::is_fun_lvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun12>::is_fun_rvalue_reference::value);
    static_assert( info_v<&MyStruct::fun12>::is_fun_reference::value);
    static_assert( info_v<&MyStruct::fun12>::is_noexcept::value);
    static_assert( info_v<&MyStruct::fun12>::signature::is_fun_const::value);
    static_assert(!info_v<&MyStruct::fun12>::signature::is_fun_volatile::value);
    static_assert( info_v<&MyStruct::fun12>::signature::is_fun_lvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun12>::signature::is_fun_rvalue_reference::value);
    static_assert( info_v<&MyStruct::fun12>::signature::is_fun_reference::value);
    static_assert( info_v<&MyStruct::fun12>::signature::is_noexcept::value);

    static_assert(same_as<info_v<&MyStruct::fun13>::object::type, volatile MyStruct&>);
    static_assert(!info_v<&MyStruct::fun13>::is_fun_const::value);
    static_assert( info_v<&MyStruct::fun13>::is_fun_volatile::value);
    static_assert( info_v<&MyStruct::fun13>::is_fun_lvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun13>::is_fun_rvalue_reference::value);
    static_assert( info_v<&MyStruct::fun13>::is_fun_reference::value);
    static_assert( info_v<&MyStruct::fun13>::is_noexcept::value);
    static_assert(!info_v<&MyStruct::fun13>::signature::is_fun_const::value);
    static_assert( info_v<&MyStruct::fun13>::signature::is_fun_volatile::value);
    static_assert( info_v<&MyStruct::fun13>::signature::is_fun_lvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun13>::signature::is_fun_rvalue_reference::value);
    static_assert( info_v<&MyStruct::fun13>::signature::is_fun_reference::value);
    static_assert( info_v<&MyStruct::fun13>::signature::is_noexcept::value);

    static_assert(same_as<info_v<&MyStruct::fun14>::object::type, const volatile MyStruct&>);
    static_assert( info_v<&MyStruct::fun14>::is_fun_const::value);
    static_assert( info_v<&MyStruct::fun14>::is_fun_volatile::value);
    static_assert( info_v<&MyStruct::fun14>::is_fun_lvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun14>::is_fun_rvalue_reference::value);
    static_assert( info_v<&MyStruct::fun14>::is_fun_reference::value);
    static_assert( info_v<&MyStruct::fun14>::is_noexcept::value);
    static_assert( info_v<&MyStruct::fun14>::signature::is_fun_const::value);
    static_assert( info_v<&MyStruct::fun14>::signature::is_fun_volatile::value);
    static_assert( info_v<&MyStruct::fun14>::signature::is_fun_lvalue_reference::value);
    static_assert(!info_v<&MyStruct::fun14>::signature::is_fun_rvalue_reference::value);
    static_assert( info_v<&MyStruct::fun14>::signature::is_fun_reference::value);
    static_assert( info_v<&MyStruct::fun14>::signature::is_noexcept::value);

    static_assert(same_as<info_v<&MyStruct::fun15>::object::type, MyStruct&&>);
    static_assert(!info_v<&MyStruct::fun15>::is_fun_const::value);
    static_assert(!info_v<&MyStruct::fun15>::is_fun_volatile::value);
    static_assert(!info_v<&MyStruct::fun15>::is_fun_lvalue_reference::value);
    static_assert( info_v<&MyStruct::fun15>::is_fun_rvalue_reference::value);
    static_assert( info_v<&MyStruct::fun15>::is_fun_reference::value);
    static_assert(!info_v<&MyStruct::fun15>::is_noexcept::value);
    static_assert(!info_v<&MyStruct::fun15>::signature::is_fun_const::value);
    static_assert(!info_v<&MyStruct::fun15>::signature::is_fun_volatile::value);
    static_assert(!info_v<&MyStruct::fun15>::signature::is_fun_lvalue_reference::value);
    static_assert( info_v<&MyStruct::fun15>::signature::is_fun_rvalue_reference::value);
    static_assert( info_v<&MyStruct::fun15>::signature::is_fun_reference::value);
    static_assert(!info_v<&MyStruct::fun15>::signature::is_noexcept::value);

    static_assert(same_as<info_v<&MyStruct::fun16>::object::type, const MyStruct&&>);
    static_assert( info_v<&MyStruct::fun16>::is_fun_const::value);
    static_assert(!info_v<&MyStruct::fun16>::is_fun_volatile::value);
    static_assert(!info_v<&MyStruct::fun16>::is_fun_lvalue_reference::value);
    static_assert( info_v<&MyStruct::fun16>::is_fun_rvalue_reference::value);
    static_assert( info_v<&MyStruct::fun16>::is_fun_reference::value);
    static_assert(!info_v<&MyStruct::fun16>::is_noexcept::value);
    static_assert( info_v<&MyStruct::fun16>::signature::is_fun_const::value);
    static_assert(!info_v<&MyStruct::fun16>::signature::is_fun_volatile::value);
    static_assert(!info_v<&MyStruct::fun16>::signature::is_fun_lvalue_reference::value);
    static_assert( info_v<&MyStruct::fun16>::signature::is_fun_rvalue_reference::value);
    static_assert( info_v<&MyStruct::fun16>::signature::is_fun_reference::value);
    static_assert(!info_v<&MyStruct::fun16>::signature::is_noexcept::value);

    static_assert(same_as<info_v<&MyStruct::fun17>::object::type, volatile MyStruct&&>);
    static_assert(!info_v<&MyStruct::fun17>::is_fun_const::value);
    static_assert( info_v<&MyStruct::fun17>::is_fun_volatile::value);
    static_assert(!info_v<&MyStruct::fun17>::is_fun_lvalue_reference::value);
    static_assert( info_v<&MyStruct::fun17>::is_fun_rvalue_reference::value);
    static_assert( info_v<&MyStruct::fun17>::is_fun_reference::value);
    static_assert(!info_v<&MyStruct::fun17>::is_noexcept::value);
    static_assert(!info_v<&MyStruct::fun17>::signature::is_fun_const::value);
    static_assert( info_v<&MyStruct::fun17>::signature::is_fun_volatile::value);
    static_assert(!info_v<&MyStruct::fun17>::signature::is_fun_lvalue_reference::value);
    static_assert( info_v<&MyStruct::fun17>::signature::is_fun_rvalue_reference::value);
    static_assert( info_v<&MyStruct::fun17>::signature::is_fun_reference::value);
    static_assert(!info_v<&MyStruct::fun17>::signature::is_noexcept::value);

    static_assert(same_as<info_v<&MyStruct::fun18>::object::type, MyStruct&&>);
    static_assert(!info_v<&MyStruct::fun18>::is_fun_const::value);
    static_assert(!info_v<&MyStruct::fun18>::is_fun_volatile::value);
    static_assert(!info_v<&MyStruct::fun18>::is_fun_lvalue_reference::value);
    static_assert( info_v<&MyStruct::fun18>::is_fun_rvalue_reference::value);
    static_assert( info_v<&MyStruct::fun18>::is_fun_reference::value);
    static_assert( info_v<&MyStruct::fun18>::is_noexcept::value);
    static_assert(!info_v<&MyStruct::fun18>::signature::is_fun_const::value);
    static_assert(!info_v<&MyStruct::fun18>::signature::is_fun_volatile::value);
    static_assert(!info_v<&MyStruct::fun18>::signature::is_fun_lvalue_reference::value);
    static_assert( info_v<&MyStruct::fun18>::signature::is_fun_rvalue_reference::value);
    static_assert( info_v<&MyStruct::fun18>::signature::is_fun_reference::value);
    static_assert( info_v<&MyStruct::fun18>::signature::is_noexcept::value);

    static_assert(same_as<info_v<&MyStruct::fun19>::object::type, const MyStruct&&>);
    static_assert( info_v<&MyStruct::fun19>::is_fun_const::value);
    static_assert(!info_v<&MyStruct::fun19>::is_fun_volatile::value);
    static_assert(!info_v<&MyStruct::fun19>::is_fun_lvalue_reference::value);
    static_assert( info_v<&MyStruct::fun19>::is_fun_rvalue_reference::value);
    static_assert( info_v<&MyStruct::fun19>::is_fun_reference::value);
    static_assert( info_v<&MyStruct::fun19>::is_noexcept::value);
    static_assert( info_v<&MyStruct::fun19>::signature::is_fun_const::value);
    static_assert(!info_v<&MyStruct::fun19>::signature::is_fun_volatile::value);
    static_assert(!info_v<&MyStruct::fun19>::signature::is_fun_lvalue_reference::value);
    static_assert( info_v<&MyStruct::fun19>::signature::is_fun_rvalue_reference::value);
    static_assert( info_v<&MyStruct::fun19>::signature::is_fun_reference::value);
    static_assert( info_v<&MyStruct::fun19>::signature::is_noexcept::value);

    static_assert(same_as<info_v<&MyStruct::fun20>::object::type, volatile MyStruct&&>);
    static_assert(!info_v<&MyStruct::fun20>::is_fun_const::value);
    static_assert( info_v<&MyStruct::fun20>::is_fun_volatile::value);
    static_assert(!info_v<&MyStruct::fun20>::is_fun_lvalue_reference::value);
    static_assert( info_v<&MyStruct::fun20>::is_fun_rvalue_reference::value);
    static_assert( info_v<&MyStruct::fun20>::is_fun_reference::value);
    static_assert( info_v<&MyStruct::fun20>::is_noexcept::value);
    static_assert(!info_v<&MyStruct::fun20>::signature::is_fun_const::value);
    static_assert( info_v<&MyStruct::fun20>::signature::is_fun_volatile::value);
    static_assert(!info_v<&MyStruct::fun20>::signature::is_fun_lvalue_reference::value);
    static_assert( info_v<&MyStruct::fun20>::signature::is_fun_rvalue_reference::value);
    static_assert( info_v<&MyStruct::fun20>::signature::is_fun_reference::value);
    static_assert( info_v<&MyStruct::fun20>::signature::is_noexcept::value);

    static_assert(same_as<info_v<&MyStruct::fun21>::object::type, const volatile MyStruct&&>);
    static_assert( info_v<&MyStruct::fun21>::is_fun_const::value);
    static_assert( info_v<&MyStruct::fun21>::is_fun_volatile::value);
    static_assert(!info_v<&MyStruct::fun21>::is_fun_lvalue_reference::value);
    static_assert( info_v<&MyStruct::fun21>::is_fun_rvalue_reference::value);
    static_assert( info_v<&MyStruct::fun21>::is_fun_reference::value);
    static_assert( info_v<&MyStruct::fun21>::is_noexcept::value);
    static_assert( info_v<&MyStruct::fun21>::signature::is_fun_const::value);
    static_assert( info_v<&MyStruct::fun21>::signature::is_fun_volatile::value);
    static_assert(!info_v<&MyStruct::fun21>::signature::is_fun_lvalue_reference::value);
    static_assert( info_v<&MyStruct::fun21>::signature::is_fun_rvalue_reference::value);
    static_assert( info_v<&MyStruct::fun21>::signature::is_fun_reference::value);
    static_assert( info_v<&MyStruct::fun21>::signature::is_noexcept::value);
}