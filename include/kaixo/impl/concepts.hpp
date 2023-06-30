#pragma once
#include "helpers.hpp"

/**
 * All standard traits as concepts.
 */
namespace kaixo {
    namespace detail {
        template<template<class Ty, class ...Args> class Trait, class Ty, class ...Args>
        struct pack_trait_helper : Trait<Ty, Args...> {};
        template<template<class Ty, class ...Args> class Trait, class Ty, class ...Args>
        struct pack_trait_helper<Trait, Ty, info<Args...>> : Trait<Ty, Args...> {};

        template<class, template<class...> class>
        struct specialization_impl : std::false_type {};
        template<template<class...> class Ref, class... Args>
        struct specialization_impl<Ref<Args...>, Ref> : std::true_type {};
    }

    namespace concepts {
        template<class Ty> concept void_type = std::is_void_v<Ty>;
        template<class Ty> concept null_pointer = std::is_null_pointer_v<Ty>;
        template<class Ty> concept boolean = std::is_same_v<Ty, bool>;
        template<class Ty> concept integral = std::is_integral_v<Ty>;
        template<class Ty> concept floating_point = std::is_floating_point_v<Ty>;
        template<class Ty> concept array = std::is_array_v<Ty>;
        template<class Ty> concept enum_type = std::is_enum_v<Ty>;
        template<class Ty> concept union_type = std::is_union_v<Ty>;
        template<class Ty> concept class_type = std::is_class_v<Ty>;
        template<class Ty> concept function = std::is_function_v<Ty>;
        template<class Ty> concept pointer = std::is_pointer_v<Ty>;
        template<class Ty> concept lvalue_reference = std::is_lvalue_reference_v<Ty>;
        template<class Ty> concept rvalue_reference = std::is_rvalue_reference_v<Ty>;
        template<class Ty> concept member_object_pointer = std::is_member_object_pointer_v<Ty>;
        template<class Ty> concept member_function_pointer = std::is_member_function_pointer_v<Ty>;
        template<class Ty> concept fundamental = std::is_fundamental_v<Ty>;
        template<class Ty> concept arithmetic = std::is_arithmetic_v<Ty>;
        template<class Ty> concept scalar = std::is_scalar_v<Ty>;
        template<class Ty> concept object = std::is_object_v<Ty>;
        template<class Ty> concept compound = std::is_compound_v<Ty>;
        template<class Ty> concept reference = std::is_reference_v<Ty>;
        template<class Ty> concept member_pointer = std::is_member_pointer_v<Ty>;
        template<class Ty> concept const_type = std::is_const_v<std::remove_reference_t<Ty>>;
        template<class Ty> concept volatile_type = std::is_volatile_v<std::remove_reference_t<Ty>>;
        template<class Ty> concept trivial = std::is_trivial_v<Ty>;
        template<class Ty> concept trivially_copyable = std::is_trivially_copyable_v<Ty>;
        template<class Ty> concept standard_layout = std::is_standard_layout_v<Ty>;
        template<class Ty> concept empty = std::is_empty_v<Ty>;
        template<class Ty> concept polymorphic = std::is_polymorphic_v<Ty>;
        template<class Ty> concept abstract = std::is_abstract_v<Ty>;
        template<class Ty> concept final = std::is_final_v<Ty>;
        template<class Ty> concept aggregate = std::is_aggregate_v<Ty>;
        template<class Ty> concept signed_integral = std::is_signed_v<Ty>;
        template<class Ty> concept unsigned_integral = std::is_unsigned_v<Ty>;
        template<class Ty> concept bounded_array = std::is_bounded_array_v<Ty>;
        template<class Ty> concept unbounded_array = std::is_unbounded_array_v<Ty>;
        template<class Ty> concept default_constructible = std::is_default_constructible_v<Ty>;
        template<class Ty> concept trivially_default_constructible = std::is_trivially_default_constructible_v<Ty>;
        template<class Ty> concept nothrow_default_constructible = std::is_nothrow_default_constructible_v<Ty>;
        template<class Ty> concept copy_constructible = std::is_copy_constructible_v<Ty>;
        template<class Ty> concept trivially_copy_constructible = std::is_trivially_copy_constructible_v<Ty>;
        template<class Ty> concept nothrow_copy_constructible = std::is_nothrow_copy_constructible_v<Ty>;
        template<class Ty> concept move_constructible = std::is_move_constructible_v<Ty>;
        template<class Ty> concept trivially_move_constructible = std::is_trivially_move_constructible_v<Ty>;
        template<class Ty> concept nothrow_move_constructible = std::is_nothrow_move_constructible_v<Ty>;
        template<class Ty> concept copy_assignable = std::is_copy_assignable_v<Ty>;
        template<class Ty> concept trivially_copy_assignable = std::is_trivially_copy_assignable_v<Ty>;
        template<class Ty> concept nothrow_copy_assignable = std::is_nothrow_copy_assignable_v<Ty>;
        template<class Ty> concept move_assignable = std::is_move_assignable_v<Ty>;
        template<class Ty> concept trivially_move_assignable = std::is_trivially_move_assignable_v<Ty>;
        template<class Ty> concept nothrow_move_assignable = std::is_nothrow_move_assignable_v<Ty>;
        template<class Ty> concept destructible = std::is_destructible_v<Ty>;
        template<class Ty> concept trivially_destructible = std::is_trivially_destructible_v<Ty>;
        template<class Ty> concept nothrow_destructible = std::is_nothrow_destructible_v<Ty>;
        template<class Ty> concept swappable = std::is_swappable_v<Ty>;
        template<class Ty> concept nothrow_swappable = std::is_nothrow_swappable_v<Ty>;

        template<class Ty> concept unique_object_representations = std::has_unique_object_representations_v<Ty>;
        template<class Ty> concept virtual_destructor = std::has_virtual_destructor_v<Ty>;

        template<class Ty, class Other> concept assignable = std::is_assignable_v<Ty, Other>;
        template<class Ty, class Other> concept trivially_assignable = std::is_trivially_assignable_v<Ty, Other>;
        template<class Ty, class Other> concept nothrow_assignable = std::is_nothrow_assignable_v<Ty, Other>;
        template<class Ty, class Other> concept assignable_to = std::is_assignable_v<Other, Ty>;
        template<class Ty, class Other> concept trivially_assignable_to = std::is_trivially_assignable_v<Other, Ty>;
        template<class Ty, class Other> concept nothrow_assignable_to = std::is_nothrow_assignable_v<Other, Ty>;
        template<class Ty, class Other> concept swappable_with = std::is_swappable_with_v<Ty, Other>;
        template<class Ty, class Other> concept nothrow_swappable_with = std::is_nothrow_swappable_with_v<Ty, Other>;

        template<class Ty, class Other> concept same_as = std::is_same_v<Ty, Other>;
        template<class Ty, class Other> concept base_of = std::is_base_of_v<Ty, Other>;
        template<class Ty, class Other> concept convertible_to = std::is_convertible_v<Ty, Other>;
        template<class Ty, class Other> concept nothrow_convertible_to = std::is_nothrow_convertible_v<Ty, Other>;
        template<class Ty, class Other> concept convertible_from = std::is_convertible_v<Other, Ty>;
        template<class Ty, class Other> concept nothrow_convertible_from = std::is_nothrow_convertible_v<Other, Ty>;

        template<class Ty, class ...Args> concept constructible = detail::pack_trait_helper<std::is_constructible, Ty, Args...>::value;
        template<class Ty, class ...Args> concept trivially_constructible = detail::pack_trait_helper<std::is_trivially_constructible, Ty, Args...>::value;
        template<class Ty, class ...Args> concept nothrow_constructible = detail::pack_trait_helper<std::is_nothrow_constructible, Ty, Args...>::value;
        template<class Ty, class ...Args> concept invocable = detail::pack_trait_helper<std::is_invocable, Ty, Args...>::value;
        template<class Ty, class ...Args> concept nothrow_invocable = detail::pack_trait_helper<std::is_nothrow_invocable, Ty, Args...>::value;

        template<class Ty, class Other> concept constructs = std::is_constructible_v<Other, Ty>;
        template<class Ty, class Other> concept trivially_constructs = std::is_trivially_constructible_v<Other, Ty>;
        template<class Ty, class Other> concept nothrow_constructs = std::is_nothrow_constructible_v<Other, Ty>;
        template<class Ty, class Other> concept invoces = std::is_invocable_v<Other, Ty>;
        template<class Ty, class Other> concept nothrow_invoces = std::is_nothrow_invocable_v<Other, Ty>;

        template<class Test, template<class...> class Ref>
        concept specialization = detail::specialization_impl<std::decay_t<Test>, Ref>::value;

        template<class Ty>
        concept structured_binding = aggregate<Ty> || requires () {
            typename std::tuple_element<0, Ty>::type;
        };

        template<class Ty> concept functor = requires(decltype(&Ty::operator()) a) { a; };
    }
}