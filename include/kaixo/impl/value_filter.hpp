#pragma once
#include <type_traits>
#include <typeinfo>
#include <concepts>
#include <array>
#include <utility>
#include <string_view>
#include <cmath>
#include <algorithm>
#include <tuple>

namespace kaixo {

    /**
     * Value filter can be used when creating a constexpr filter
     * based on some value.
     * @tparam Val unevaluated value
     */
    template<template<class ...> class Val>
    struct value_filter {
        using is_value_filter = int;
        template<class ...Tys>
        consteval auto evaluate() const { return Val<Tys...>::value; }
    };

    template<class Ty>
    concept is_value_filter = requires (Ty) { typename std::decay_t<Ty>::is_value_filter; };

    template<class ...As>
    concept valid_value_filter_op = (is_value_filter<As> || ...);

    template<class ...Tys, class A>
    consteval decltype(auto) _vfv(A& val) {
        if constexpr (is_value_filter<A>) return val.template evaluate<Tys...>();
        else return val;
    }

#define KAIXO_VALUE_FILTER_OP(name, op)                                           \
    template<class A, class B>                                                    \
    struct name##_value_filter {                                                  \
        using is_value_filter = int;                                              \
        [[no_unique_address]] A a;                                                \
        [[no_unique_address]] B b;                                                \
                                                                                  \
        template<class ...Tys>                                                    \
        consteval decltype(auto) evaluate() const {                               \
            return _vfv<Tys...>(a) op _vfv<Tys...>(b);                            \
        }                                                                         \
    };                                                                            \
                                                                                  \
    template<class A, class B>                                                    \
        requires valid_value_filter_op<A, B>                                      \
    consteval auto operator op(A&& a, B&& b) {                                    \
        return name##_value_filter{ std::forward<A>(a), std::forward<B>(b) };     \
    }

    KAIXO_VALUE_FILTER_OP(add, +);
    KAIXO_VALUE_FILTER_OP(subtract, -);
    KAIXO_VALUE_FILTER_OP(multiply, *);
    KAIXO_VALUE_FILTER_OP(divide, / );
    KAIXO_VALUE_FILTER_OP(modulo, %);
    KAIXO_VALUE_FILTER_OP(less_than, < );
    KAIXO_VALUE_FILTER_OP(less_or_equal, <= );
    KAIXO_VALUE_FILTER_OP(greater_than, > );
    KAIXO_VALUE_FILTER_OP(greater_or_equal, >= );
    KAIXO_VALUE_FILTER_OP(equal, == );
    KAIXO_VALUE_FILTER_OP(not_equal, != );
    KAIXO_VALUE_FILTER_OP(left_shift, << );
    KAIXO_VALUE_FILTER_OP(right_shift, >> );
    KAIXO_VALUE_FILTER_OP(boolean_and, &&);
    KAIXO_VALUE_FILTER_OP(boolean_or, || );
    KAIXO_VALUE_FILTER_OP(bitwise_and, &);
    KAIXO_VALUE_FILTER_OP(bitwise_or, | );
    KAIXO_VALUE_FILTER_OP(bitwise_xor, ^);
    KAIXO_VALUE_FILTER_OP(spaceship, <=> );
    KAIXO_VALUE_FILTER_OP(add_assign, +=);
    KAIXO_VALUE_FILTER_OP(subtract_assign, -=);
    KAIXO_VALUE_FILTER_OP(multiply_assign, *=);
    KAIXO_VALUE_FILTER_OP(divide_assign, /=);
    KAIXO_VALUE_FILTER_OP(modulo_assign, %=);
    KAIXO_VALUE_FILTER_OP(left_shift_assign, <<=);
    KAIXO_VALUE_FILTER_OP(right_shift_assign, >>=);
    KAIXO_VALUE_FILTER_OP(and_assign, &=);
    KAIXO_VALUE_FILTER_OP(or_assign, |=);
    KAIXO_VALUE_FILTER_OP(xor_assign, ^=);
#undef KAIXO_VALUE_FILTER_OP
}