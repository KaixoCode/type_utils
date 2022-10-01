#pragma once
#include "helpers.hpp"

/**
 * Modify the qualifiers of a type.
 */
namespace kaixo {
    template<class To, class From>
    struct add_ref {
        using _lvalue = std::conditional_t<std::is_lvalue_reference_v<From>, std::add_lvalue_reference_t<To>, To>;
        using _rvalue = std::conditional_t<std::is_rvalue_reference_v<From>, std::add_rvalue_reference_t<_lvalue>, _lvalue>;
        using type = typename _rvalue;
    };

    template<class From, class ...Tys>
    struct add_ref<info<Tys...>, From> {
        using type = info<typename add_ref<Tys, From>::type...>;
    };

    template<class To, class From>
    struct copy_ref {
        using type = add_ref<std::decay_t<To>, From>::type;
    };

    template<class From, class ...Tys>
    struct copy_ref<info<Tys...>, From> {
        using type = info<typename copy_ref<Tys, From>::type...>;
    };

    template<class To, class From = const int>
    struct add_const {
        using _unrefFrom = std::remove_reference_t<From>;
        using _unrefTo = std::remove_reference_t<To>;
        using _const = std::conditional_t<std::is_const_v<_unrefFrom>, typename add_ref<std::add_const_t<_unrefTo>, To>::type, To>;
        using type = _const;
    };

    template<class From, class ...Tys>
    struct add_const<info<Tys...>, From> {
        using type = info<typename add_const<Tys, From>::type...>;
    };

    template<class To, class From = volatile int>
    struct add_volatile {
        using _unrefFrom = std::remove_reference_t<From>;
        using _unrefTo = std::remove_reference_t<To>;
        using _volatile = std::conditional_t<std::is_volatile_v<_unrefFrom>, typename add_ref<std::add_volatile_t<To>, To>::type, To>;
        using type = _volatile;
    };

    template<class From, class ...Tys>
    struct add_volatile<info<Tys...>, From> {
        using type = info<typename add_volatile<Tys, From>::type...>;
    };

    template<class To, class From = const volatile int>
    struct add_cv {
        using type = typename add_volatile<typename add_const<To, From>::type, From>::type;
    };

    template<class From, class ...Tys>
    struct add_cv<info<Tys...>, From> {
        using type = info<typename add_cv<Tys, From>::type...>;
    };

    template<class To, class From>
    struct copy_const {
        using type = add_ref<typename add_const<std::remove_const_t<std::remove_reference_t<To>>, From>::type, To>::type;
    };

    template<class From, class ...Tys>
    struct copy_const<info<Tys...>, From> {
        using type = info<typename copy_const<Tys, From>::type...>;
    };

    template<class To, class From>
    struct copy_volatile {
        using type = add_ref<typename add_volatile<std::remove_volatile_t<std::remove_reference_t<To>>, From>::type, To>::type;
    };

    template<class From, class ...Tys>
    struct copy_volatile<info<Tys...>, From> {
        using type = info<typename copy_volatile<Tys, From>::type...>;
    };

    template<class To, class From>
    struct copy_cv {
        using type = add_ref<typename add_cv<std::remove_cv_t<std::remove_reference_t<To>>, From>::type, To>::type;
    };

    template<class From, class ...Tys>
    struct copy_cv<info<Tys...>, From> {
        using type = info<typename copy_cv<Tys, From>::type...>;
    };

    template<class To, class From>
    struct add_cvref {
        using _cv = typename add_cv<To, From>::type;
        using _lvalue = std::conditional_t<std::is_lvalue_reference_v<From>, std::add_lvalue_reference_t<_cv>, _cv>;
        using _rvalue = std::conditional_t<std::is_rvalue_reference_v<From>, std::add_rvalue_reference_t<_lvalue>, _lvalue>;
        using type = typename _rvalue;
    };

    template<class From, class ...Tys>
    struct add_cvref<info<Tys...>, From> {
        using type = info<typename add_cvref<Tys, From>::type...>;
    };

    template<class To, class From>
    struct copy_cvref {
        using type = add_cvref<std::decay_t<To>, From>::type;
    };

    template<class From, class ...Tys>
    struct copy_cvref<info<Tys...>, From> {
        using type = info<typename copy_cvref<Tys, From>::type...>;
    };

    template<class To, class From>
    using copy_const_t = typename copy_const<To, From>::type;
    template<class To, class From>
    using copy_volatile_t = typename copy_volatile<To, From>::type;
    template<class To, class From>
    using copy_cv_t = typename copy_cv<To, From>::type;
    template<class To, class From>
    using copy_ref_t = typename copy_ref<To, From>::type;
    template<class To, class From>
    using copy_cvref_t = typename copy_cvref<To, From>::type;
    template<class To, class From = const int>
    using add_const_t = typename add_const<To, From>::type;
    template<class To, class From = volatile int>
    using add_volatile_t = typename add_volatile<To, From>::type;
    template<class To, class From = const volatile int>
    using add_cv_t = typename add_cv<To, From>::type;
    template<class To, class From>
    using add_ref_t = typename add_ref<To, From>::type;
    template<class To, class From>
    using add_cvref_t = typename add_cvref<To, From>::type;

    template<class To>
    struct remove_const {
        using type = typename copy_const<To, int>::type;
    };

    template<class To>
    using remove_const_t = typename remove_const<To>::type;

    template<class To>
    struct remove_volatile {
        using type = typename copy_volatile<To, int>::type;
    };

    template<class To>
    using remove_volatile_t = typename remove_volatile<To>::type;

    template<class To>
    struct remove_cv {
        using type = typename copy_cv<To, int>::type;
    };

    template<class To>
    using remove_cv_t = typename remove_cv<To>::type;

    template<class Ty>
    struct decay {
        using type = std::decay_t<Ty>;
    };

    template<class ...Tys>
    struct decay<info<Tys...>> {
        using type = info<typename decay<Tys>::type...>;
    };

    template<class Ty>
    using decay_t = typename decay<Ty>::type;

    template<class Ty>
    struct remove_reference {
        using type = std::remove_reference_t<Ty>;
    };

    template<class ...Tys>
    struct remove_reference<info<Tys...>> {
        using type = info<typename remove_reference<Tys>::type...>;
    };

    template<class Ty>
    using remove_reference_t = typename remove_reference<Ty>::type;

    template<class Ty>
    struct remove_cvref {
        using type = std::remove_cvref_t<Ty>;
    };

    template<class ...Tys>
    struct remove_cvref<info<Tys...>> {
        using type = info<typename remove_cvref<Tys>::type...>;
    };

    template<class Ty>
    using remove_cvref_t = typename remove_cvref<Ty>::type;

    template<class Ty>
    struct add_lvalue_reference {
        using type = std::add_lvalue_reference_t<Ty>;
    };

    template<class ...Tys>
    struct add_lvalue_reference<info<Tys...>> {
        using type = info<typename add_lvalue_reference<Tys>::type...>;
    };

    template<class Ty>
    using add_lvalue_reference_t = typename add_lvalue_reference<Ty>::type;

    template<class Ty>
    struct add_rvalue_reference {
        using type = std::add_rvalue_reference_t<Ty>;
    };

    template<class ...Tys>
    struct add_rvalue_reference<info<Tys...>> {
        using type = info<typename add_rvalue_reference<Tys>::type...>;
    };

    template<class Ty>
    using add_rvalue_reference_t = typename add_rvalue_reference<Ty>::type;

    template<class Ty>
    struct remove_pointer {
        using type = std::remove_pointer_t<Ty>;
    };

    template<class ...Tys>
    struct remove_pointer<info<Tys...>> {
        using type = info<typename remove_pointer<Tys>::type...>;
    };

    template<class Ty>
    using remove_pointer_t = typename remove_pointer<Ty>::type;

    template<class Ty>
    struct add_pointer {
        using type = std::add_pointer_t<Ty>;
    };

    template<class ...Tys>
    struct add_pointer<info<Tys...>> {
        using type = info<typename add_pointer<Tys>::type...>;
    };

    template<class Ty>
    using add_pointer_t = typename add_pointer<Ty>::type;
}