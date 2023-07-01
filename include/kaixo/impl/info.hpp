#pragma once
#include "type_traits.hpp"
#include "pack.hpp"
#include "functions.hpp"
#include "struct.hpp"
#include "qualifiers.hpp"
#include "members.hpp"

namespace kaixo {

    struct _s_none {};
    struct _s_binding {};
    struct _s_fun {};
    struct _s_fun_ptr {};
    struct _s_integral {};
    struct _s_floating {};
    struct _s_enum {};
    struct _s_array {};
    struct _s_aggregate {};
    struct _s_memfun {};
    struct _s_value {};
    struct _s_templated {};

    template<class ...Tys>
    struct specialized_info {
        using _selected_specialization = _s_none;
    };

    /**
     * Specialization for types that define structured binding.
     */
    template<class ...Tys>
        requires ((concepts::structured_binding<Tys> && ...) && ((!concepts::array<Tys> && !concepts::aggregate<Tys>) && ...))
    struct specialized_info<Tys...> {
        using _selected_specialization = _s_binding;

        using binding_types = info<binding_types_t<Tys>...>;
        using binding_size = info<value_t<binding_size_v<Tys>>...>;
    };

    /**
     * Specialization for callable types, to also
     * contain function info.
     */
    template<class ...Tys>
    struct specialized_info_fun0;

    template<class ...Tys> requires (sizeof...(Tys) > 1)
        struct specialized_info_fun0<Tys...> {
        using arguments = info<typename detail::function_info<Tys>::arguments...>;
    };

    template<class Ty>
    struct specialized_info_fun0<Ty> {
        using arguments = typename detail::function_info<Ty>::arguments;
    };

    template<class ...Tys>
    struct specialized_info_fun1 : specialized_info_fun0<Tys...> {
        using pointer = info<typename detail::function_info<Tys>::pointer...>;
        using signature = info<typename detail::function_info<Tys>::signature...>;
        using result = info<typename detail::function_info<Tys>::result...>;

        using is_noexcept = info<value_t<detail::function_info<Tys>::is_noexcept>...>;

        using add_noexcept = info<typename detail::function_info<Tys>::add_noexcept...>;
        using remove_noexcept = info<typename detail::function_info<Tys>::remove_noexcept...>;
        using fun_decay = info<typename detail::function_info<Tys>::fun_decay...>;
    };

    template<class ...Tys>
    struct specialized_info_fun2 : specialized_info_fun1<Tys...> {
        using is_fun_const = info<value_t<detail::function_info<Tys>::is_fun_const>...>;
        using is_fun_mutable = info<value_t<detail::function_info<Tys>::is_fun_mutable>...>;
        using is_fun_volatile = info<value_t<detail::function_info<Tys>::is_fun_volatile>...>;
        using is_fun_lvalue_reference = info<value_t<detail::function_info<Tys>::is_fun_lvalue_reference>...>;
        using is_fun_rvalue_reference = info<value_t<detail::function_info<Tys>::is_fun_rvalue_reference>...>;
        using is_fun_reference = info<value_t<detail::function_info<Tys>::is_fun_reference>...>;

        using add_fun_const = info<typename detail::function_info<Tys>::add_fun_const...>;
        using remove_fun_const = info<typename detail::function_info<Tys>::remove_fun_const...>;
        using add_fun_volatile = info<typename detail::function_info<Tys>::add_fun_volatile...>;
        using remove_fun_volatile = info<typename detail::function_info<Tys>::remove_fun_volatile...>;
        using add_fun_cv = info<typename detail::function_info<Tys>::add_fun_cv...>;
        using remove_fun_cv = info<typename detail::function_info<Tys>::remove_fun_cv...>;
        using add_fun_lvalue_reference = info<typename detail::function_info<Tys>::add_fun_lvalue_reference...>;
        using add_fun_rvalue_reference = info<typename detail::function_info<Tys>::add_fun_rvalue_reference...>;
        using remove_fun_reference = info<typename detail::function_info<Tys>::remove_fun_reference...>;
        using remove_fun_cvref = info<typename detail::function_info<Tys>::remove_fun_cvref...>;
    };

    template<class ...Tys>
        requires ((detail::callable_type<Tys> && ...) && (concepts::pointer<Tys> || ...))
    struct specialized_info<Tys...> : specialized_info_fun1<Tys...> {
        using _selected_specialization = _s_fun_ptr;
    };

    template<class ...Tys>
        requires ((detail::callable_type<Tys> && ...) && (!concepts::pointer<Tys> && ...))
    struct specialized_info<Tys...> : specialized_info_fun2<Tys...> {
        using _selected_specialization = _s_fun;
    };

    /**
     * Specialization for integral types, to
     * add the make_signed/make_unsigned type traits.
     * Also inherits from std::numeric_limits<Ty>
     */
    template<class ...Tys>
        requires ((concepts::integral<Tys> && !concepts::boolean<Tys>) && ...)
    struct specialized_info<Tys...> {
        using _selected_specialization = _s_integral;

        using make_signed = info<std::make_signed<Tys>...>;
        using make_unsigned = info<std::make_unsigned<Tys>...>;
    };

    /**
     * Specialization for floating pointer, inherits
     * from std::numeric_limits<Ty>
     */
    template<class Ty, class ...Tys>
        requires (concepts::floating_point<Ty> && (concepts::same_as<Ty, Tys> && ...))
    struct specialized_info<Ty, Tys...> : std::numeric_limits<Ty> {
        using _selected_specialization = _s_floating;
    };

    /**
     * Specialization for enum types, contains underlying
     * type, and enum names.
     */
    template<class ...Tys>
        requires (concepts::enum_type<Tys> && ...)
    struct specialized_info<Tys...> {
        using _selected_specialization = _s_enum;

        using underlying = info<std::underlying_type_t<Tys>...>;

        template<auto Value>
        using name = info<value_t<enum_name<Tys, Value>>...>;

        template<auto Value>
        using defined = info<value_t<enum_defined<Tys, Value>>...>;
    };

    /**
     * Specialization for member object pointer,
     * contains the 'object' and 'value_type'
     */
    template<class ...Tys, class ...Objs>
        requires concepts::member_object_pointer<Tys Objs::*...>
    struct specialized_info<Tys Objs::*...> {
        using _selected_specialization = _s_memfun;

        using object = info<Objs...>;
        using value_type = info<Tys...>;
    };

    /**
     * Specialization for Arrays,
     * contains rank/extent traits.
     */
    template<class ...Tys>
        requires (concepts::array<Tys> && ...)
    struct specialized_info<Tys...> {
        using _selected_specialization = _s_array;

        using rank = info<value_t<std::rank_v<Tys>>...>;

        template<std::size_t Dim = 0>
        using extent = info<value_t<std::extent_v<Tys, Dim>>...>;

        using remove_extent = info<std::remove_extent_t<Tys>...>;
        using remove_all_extents = info<std::remove_all_extents_t<Tys>...>;
    };

    /**
     * Specialization for structs, contains information on members.
     */
    template<class ...Tys>
        requires ((concepts::aggregate<Tys> && ...) && (!concepts::array<Tys> && ...))
    struct specialized_info<Tys...> {
        using _selected_specialization = _s_aggregate;

        using members = info<struct_members_t<Tys>...>;
        using struct_size = info<value_t<struct_size_v<Tys>>...>;
    };

    /**
     * Specialization for a single value.
     */
    template<auto V>
    struct specialized_info<value_t<V>> : info<decltype(V)> {
        using _selected_specialization = _s_value;

        constexpr static auto value = V;
    };

    /**
     * Specialization for pack of values.
     */
    template<auto ...Vs> requires (sizeof...(Vs) > 1)
        struct specialized_info<value_t<Vs>...> : info<decltype(Vs)...> {
        using _selected_specialization = _s_value;

        template<std::size_t I> constexpr static auto value = pack::element_t<I, value_t<Vs>...>::value;
    };

    /**
     * Specialization for templated types.
     */
    template<template<class...> class ...Tys>
    struct specialized_info<templated_t<Tys>...> {
        using _selected_specialization = _s_templated;

        template<class ...Args> 
        using instantiate = info<typename pack::instantiate<Tys, pack::detail::convert_to_info<Args...>>::type...>;
    };

    template<class ...T>
    struct info_base;

    /**
     * Base for a pack of types.
     * @tparam ...Tys types
     */
    template<class ...Tys> requires (sizeof...(Tys) > 1)
    struct info_base<Tys...> : specialized_info<Tys...> {
        template<std::size_t I>
        using type = pack::element_t<I, Tys...>;

        using alignment = info<value_t<alignof_v<Tys>>...>;
    };

    template<class Ty>
    struct info_base<Ty> : specialized_info<Ty> {
        using type = Ty;

        using alignment = info<value_t<alignof_v<Ty>>>;
    };

    template<> struct info_base<> {};

    template<class ...Tys>
    struct info : info_base<Tys...> {
        constexpr static std::size_t size = sizeof...(Tys);
        constexpr static std::size_t unique_size = pack::count_unique_v<info<Tys...>>;
        using bytes = info<value_t<sizeof_v<Tys>>...>;

        // Type manipulators

        template<template<class...> class              T> using transform = info<pack::transform_t<T, Tys>...>;
        template<auto Filter, template<class...> class T> using conditional_transform = info<pack::conditional_transform_t<Filter, T, Tys>...>;
        using tparams = info<pack::tparams_t<Tys>...>;
        template<template<class...> class T> using as = T<Tys...>;
        using uninstantiate = info<pack::uninstantiate_t<Tys>...>;
        template<class T> using reinstantiate = info<pack::reinstantiate_t<T, Tys>...>;

        constexpr static auto for_each = []<class Ty>(Ty && lambda) {
            return lambda.template operator()<Tys...>();
        };

        template<auto Filter> struct when {
            template<template<class...> class T>
            using transform = pack::conditional_transform_t<Filter, T, info>;
        };

        // Pack info

        template<class Ty> struct _element_is_info { using type = info<Ty>; };
        template<class ...Tys> struct _element_is_info<info<Tys...>> { using type = info<Tys...>; };

        template<std::size_t I> using element = _element_is_info<pack::element_t<I, info<Tys...>>>::type;
        template<class    Ty> constexpr static bool contains = pack::contains_v<Ty, info<Tys...>>;
        template<class ...As> constexpr static bool contains_all = pack::contains_all_v<pack::detail::convert_to_info<As...>, info<Tys...>>;
        template<class ...As> constexpr static bool contains_any = pack::contains_any_v<pack::detail::convert_to_info<As...>, info<Tys...>>;
        template<class    Ty> constexpr static std::size_t count = pack::count_v<Ty, info<Tys...>>;
        template<class ...As> constexpr static std::size_t count_all = pack::count_all_v<pack::detail::convert_to_info<As...>, info<Tys...>>;
        template<auto Filter> constexpr static std::size_t count_filter = pack::count_filter_v<Filter, info<Tys...>>;
        constexpr static std::size_t count_unique = unique_size;
        template<class     T> constexpr static std::size_t index = pack::index_v<T, info<Tys...>>;
        template<auto Filter> constexpr static std::size_t index_filter = pack::index_filter_v<Filter, info<Tys...>>;
        template<auto Filter> constexpr static std::size_t index_not_filter = pack::index_not_filter_v<Filter, info<Tys...>>;
        template<class ...As> constexpr static std::array<std::size_t, count_all<As...>> indices = pack::indices_v<pack::detail::convert_to_info<As...>, info<Tys...>>;
        template<class ...As> constexpr static std::array<std::size_t, size - count_all<As...>> indices_except = pack::indices_except_v<pack::detail::convert_to_info<As...>, info<Tys...>>;
        template<auto Filter> constexpr static std::array<std::size_t, count_filter<Filter>> indices_filter = pack::indices_filter_v<Filter, info<Tys...>>;
        template<auto Filter> constexpr static std::array<std::size_t, size - count_filter<Filter>> indices_except_filter = pack::indices_except_filter_v<Filter, info<Tys...>>;
        template<class ...As> constexpr static auto first_indices = pack::first_indices_v<pack::detail::convert_to_info<As...>, info<Tys...>>;
        template<class ...As> constexpr static std::size_t first_index = pack::first_index_v<pack::detail::convert_to_info<As...>, info<Tys...>>;

        // Pack manipulators

        using reverse = pack::reverse_t<info<Tys...>>;
        using unique = pack::unique_t<info<Tys...>>;
        using join = pack::join_t<info<Tys...>>;
        
        template<class   ...As> using split = pack::split_t<pack::detail::convert_to_info<As...>, info<Tys...>>;
        template<class   ...As> using split_after = pack::split_after_t<pack::detail::convert_to_info<As...>, info<Tys...>>;
        template<class   ...As> using split_before = pack::split_before_t<pack::detail::convert_to_info<As...>, info<Tys...>>;
        template<auto   Filter> using split_filter = pack::split_filter_t<Filter, info<Tys...>>;
        template<auto   Filter> using split_after_filter = pack::split_after_filter_t<Filter, info<Tys...>>;
        template<auto   Filter> using split_before_filter = pack::split_before_filter_t<Filter, info<Tys...>>;
        template<std::size_t A, std::size_t B> using sub = pack::sub_t<A, B, info<Tys...>>;
        template<std::size_t I> using take = pack::take_t<I, info<Tys...>>;
        template<auto   Filter> using take_while = pack::take_while_t<Filter, info<Tys...>>;
        template<std::size_t I> using drop = pack::drop_t<I, info<Tys...>>;
        template<auto   Filter> using drop_while = pack::drop_while_t<Filter, info<Tys...>>;
        template<std::size_t I> using take_last = pack::take_last_t<I, info<Tys...>>;
        template<auto   Filter> using take_last_while = pack::take_last_while_t<Filter, info<Tys...>>;
        template<std::size_t I> using drop_last = pack::drop_last_t<I, info<Tys...>>;
        template<auto   Filter> using drop_last_while = pack::drop_last_while_t<Filter, info<Tys...>>;
        template<auto    ...Is> using keep_indices = pack::keep_indices_t<pack::detail::convert_to_array<Is...>, info<Tys...>>;
        template<class   ...As> using keep = pack::keep_t<pack::detail::convert_to_info<As...>, info<Tys...>>;
        template<auto    ...Is> using remove_indices = pack::remove_indices_t<pack::detail::convert_to_array<Is...>, info<Tys...>>;
        template<class   ...As> using remove = pack::remove_t<pack::detail::convert_to_info<As...>, info<Tys...>>;
        template<class   ...As> using append = pack::append_t<pack::detail::convert_to_info<As...>, info<Tys...>>;
        template<class   ...As> using prepend = pack::prepend_t<pack::detail::convert_to_info<As...>, info<Tys...>>;
        template<std::size_t I> using erase = pack::erase_t<I, info<Tys...>>;
        template<std::size_t I, class   ...As> using insert = pack::insert_t<I, pack::detail::convert_to_info<As...>, info<Tys...>>;
        template<std::size_t I, class       R> using swap = pack::swap_t<I, R, info<Tys...>>;
        template<class       R, class   ...As> using replace = pack::replace_t<R, pack::detail::convert_to_info<As...>, info<Tys...>>;
        template<class       R, auto   Filter> using replace_filter = pack::replace_filter_t<R, Filter, info<Tys...>>;
        template<auto   Filter> using filter = pack::filter_t<Filter, info<Tys...>>;
        template<auto   Sorter> using sort = pack::sort_t<Sorter, info<Tys...>>;

        // Pack combiners

        template<class ...Args> using concat = pack::concat_t<info<Tys...>, Args...>;
        template<class ...Args> using zip = pack::zip_t<info<Tys...>, Args...>;
        template<class ...Args> using cartesian = pack::cartesian_t<info, Args...>;

        // Type traits

        using is_void = info<value_t<std::is_void_v<Tys>>...>;
        using is_null_pointer = info<value_t<std::is_null_pointer_v<Tys>>...>;
        using is_integral = info<value_t<std::is_integral_v<Tys>>...>;
        using is_floating_point = info<value_t<std::is_floating_point_v<Tys>>...>;
        using is_array = info<value_t<std::is_array_v<Tys>>...>;
        using is_enum = info<value_t<std::is_enum_v<Tys>>...>;
        using is_union = info<value_t<std::is_union_v<Tys>>...>;
        using is_class = info<value_t<std::is_class_v<Tys>>...>;
        using is_function = info<value_t<std::is_function_v<Tys>>...>;
        using is_pointer = info<value_t<std::is_pointer_v<Tys>>...>;
        using is_lvalue_reference = info<value_t<std::is_lvalue_reference_v<Tys>>...>;
        using is_rvalue_reference = info<value_t<std::is_rvalue_reference_v<Tys>>...>;
        using is_member_object_pointer = info<value_t<std::is_member_object_pointer_v<Tys>>...>;
        using is_member_function_pointer = info<value_t<std::is_member_function_pointer_v<Tys>>...>;
        using is_fundamental = info<value_t<std::is_fundamental_v<Tys>>...>;
        using is_arithmetic = info<value_t<std::is_arithmetic_v<Tys>>...>;
        using is_scalar = info<value_t<std::is_scalar_v<Tys>>...>;
        using is_object = info<value_t<std::is_object_v<Tys>>...>;
        using is_compound = info<value_t<std::is_compound_v<Tys>>...>;
        using is_reference = info<value_t<std::is_reference_v<Tys>>...>;
        using is_member_pointer = info<value_t<std::is_member_pointer_v<Tys>>...>;
        using is_const = info<value_t<std::is_const_v<Tys>>...>;
        using is_volatile = info<value_t<std::is_volatile_v<Tys>>...>;
        using is_trivial = info<value_t<std::is_trivial_v<Tys>>...>;
        using is_trivially_copyable = info<value_t<std::is_trivially_copyable_v<Tys>>...>;
        using is_standard_layout = info<value_t<std::is_standard_layout_v<Tys>>...>;
        using is_empty = info<value_t<std::is_empty_v<Tys>>...>;
        using is_polymorphic = info<value_t<std::is_polymorphic_v<Tys>>...>;
        using is_abstract = info<value_t<std::is_abstract_v<Tys>>...>;
        using is_final = info<value_t<std::is_final_v<Tys>>...>;
        using is_aggregate = info<value_t<std::is_aggregate_v<Tys>>...>;
        using is_signed_integral = info<value_t<std::is_signed_v<Tys>>...>;
        using is_unsigned_integral = info<value_t<std::is_unsigned_v<Tys>>...>;
        using is_bounded_array = info<value_t<std::is_bounded_array_v<Tys>>...>;
        using is_unbounded_array = info<value_t<std::is_unbounded_array_v<Tys>>...>;
        using is_default_constructible = info<value_t<std::is_default_constructible_v<Tys>>...>;
        using is_trivially_default_constructible = info<value_t<std::is_trivially_default_constructible_v<Tys>>...>;
        using is_nothrow_default_constructible = info<value_t<std::is_nothrow_default_constructible_v<Tys>>...>;
        using is_copy_constructible = info<value_t<std::is_copy_constructible_v<Tys>>...>;
        using is_trivially_copy_constructible = info<value_t<std::is_trivially_copy_constructible_v<Tys>>...>;
        using is_nothrow_copy_constructible = info<value_t<std::is_nothrow_copy_constructible_v<Tys>>...>;
        using is_move_constructible = info<value_t<std::is_move_constructible_v<Tys>>...>;
        using is_trivially_move_constructible = info<value_t<std::is_trivially_move_constructible_v<Tys>>...>;
        using is_nothrow_move_constructible = info<value_t<std::is_nothrow_move_constructible_v<Tys>>...>;
        using is_copy_assignable = info<value_t<std::is_copy_assignable_v<Tys>>...>;
        using is_trivially_copy_assignable = info<value_t<std::is_trivially_copy_assignable_v<Tys>>...>;
        using is_nothrow_copy_assignable = info<value_t<std::is_nothrow_copy_assignable_v<Tys>>...>;
        using is_move_assignable = info<value_t<std::is_move_assignable_v<Tys>>...>;
        using is_trivially_move_assignable = info<value_t<std::is_trivially_move_assignable_v<Tys>>...>;
        using is_nothrow_move_assignable = info<value_t<std::is_nothrow_move_assignable_v<Tys>>...>;
        using is_destructible = info<value_t<std::is_destructible_v<Tys>>...>;
        using is_trivially_destructible = info<value_t<std::is_trivially_destructible_v<Tys>>...>;
        using is_nothrow_destructible = info<value_t<std::is_nothrow_destructible_v<Tys>>...>;
        using is_swappable = info<value_t<std::is_swappable_v<Tys>>...>;
        using is_nothrow_swappable = info<value_t<std::is_nothrow_swappable_v<Tys>>...>;

        using has_unique_object_representations = info<value_t<std::has_unique_object_representations_v<Tys>>...>;
        using has_virtual_destructors = info<value_t<std::has_virtual_destructor_v<Tys>>...>;

        template<class Other> using is_assignable = info<value_t<std::is_assignable_v<Tys, Other>>...>;
        template<class Other> using is_trivially_assignable = info<value_t<std::is_trivially_assignable_v<Tys, Other>>...>;
        template<class Other> using is_nothrow_assignable = info<value_t<std::is_nothrow_assignable_v<Tys, Other>>...>;
        template<class Other> using is_swappable_with = info<value_t<std::is_swappable_with_v<Tys, Other>>...>;
        template<class Other> using is_nothrow_swappable_with = info<value_t<std::is_nothrow_swappable_with_v<Tys, Other>>...>;

        template<class Other> using is_same = info<value_t<std::is_same_v<Tys, Other>>...>;
        template<class Other> using is_base_of = info<value_t<std::is_base_of_v<Tys, Other>>...>;
        template<class Other> using is_convertible_to = info<value_t<std::is_convertible_v<Tys, Other>>...>;
        template<class Other> using is_nothrow_convertible_to = info<value_t<std::is_nothrow_convertible_v<Tys, Other>>...>;

        template<class ...Args> using is_constructible = info<value_t<std::is_constructible_v<Tys, Args...>>...>;
        template<class ...Args> using is_trivially_constructible = info<value_t<std::is_trivially_constructible_v<Tys, Args...>>...>;
        template<class ...Args> using is_nothrow_constructible = info<value_t<std::is_nothrow_constructible_v<Tys, Args...>>...>;
        template<class ...Args> using is_invocable = info<value_t<std::is_invocable_v<Tys, Args...>>...>;
        template<class ...Args> using is_nothrow_invocable = info<value_t<std::is_nothrow_invocable_v<Tys, Args...>>...>;

        template<class Ty> using can_construct = info<value_t<std::is_constructible_v<Ty, Tys...>>>;
        template<class Ty> using can_trivially_construct = info<value_t<std::is_trivially_constructible_v<Ty, Tys...>>>;
        template<class Ty> using can_nothrow_construct = info<value_t<std::is_nothrow_constructible_v<Ty, Tys...>>>;
        template<class Ty> using can_invoke = info<value_t<std::is_invocable_v<Ty, Tys...>>>;
        template<class Ty> using can_nothrow_invoke = info<value_t<std::is_nothrow_invocable_v<Ty, Tys...>>>;

        // Qualifiers

        // Copy modifiers from From to ...Tys
        template<class From>
        using copy_const_from = info<kaixo::copy_const_t<Tys, From>...>;
        template<class From>
        using copy_volatile_from = info<kaixo::copy_volatile_t<Tys, From>...>;
        template<class From>
        using copy_cv_from = info<kaixo::copy_cv_t<Tys, From>...>;
        template<class From>
        using copy_ref_from = info<kaixo::copy_ref_t<Tys, From>...>;
        template<class From>
        using copy_cvref_from = info<kaixo::copy_cvref_t<Tys, From>...>;

        // Add modifiers of From to ...Tys
        template<class From>
        using add_const_from = info<kaixo::add_const_t<Tys, From>...>;
        template<class From>
        using add_volatile_from = info<kaixo::add_volatile_t<Tys, From>...>;
        template<class From>
        using add_cv_from = info<kaixo::add_cv_t<Tys, From>...>;
        template<class From>
        using add_ref_from = info<kaixo::add_ref_t<Tys, From>...>;
        template<class From>
        using add_cvref_from = info<kaixo::add_cvref_t<Tys, From>...>;

        // Copy modifiers from ...Tys to To
        template<class To>
        using copy_const_to = info<kaixo::copy_const_t<To, Tys>...>;
        template<class To>
        using copy_volatile_to = info<kaixo::copy_volatile_t<To, Tys>...>;
        template<class To>
        using copy_cv_to = info<kaixo::copy_cv_t<To, Tys>...>;
        template<class To>
        using copy_ref_to = info<kaixo::copy_ref_t<To, Tys>...>;
        template<class To>
        using copy_cvref_to = info<kaixo::copy_cvref_t<To, Tys>...>;

        // Add modifiers of ...Tys to To
        template<class To>
        using add_const_to = info<kaixo::add_const_t<To, Tys>...>;
        template<class To>
        using add_volatile_to = info<kaixo::add_volatile_t<To, Tys>...>;
        template<class To>
        using add_cv_to = info<kaixo::add_cv_t<To, Tys>...>;
        template<class To>
        using add_ref_to = info<kaixo::add_ref_t<To, Tys>...>;
        template<class To>
        using add_cvref_to = info<kaixo::add_cvref_t<To, Tys>...>;

        using decay = info<kaixo::decay_t<Tys>...>;
        using remove_cv = info<kaixo::remove_cv_t<Tys>...>;
        using remove_const = info<kaixo::remove_const_t<Tys>...>;
        using remove_volatile = info<kaixo::remove_volatile_t<Tys>...>;
        using add_cv = info<kaixo::add_cv_t<Tys>...>;
        using add_const = info<kaixo::add_const_t<Tys>...>;
        using add_volatile = info<kaixo::add_volatile_t<Tys>...>;
        using remove_reference = info<kaixo::remove_reference_t<Tys>...>;
        using remove_cvref = info<kaixo::remove_cvref_t<Tys>...>;
        using add_lvalue_reference = info<kaixo::add_lvalue_reference_t<Tys>...>;
        using add_rvalue_reference = info<kaixo::add_rvalue_reference_t<Tys>...>;
        using remove_pointer = info<kaixo::remove_pointer_t<Tys>...>;
        using add_pointer = info<kaixo::add_pointer_t<Tys>...>;

        template<class Ty> using to_function_args = info<Ty(Tys...)>;
        template<class Ty> using to_member_pointer = info<kaixo::remove_reference_t<Tys> Ty::* ...>;
    };

    /**
     * Info object of value_t's.
     * @tparam ...Vs non-type template parameters
     */
    template<auto ...Vs>
    using info_v = info<value_t<Vs>...>;

    /**
     * Info object of templated types.
     * @tparam ...Tys templated types
     */
    template<template<class...> class ...Tys>
    using info_t = info<templated_t<Tys>...>;

    /**
     * Template parameters of Ty to info.
     * @tparam Ty templated type
     */
    template<class Ty>
    using as_info = pack::copy_tparams_t<info, decay_t<Ty>>;
}