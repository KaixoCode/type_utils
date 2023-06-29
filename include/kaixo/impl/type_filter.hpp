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
    namespace detail {
        template<class Ty>
        concept is_type_filter = requires (Ty) { typename std::decay_t<Ty>::_is_type_filter; };

        template<class ...As>
        concept valid_type_filter_op = (is_type_filter<As> || ...);

        template<class ...Tys, class A>
        constexpr decltype(auto) _vfv(A& val) {
            if constexpr (detail::is_type_filter<A>)
                return val.template evaluate<Tys...>();
            else return val;
        }

        template<class A, class B>
        struct index_type_filter {
            using _is_type_filter = int;
            [[no_unique_address]] A a;
            [[no_unique_address]] B b;
 
            template<class ...Tys>
            constexpr auto evaluate() const {
                return detail::_vfv<Tys...>(a)[detail::_vfv<Tys...>(b)];
            }
        };
    }

    /**
     * Type filter can be used when creating a constexpr filter based on some
     * criteria that depends on types.
     * @tparam Val unevaluated value
     */
    template<template<class ...> class Val>
    struct type_filter {
        using _is_type_filter = int;

        template<class ...Tys>
        constexpr auto evaluate() const { return Val<Tys...>::value; }

        template<class B>
        constexpr auto operator[](B&& b) const {
            return detail::index_type_filter{ *this, std::forward<B>(b) };
        }
    };

    template<detail::is_type_filter auto Fltr, class ...Args>
    constexpr auto evaluate_type_filter = Fltr.template evaluate<Args...>();

    /**
     * Concept to match a type_trait.
     * @tparam Ty type
     * @tparam V type_trait value
     */
    template<class Ty, detail::is_type_filter auto V>
    concept require = static_cast<bool>(evaluate_type_filter<V, Ty>);

#define KAIXO_TYPE_FILTER_OP(name, op)                                       \
    template<class A, class B>                                               \
    struct name##_type_filter {                                              \
        using _is_type_filter = int;                                         \
        [[no_unique_address]] A a;                                           \
        [[no_unique_address]] B b;                                           \
                                                                             \
        template<class ...Tys>                                               \
        constexpr decltype(auto) evaluate() const {                          \
            return detail::_vfv<Tys...>(a) op detail::_vfv<Tys...>(b);       \
        }                                                                    \
                                                                             \
        template<class B>                                                    \
        constexpr auto operator[](B&& b) const {                             \
            return detail::index_type_filter{ *this, std::forward<B>(b) };   \
        }                                                                    \
    };                                                                       \
                                                                             \
    template<class A, class B>                                               \
        requires detail::valid_type_filter_op<A, B>                          \
    constexpr auto operator op(A&& a, B&& b) {                               \
        return name##_type_filter{ std::forward<A>(a), std::forward<B>(b) }; \
    }

    namespace operators {
        KAIXO_TYPE_FILTER_OP(add, +);
        KAIXO_TYPE_FILTER_OP(subtract, -);
        KAIXO_TYPE_FILTER_OP(multiply, *);
        KAIXO_TYPE_FILTER_OP(divide, / );
        KAIXO_TYPE_FILTER_OP(modulo, %);
        KAIXO_TYPE_FILTER_OP(less_than, < );
        KAIXO_TYPE_FILTER_OP(less_or_equal, <= );
        KAIXO_TYPE_FILTER_OP(greater_than, > );
        KAIXO_TYPE_FILTER_OP(greater_or_equal, >= );
        KAIXO_TYPE_FILTER_OP(equal, == );
        KAIXO_TYPE_FILTER_OP(not_equal, != );
        KAIXO_TYPE_FILTER_OP(left_shift, << );
        KAIXO_TYPE_FILTER_OP(right_shift, >> );
        KAIXO_TYPE_FILTER_OP(boolean_and, &&);
        KAIXO_TYPE_FILTER_OP(boolean_or, || );
        KAIXO_TYPE_FILTER_OP(bitwise_and, &);
        KAIXO_TYPE_FILTER_OP(bitwise_or, | );
        KAIXO_TYPE_FILTER_OP(bitwise_xor, ^);
        KAIXO_TYPE_FILTER_OP(spaceship, <=> );
        KAIXO_TYPE_FILTER_OP(add_assign, +=);
        KAIXO_TYPE_FILTER_OP(subtract_assign, -=);
        KAIXO_TYPE_FILTER_OP(multiply_assign, *=);
        KAIXO_TYPE_FILTER_OP(divide_assign, /=);
        KAIXO_TYPE_FILTER_OP(modulo_assign, %=);
        KAIXO_TYPE_FILTER_OP(left_shift_assign, <<=);
        KAIXO_TYPE_FILTER_OP(right_shift_assign, >>=);
        KAIXO_TYPE_FILTER_OP(and_assign, &=);
        KAIXO_TYPE_FILTER_OP(or_assign, |=);
        KAIXO_TYPE_FILTER_OP(xor_assign, ^=);
    }
#undef KAIXO_TYPE_FILTER_OP
}