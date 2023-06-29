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
        constexpr auto is_void = type_filter<std::is_void>{};
        constexpr auto is_null_pointer = type_filter<std::is_null_pointer>{};
        constexpr auto is_integral = type_filter<std::is_integral>{};
        constexpr auto is_floating_point = type_filter<std::is_floating_point>{};
        constexpr auto is_array = type_filter<std::is_array>{};
        constexpr auto is_enum = type_filter<std::is_enum>{};
        constexpr auto is_union = type_filter<std::is_union>{};
        constexpr auto is_class = type_filter<std::is_class>{};
        constexpr auto is_function = type_filter<std::is_function>{};
        constexpr auto is_pointer = type_filter<std::is_pointer>{};
        constexpr auto is_lvalue_reference = type_filter<std::is_lvalue_reference>{};
        constexpr auto is_rvalue_reference = type_filter<std::is_rvalue_reference>{};
        constexpr auto is_member_object_pointer = type_filter<std::is_member_object_pointer>{};
        constexpr auto is_member_function_pointer = type_filter<std::is_member_function_pointer>{};
        constexpr auto is_fundamental = type_filter<std::is_fundamental>{};
        constexpr auto is_arithmetic = type_filter<std::is_arithmetic>{};
        constexpr auto is_scalar = type_filter<std::is_scalar>{};
        constexpr auto is_object = type_filter<std::is_object>{};
        constexpr auto is_compound = type_filter<std::is_compound>{};
        constexpr auto is_reference = type_filter<std::is_reference>{};
        constexpr auto is_member_pointer = type_filter<std::is_member_pointer>{};
        constexpr auto is_const = type_filter<std::is_const>{};
        constexpr auto is_volatile = type_filter<std::is_volatile>{};
        constexpr auto is_trivial = type_filter<std::is_trivial>{};
        constexpr auto is_trivially_copyable = type_filter<std::is_trivially_copyable>{};
        constexpr auto is_standard_layout = type_filter<std::is_standard_layout>{};
        constexpr auto is_empty = type_filter<std::is_empty>{};
        constexpr auto is_polymorphic = type_filter<std::is_polymorphic>{};
        constexpr auto is_abstract = type_filter<std::is_abstract>{};
        constexpr auto is_final = type_filter<std::is_final>{};
        constexpr auto is_aggregate = type_filter<std::is_aggregate>{};
        constexpr auto is_signed = type_filter<std::is_signed>{};
        constexpr auto is_unsigned = type_filter<std::is_unsigned>{};
        constexpr auto is_bounded_array = type_filter<std::is_bounded_array>{};
        constexpr auto is_unbounded_array = type_filter<std::is_unbounded_array>{};
        constexpr auto is_default_constructible = type_filter<std::is_default_constructible>{};
        constexpr auto is_trivially_default_constructible = type_filter<std::is_trivially_default_constructible>{};
        constexpr auto is_nothrow_default_constructible = type_filter<std::is_nothrow_default_constructible>{};
        constexpr auto is_copy_constructible = type_filter<std::is_copy_constructible>{};
        constexpr auto is_trivially_copy_constructible = type_filter<std::is_trivially_copy_constructible>{};
        constexpr auto is_nothrow_copy_constructible = type_filter<std::is_nothrow_copy_constructible>{};
        constexpr auto is_move_constructible = type_filter<std::is_move_constructible>{};
        constexpr auto is_trivially_move_constructible = type_filter<std::is_trivially_move_constructible>{};
        constexpr auto is_nothrow_move_constructible = type_filter<std::is_nothrow_move_constructible>{};
        constexpr auto is_copy_assignable = type_filter<std::is_copy_assignable>{};
        constexpr auto is_trivially_copy_assignable = type_filter<std::is_trivially_copy_assignable>{};
        constexpr auto is_nothrow_copy_assignable = type_filter<std::is_nothrow_copy_assignable>{};
        constexpr auto is_move_assignable = type_filter<std::is_move_assignable>{};
        constexpr auto is_trivially_move_assignable = type_filter<std::is_trivially_move_assignable>{};
        constexpr auto is_nothrow_move_assignable = type_filter<std::is_nothrow_move_assignable>{};
        constexpr auto is_destructible = type_filter<std::is_destructible>{};
        constexpr auto is_trivially_destructible = type_filter<std::is_trivially_destructible>{};
        constexpr auto is_nothrow_destructible = type_filter<std::is_nothrow_destructible>{};
        constexpr auto is_swappable = type_filter<std::is_swappable>{};
        constexpr auto is_nothrow_swappable = type_filter<std::is_nothrow_swappable>{};

        constexpr auto has_unique_object_representations = type_filter<std::has_unique_object_representations>{};
        constexpr auto has_virtual_destructor = type_filter<std::has_virtual_destructor>{};

        template<class Other> constexpr auto is_assignable = type_filter<typename partial_last<std::is_assignable, Other>::type>{};
        template<class Other> constexpr auto is_trivially_assignable = type_filter<typename partial_last<std::is_trivially_assignable, Other>::type>{};
        template<class Other> constexpr auto is_nothrow_assignable = type_filter<typename partial_last<std::is_nothrow_assignable, Other>::type>{};
        template<class Other> constexpr auto is_assignable_to = type_filter<typename partial<std::is_assignable, Other>::type>{};
        template<class Other> constexpr auto is_trivially_assignable_to = type_filter<typename partial<std::is_trivially_assignable, Other>::type>{};
        template<class Other> constexpr auto is_nothrow_assignable_to = type_filter<typename partial<std::is_nothrow_assignable, Other>::type>{};
        template<class Other> constexpr auto is_swappable_with = type_filter<typename partial_last<std::is_swappable_with, Other>::type>{};
        template<class Other> constexpr auto is_nothrow_swappable_with = type_filter<typename partial_last<std::is_nothrow_swappable_with, Other>::type>{};

        template<class Other> constexpr auto is_same = type_filter<typename partial_last<std::is_same, Other>::type>{};
        template<class Other> constexpr auto is_base_of = type_filter<typename partial_last<std::is_base_of, Other>::type>{};
        template<class Other> constexpr auto is_derived_of = type_filter<typename partial<std::is_base_of, Other>::type>{};
        template<class Other> constexpr auto is_convertible_to = type_filter<typename partial_last<std::is_convertible, Other>::type>{};
        template<class Other> constexpr auto is_nothrow_convertible_to = type_filter<typename partial_last<std::is_nothrow_convertible, Other>::type>{};
        template<class Other> constexpr auto is_convertible_from = type_filter<typename partial<std::is_convertible, Other>::type>{};
        template<class Other> constexpr auto is_nothrow_convertible_from = type_filter<typename partial<std::is_nothrow_convertible, Other>::type>{};

        template<class ...Args> constexpr auto is_constructible = type_filter<typename partial_last<std::is_constructible, Args...>::type>{};
        template<class ...Args> constexpr auto is_trivially_constructible = type_filter<typename partial_last<std::is_trivially_constructible, Args...>::type>{};
        template<class ...Args> constexpr auto is_nothrow_constructible = type_filter<typename partial_last<std::is_nothrow_constructible, Args...>::type>{};
        template<class ...Args> constexpr auto is_invocable = type_filter<typename partial_last<std::is_invocable, Args...>::type>{};
        template<class ...Args> constexpr auto is_nothrow_invocable = type_filter<typename partial_last<std::is_nothrow_invocable, Args...>::type>{};
        
        template<class Other> constexpr auto can_construct = type_filter<typename partial<std::is_constructible, Other>::type>{};
        template<class Other> constexpr auto can_trivially_construct = type_filter<typename partial<std::is_trivially_constructible, Other>::type>{};
        template<class Other> constexpr auto can_nothrow_construct = type_filter<typename partial<std::is_nothrow_constructible, Other>::type>{};
        template<class Other> constexpr auto can_invoke = type_filter<typename partial<std::is_invocable, Other>::type>{};
        template<class Other> constexpr auto can_nothrow_invoke = type_filter<typename partial<std::is_nothrow_invocable, Other>::type>{};

        template<template<class ...> class Ty>
        struct is_specialization_impl {
            template<class T> struct type {
                constexpr static bool value = specialization<T, Ty>;
            };
        };

        template<template<class ...> class Ty> constexpr auto is_specialization = type_filter<typename is_specialization_impl<Ty>::type>{};

        template<class Ty> struct structured_binding_impl : std::false_type {};
        template<structured_binding Ty> struct structured_binding_impl<Ty> : std::true_type {};

        constexpr auto has_structured_binding = type_filter<structured_binding_impl>{};
    }
}