#pragma once
#include "kaixo/type_utils.hpp"

constexpr void concepts_tests() {
    using namespace kaixo;
    using namespace kaixo::concepts;

    struct MyFunctor {
        void operator()(int, double) {}
    };

    struct MyNoThrowFunctor {
        void operator()(int, double) noexcept {}
    };

    using MyLambda = decltype([](int, double) {});

    class MyClass { public: MyClass(int, double) {} };
    class MyNoThrowClass { public: MyNoThrowClass(int, double) noexcept {} };
    struct MyTrivialClass { int a; double b; };

    static_assert(functor<MyFunctor>);
    static_assert(functor<MyNoThrowFunctor>);
    static_assert(functor<MyLambda>);

    static_assert(constructible<MyClass, int, double>);
    static_assert(constructible<MyClass, info<int, double>>);
    static_assert(!trivially_constructible<MyClass, int, double>);
    static_assert(!trivially_constructible<MyClass, info<int, double>>);
    static_assert(!nothrow_constructible<MyClass, int, double>);
    static_assert(!nothrow_constructible<MyClass, info<int, double>>);

    static_assert(constructible<MyNoThrowClass, int, double>);
    static_assert(constructible<MyNoThrowClass, info<int, double>>);
    static_assert(!trivially_constructible<MyNoThrowClass, int, double>);
    static_assert(!trivially_constructible<MyNoThrowClass, info<int, double>>);
    static_assert(nothrow_constructible<MyNoThrowClass, int, double>);
    static_assert(nothrow_constructible<MyNoThrowClass, info<int, double>>);
    
    static_assert(constructible<MyTrivialClass, int, double>);
    static_assert(constructible<MyTrivialClass, info<int, double>>);
    static_assert(trivially_constructible<MyTrivialClass, int, double>);
    static_assert(trivially_constructible<MyTrivialClass, info<int, double>>);
    static_assert(nothrow_constructible<MyTrivialClass, int, double>);
    static_assert(nothrow_constructible<MyTrivialClass, info<int, double>>);

    static_assert(invocable<MyFunctor, int, double>);
    static_assert(invocable<MyFunctor, info<int, double>>);
    static_assert(!nothrow_invocable<MyFunctor, int, double>);
    static_assert(!nothrow_invocable<MyFunctor, info<int, double>>);

    static_assert(invocable<MyNoThrowFunctor, int, double>);
    static_assert(invocable<MyNoThrowFunctor, info<int, double>>);
    static_assert(nothrow_invocable<MyNoThrowFunctor, int, double>);
    static_assert(nothrow_invocable<MyNoThrowFunctor, info<int, double>>);

    static_assert(specialization<std::tuple<int>, std::tuple>);
    static_assert(specialization<std::tuple<int, double>, std::tuple>);
    static_assert(!specialization<std::tuple<int>, std::pair>);
    static_assert(!specialization<std::pair<int, double>, std::tuple>);

    static_assert(structured_binding<std::tuple<int, double>>); // Explicit bindings
    static_assert(structured_binding<MyTrivialClass>); // Implicit bindings (aggregate)
    static_assert(!structured_binding<MyClass>); // No bindings

    static_assert(const_type<const int&>);       // My version does check behind references!
    static_assert(volatile_type<volatile int&>);
    static_assert(!std::is_const_v<const int&>); // standard version does not.
    static_assert(!std::is_volatile_v<volatile int&>); 
}