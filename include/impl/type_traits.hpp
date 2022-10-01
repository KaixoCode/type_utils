#pragma once
#include "concepts.hpp"

/**
 * All standard type traits wrapped in an object, allows for
 * simple boolean operations and partial application.
 * Even allows for complex concept constraints like:
 * template<require<is_integral || is_floating_point> Ty>
 */
namespace kaixo {
    inline namespace type_traits {
        /**
         * Concept to match a type_trait.
         * @tparam Ty type
         * @tparam V type_trait value
         */
        template<class Ty, auto V>
        concept require = V.template value<Ty>;

        /**
         * Boolean and on 2 type_trait classes.
         * @tparam A type trait 1
         * @tparam B type trait 2
         */
        template<template<class ...> class A, template<class ...> class B>
        struct type_trait_and {
            template<class ...Args> struct type {
                constexpr static bool value = A<Args...>::value && B<Args...>::value;
            };
        };

        /**
         * Boolean or on 2 type_trait classes.
         * @tparam A type trait 1
         * @tparam B type trait 2
         */
        template<template<class ...> class A, template<class ...> class B>
        struct type_trait_or {
            template<class ...Args> struct type {
                constexpr static bool value = A<Args...>::value || B<Args...>::value;
            };
        };

        /**
         * Boolean not on a type_trait class.
         * @tparam A type trait
         */
        template<template<class ...> class A>
        struct type_trait_not {
            template<class ...Args> struct type {
                constexpr static bool value = !A<Args...>::value;
            };
        };

        /**
         * Partially applied type trait, where last types
         * are provided.
         * @tparam A type trait
         * @tparam Tys... provided arguments
         */
        template<template<class ...> class A, class ...Tys>
        struct type_trait_partial_last {
            template<class ...Args> struct type {
                constexpr static bool value = A<Args..., Tys...>::value;
            };
        };

        /**
         * Partially applied type trait, where last types
         * are provided in a pack.
         * @tparam A type trait
         * @tparam Tys... provided arguments
         */
        template<template<class ...> class A, class ...Tys>
        struct type_trait_partial_last<A, info<Tys...>> {
            template<class ...Args> struct type {
                constexpr static bool value = A<Args..., Tys...>::value;
            };
        };

        /**
         * Partially applied type trait, where first types
         * are provided.
         * @tparam A type trait
         * @tparam Tys... provided arguments
         */
        template<template<class ...> class A, class ...Tys>
        struct type_trait_partial_first {
            template<class ...Args> struct type {
                constexpr static bool value = A<Tys..., Args...>::value;
            };
        };

        /**
         * Partially applied type trait, where first types
         * are provided in a pack.
         * @tparam A type trait
         * @tparam Tys... provided arguments
         */
        template<template<class ...> class A, class ...Tys>
        struct type_trait_partial_first<A, info<Tys...>> {
            template<class ...Args> struct type {
                constexpr static bool value = A<Tys..., Args...>::value;
            };
        };

        /**
         * Unevaluated type trait wrapper.
         * @tparam Trait type trait
         */
        template<template<class ...> class Trait>
        struct type_trait {
            template<class ...Tys>
            constexpr static bool value = Trait<Tys...>::value;
        };

        template<class> struct is_type_trait_impl : std::false_type {};
        template<template<class ...> class T>
        struct is_type_trait_impl<type_trait<T>> : std::true_type {};

        /**
         * Check if Ty is a type trait object.
         * @tparam Ty type
         */
        template<class Ty>
        concept is_type_trait = is_type_trait_impl<Ty>::value;

        /**
         * Boolean and on 2 type traits
         * @tparam A type trait 1
         * @tparam B type trait 2
         * @return type trait that matches if both match
         */
        template<template<class ...> class A, template<class ...> class B>
        consteval auto operator and(type_trait<A>, type_trait<B>) {
            return type_trait<typename type_trait_and<A, B>::type>{};
        }

        /**
         * Boolean or on 2 type traits
         * @tparam A type trait 1
         * @tparam B type trait 2
         * @return type trait that matches if either matches
         */
        template<template<class ...> class A, template<class ...> class B>
        consteval auto operator or(type_trait<A>, type_trait<B>) {
            return type_trait<typename type_trait_or<A, B>::type>{};
        }

        /**
         * Boolean not on a type trait
         * @tparam A type trait
         * @return type trait that matches if A doesn't match
         */
        template<template<class ...> class A>
        consteval auto operator not(type_trait<A>) {
            return type_trait<typename type_trait_not<A>::type>{};
        }

        constexpr auto is_void = type_trait<std::is_void>{};
        constexpr auto is_null_pointer = type_trait<std::is_null_pointer>{};
        constexpr auto is_integral = type_trait<std::is_integral>{};
        constexpr auto is_floating_point = type_trait<std::is_floating_point>{};
        constexpr auto is_array = type_trait<std::is_array>{};
        constexpr auto is_enum = type_trait<std::is_enum>{};
        constexpr auto is_union = type_trait<std::is_union>{};
        constexpr auto is_class = type_trait<std::is_class>{};
        constexpr auto is_function = type_trait<std::is_function>{};
        constexpr auto is_pointer = type_trait<std::is_pointer>{};
        constexpr auto is_lvalue_reference = type_trait<std::is_lvalue_reference>{};
        constexpr auto is_rvalue_reference = type_trait<std::is_rvalue_reference>{};
        constexpr auto is_member_object_pointer = type_trait<std::is_member_object_pointer>{};
        constexpr auto is_member_function_pointer = type_trait<std::is_member_function_pointer>{};
        constexpr auto is_fundamental = type_trait<std::is_fundamental>{};
        constexpr auto is_arithmetic = type_trait<std::is_arithmetic>{};
        constexpr auto is_scalar = type_trait<std::is_scalar>{};
        constexpr auto is_object = type_trait<std::is_object>{};
        constexpr auto is_compound = type_trait<std::is_compound>{};
        constexpr auto is_reference = type_trait<std::is_reference>{};
        constexpr auto is_member_pointer = type_trait<std::is_member_pointer>{};
        constexpr auto is_const = type_trait<std::is_const>{};
        constexpr auto is_volatile = type_trait<std::is_volatile>{};
        constexpr auto is_trivial = type_trait<std::is_trivial>{};
        constexpr auto is_trivially_copyable = type_trait<std::is_trivially_copyable>{};
        constexpr auto is_standard_layout = type_trait<std::is_standard_layout>{};
        constexpr auto is_empty = type_trait<std::is_empty>{};
        constexpr auto is_polymorphic = type_trait<std::is_polymorphic>{};
        constexpr auto is_abstract = type_trait<std::is_abstract>{};
        constexpr auto is_final = type_trait<std::is_final>{};
        constexpr auto is_aggregate = type_trait<std::is_aggregate>{};
        constexpr auto is_signed = type_trait<std::is_signed>{};
        constexpr auto is_unsigned = type_trait<std::is_unsigned>{};
        constexpr auto is_bounded_array = type_trait<std::is_bounded_array>{};
        constexpr auto is_unbounded_array = type_trait<std::is_unbounded_array>{};
        constexpr auto is_default_constructible = type_trait<std::is_default_constructible>{};
        constexpr auto is_trivially_default_constructible = type_trait<std::is_trivially_default_constructible>{};
        constexpr auto is_nothrow_default_constructible = type_trait<std::is_nothrow_default_constructible>{};
        constexpr auto is_copy_constructible = type_trait<std::is_copy_constructible>{};
        constexpr auto is_trivially_copy_constructible = type_trait<std::is_trivially_copy_constructible>{};
        constexpr auto is_nothrow_copy_constructible = type_trait<std::is_nothrow_copy_constructible>{};
        constexpr auto is_move_constructible = type_trait<std::is_move_constructible>{};
        constexpr auto is_trivially_move_constructible = type_trait<std::is_trivially_move_constructible>{};
        constexpr auto is_nothrow_move_constructible = type_trait<std::is_nothrow_move_constructible>{};
        constexpr auto is_copy_assignable = type_trait<std::is_copy_assignable>{};
        constexpr auto is_trivially_copy_assignable = type_trait<std::is_trivially_copy_assignable>{};
        constexpr auto is_nothrow_copy_assignable = type_trait<std::is_nothrow_copy_assignable>{};
        constexpr auto is_move_assignable = type_trait<std::is_move_assignable>{};
        constexpr auto is_trivially_move_assignable = type_trait<std::is_trivially_move_assignable>{};
        constexpr auto is_nothrow_move_assignable = type_trait<std::is_nothrow_move_assignable>{};
        constexpr auto is_destructible = type_trait<std::is_destructible>{};
        constexpr auto is_trivially_destructible = type_trait<std::is_trivially_destructible>{};
        constexpr auto is_nothrow_destructible = type_trait<std::is_nothrow_destructible>{};
        constexpr auto is_swappable = type_trait<std::is_swappable>{};
        constexpr auto is_nothrow_swappable = type_trait<std::is_nothrow_swappable>{};

        constexpr auto has_unique_object_representations = type_trait<std::has_unique_object_representations>{};
        constexpr auto has_virtual_destructor = type_trait<std::has_virtual_destructor>{};

        template<class Other> constexpr auto is_assignable = type_trait<typename type_trait_partial_last<std::is_assignable, Other>::type>{};
        template<class Other> constexpr auto is_trivially_assignable = type_trait<typename type_trait_partial_last<std::is_trivially_assignable, Other>::type>{};
        template<class Other> constexpr auto is_nothrow_assignable = type_trait<typename type_trait_partial_last<std::is_nothrow_assignable, Other>::type>{};
        template<class Other> constexpr auto is_assignable_to = type_trait<typename type_trait_partial_first<std::is_assignable, Other>::type>{};
        template<class Other> constexpr auto is_trivially_assignable_to = type_trait<typename type_trait_partial_first<std::is_trivially_assignable, Other>::type>{};
        template<class Other> constexpr auto is_nothrow_assignable_to = type_trait<typename type_trait_partial_first<std::is_nothrow_assignable, Other>::type>{};
        template<class Other> constexpr auto is_swappable_with = type_trait<typename type_trait_partial_last<std::is_swappable_with, Other>::type>{};
        template<class Other> constexpr auto is_nothrow_swappable_with = type_trait<typename type_trait_partial_last<std::is_nothrow_swappable_with, Other>::type>{};

        template<class Other> constexpr auto is_same = type_trait<typename type_trait_partial_last<std::is_same, Other>::type>{};
        template<class Other> constexpr auto is_base_of = type_trait<typename type_trait_partial_last<std::is_base_of, Other>::type>{};
        template<class Other> constexpr auto is_derived_of = type_trait<typename type_trait_partial_first<std::is_base_of, Other>::type>{};
        template<class Other> constexpr auto is_convertible_to = type_trait<typename type_trait_partial_last<std::is_convertible, Other>::type>{};
        template<class Other> constexpr auto is_nothrow_convertible_to = type_trait<typename type_trait_partial_last<std::is_nothrow_convertible, Other>::type>{};
        template<class Other> constexpr auto is_convertible_from = type_trait<typename type_trait_partial_first<std::is_convertible, Other>::type>{};
        template<class Other> constexpr auto is_nothrow_convertible_from = type_trait<typename type_trait_partial_first<std::is_nothrow_convertible, Other>::type>{};

        template<class ...Args> constexpr auto is_constructible = type_trait<typename type_trait_partial_last<std::is_constructible, Args...>::type>{};
        template<class ...Args> constexpr auto is_trivially_constructible = type_trait<typename type_trait_partial_last<std::is_trivially_constructible, Args...>::type>{};
        template<class ...Args> constexpr auto is_nothrow_constructible = type_trait<typename type_trait_partial_last<std::is_nothrow_constructible, Args...>::type>{};
        template<class ...Args> constexpr auto is_invocable = type_trait<typename type_trait_partial_last<std::is_invocable, Args...>::type>{};
        template<class ...Args> constexpr auto is_nothrow_invocable = type_trait<typename type_trait_partial_last<std::is_nothrow_invocable, Args...>::type>{};

        template<template<class ...> class Ty>
        struct is_specialization_impl {
            template<class T> struct type {
                constexpr static bool value = specialization<T, Ty>;
            };
        };

        template<template<class ...> class Ty> constexpr auto is_specialization = type_trait<typename is_specialization_impl<Ty>::type>{};
    }
}