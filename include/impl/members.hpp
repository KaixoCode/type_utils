#pragma once
#include "type_traits.hpp"

/**
 * Check for or grab certain members on an object.
 * Most members in the standard are available here.
 */
namespace kaixo {
    template<class Ty> concept has_type = requires (Ty) { typename Ty::type; };
    template<class Ty> concept has_value = requires (Ty) { Ty::value; };
    template<class Ty> concept has_size = requires (Ty) { Ty::size; };
    template<class Ty> concept has_off_type = requires (Ty) { typename Ty::off_type; };
    template<class Ty> concept has_state_type = requires (Ty) { typename Ty::state_type; };
    template<class Ty> concept has_int_type = requires (Ty) { typename Ty::int_type; };
    template<class Ty> concept has_pos_type = requires (Ty) { typename Ty::pos_type; };
    template<class Ty> concept has_char_type = requires (Ty) { typename Ty::char_type; };
    template<class Ty> concept has_comparison_category = requires (Ty) { typename Ty::comparison_category; };
    template<class Ty> concept has_traits_type = requires (Ty) { typename Ty::traits_type; };
    template<class Ty> concept has_string_type = requires (Ty) { typename Ty::string_type; };
    template<class Ty> concept has_format = requires (Ty) { typename Ty::format; };
    template<class Ty> concept has_iterator_category = requires (Ty) { typename Ty::iterator_category; };
    template<class Ty> concept has_iterator_concept = requires (Ty) { typename Ty::iterator_concept; };
    template<class Ty> concept has_key_type = requires (Ty) { typename Ty::key_type; };
    template<class Ty> concept has_mapped_type = requires (Ty) { typename Ty::mapped_type; };
    template<class Ty> concept has_key_compare = requires (Ty) { typename Ty::key_compare; };
    template<class Ty> concept has_value_compare = requires (Ty) { typename Ty::value_compare; };
    template<class Ty> concept has_node_type = requires (Ty) { typename Ty::node_type; };
    template<class Ty> concept has_insert_return_type = requires (Ty) { typename Ty::insert_return_type; };
    template<class Ty> concept has_value_type = requires (Ty) { typename Ty::value_type; };
    template<class Ty> concept has_allocator_type = requires (Ty) { typename Ty::allocator_type; };
    template<class Ty> concept has_size_type = requires (Ty) { typename Ty::size_type; };
    template<class Ty> concept has_difference_type = requires (Ty) { typename Ty::difference_type; };
    template<class Ty> concept has_reference = requires (Ty) { typename Ty::reference; };
    template<class Ty> concept has_const_reference = requires (Ty) { typename Ty::const_reference; };
    template<class Ty> concept has_pointer = requires (Ty) { typename Ty::pointer; };
    template<class Ty> concept has_const_pointer = requires (Ty) { typename Ty::const_pointer; };
    template<class Ty> concept has_iterator = requires (Ty) { typename Ty::iterator; };
    template<class Ty> concept has_const_iterator = requires (Ty) { typename Ty::const_iterator; };
    template<class Ty> concept has_reverse_iterator = requires (Ty) { typename Ty::reverse_iterator; };
    template<class Ty> concept has_const_reverse_iterator = requires (Ty) { typename Ty::const_reverse_iterator; };

    namespace has {
        template<class Ty> struct type_impl : std::bool_constant<has_type<Ty>> {};
        template<class Ty> struct value_impl : std::bool_constant<has_value<Ty>> {};
        template<class Ty> struct size_impl : std::bool_constant<has_size<Ty>> {};
        template<class Ty> struct off_type_impl : std::bool_constant<has_off_type<Ty>> {};
        template<class Ty> struct state_type_impl : std::bool_constant<has_state_type<Ty>> {};
        template<class Ty> struct int_type_impl : std::bool_constant<has_int_type<Ty>> {};
        template<class Ty> struct pos_type_impl : std::bool_constant<has_pos_type<Ty>> {};
        template<class Ty> struct char_type_impl : std::bool_constant<has_char_type<Ty>> {};
        template<class Ty> struct comparison_category_impl : std::bool_constant<has_comparison_category<Ty>> {};
        template<class Ty> struct traits_type_impl : std::bool_constant<has_traits_type<Ty>> {};
        template<class Ty> struct string_type_impl : std::bool_constant<has_string_type<Ty>> {};
        template<class Ty> struct format_impl : std::bool_constant<has_format<Ty>> {};
        template<class Ty> struct iterator_category_impl : std::bool_constant<has_iterator_category<Ty>> {};
        template<class Ty> struct iterator_concept_impl : std::bool_constant<has_iterator_concept<Ty>> {};
        template<class Ty> struct key_type_impl : std::bool_constant<has_key_type<Ty>> {};
        template<class Ty> struct mapped_type_impl : std::bool_constant<has_mapped_type<Ty>> {};
        template<class Ty> struct key_compare_impl : std::bool_constant<has_key_compare<Ty>> {};
        template<class Ty> struct value_compare_impl : std::bool_constant<has_value_compare<Ty>> {};
        template<class Ty> struct node_type_impl : std::bool_constant<has_node_type<Ty>> {};
        template<class Ty> struct insert_return_type_impl : std::bool_constant<has_insert_return_type<Ty>> {};
        template<class Ty> struct value_type_impl : std::bool_constant<has_value_type<Ty>> {};
        template<class Ty> struct allocator_type_impl : std::bool_constant<has_allocator_type<Ty>> {};
        template<class Ty> struct size_type_impl : std::bool_constant<has_size_type<Ty>> {};
        template<class Ty> struct difference_type_impl : std::bool_constant<has_difference_type<Ty>> {};
        template<class Ty> struct reference_impl : std::bool_constant<has_reference<Ty>> {};
        template<class Ty> struct const_reference_impl : std::bool_constant<has_const_reference<Ty>> {};
        template<class Ty> struct pointer_impl : std::bool_constant<has_pointer<Ty>> {};
        template<class Ty> struct const_pointer_impl : std::bool_constant<has_const_pointer<Ty>> {};
        template<class Ty> struct iterator_impl : std::bool_constant<has_iterator<Ty>> {};
        template<class Ty> struct const_iterator_impl : std::bool_constant<has_const_iterator<Ty>> {};
        template<class Ty> struct reverse_iterator_impl : std::bool_constant<has_reverse_iterator<Ty>> {};
        template<class Ty> struct const_reverse_iterator_impl : std::bool_constant<has_const_reverse_iterator<Ty>> {};

        constexpr auto type = type_trait<type_impl>{};
        constexpr auto value = type_trait<value_impl>{};
        constexpr auto size = type_trait<size_impl>{};
        constexpr auto off_type = type_trait<off_type_impl>{};
        constexpr auto state_type = type_trait<state_type_impl>{};
        constexpr auto int_type = type_trait<int_type_impl>{};
        constexpr auto pos_type = type_trait<pos_type_impl>{};
        constexpr auto char_type = type_trait<char_type_impl>{};
        constexpr auto comparison_category = type_trait<comparison_category_impl>{};
        constexpr auto traits_type = type_trait<traits_type_impl>{};
        constexpr auto string_type = type_trait<string_type_impl>{};
        constexpr auto format = type_trait<format_impl>{};
        constexpr auto iterator_category = type_trait<iterator_category_impl>{};
        constexpr auto iterator_concept = type_trait<iterator_concept_impl>{};
        constexpr auto key_type = type_trait<key_type_impl>{};
        constexpr auto mapped_type = type_trait<mapped_type_impl>{};
        constexpr auto key_compare = type_trait<key_compare_impl>{};
        constexpr auto value_compare = type_trait<value_compare_impl>{};
        constexpr auto node_type = type_trait<node_type_impl>{};
        constexpr auto insert_return_type = type_trait<insert_return_type_impl>{};
        constexpr auto value_type = type_trait<value_type_impl>{};
        constexpr auto allocator_type = type_trait<allocator_type_impl>{};
        constexpr auto size_type = type_trait<size_type_impl>{};
        constexpr auto difference_type = type_trait<difference_type_impl>{};
        constexpr auto reference = type_trait<reference_impl>{};
        constexpr auto const_reference = type_trait<const_reference_impl>{};
        constexpr auto pointer = type_trait<pointer_impl>{};
        constexpr auto const_pointer = type_trait<const_pointer_impl>{};
        constexpr auto iterator = type_trait<iterator_impl>{};
        constexpr auto const_iterator = type_trait<const_iterator_impl>{};
        constexpr auto reverse_iterator = type_trait<reverse_iterator_impl>{};
        constexpr auto const_reverse_iterator = type_trait<const_reverse_iterator_impl>{};
    }

    namespace grab {
        template<class Ty> struct type_impl { using type = Ty; };
        template<has_type Ty> struct type_impl<Ty> { using type = typename Ty::type; };
        template<class Ty> struct value_impl { using type = Ty; };
        template<has_value Ty> struct value_impl<Ty> { using type = value_t<Ty::value>; };
        template<class Ty> struct size_impl { using type = Ty; };
        template<has_size Ty> struct size_impl<Ty> { using type = value_t<Ty::size>; };
        template<class Ty> struct off_type_impl { using type = Ty; };
        template<has_off_type Ty> struct off_type_impl<Ty> { using type = typename Ty::off_type; };
        template<class Ty> struct state_type_impl { using type = Ty; };
        template<has_state_type Ty> struct state_type_impl<Ty> { using type = typename Ty::state_type; };
        template<class Ty> struct int_type_impl { using type = Ty; };
        template<has_int_type Ty> struct int_type_impl<Ty> { using type = typename Ty::int_type; };
        template<class Ty> struct pos_type_impl { using type = Ty; };
        template<has_pos_type Ty> struct pos_type_impl<Ty> { using type = typename Ty::pos_type; };
        template<class Ty> struct char_type_impl { using type = Ty; };
        template<has_char_type Ty> struct char_type_impl<Ty> { using type = typename Ty::char_type; };
        template<class Ty> struct comparison_category_impl { using type = Ty; };
        template<has_comparison_category Ty> struct comparison_category_impl<Ty> { using type = typename Ty::comparison_category; };
        template<class Ty> struct traits_type_impl { using type = Ty; };
        template<has_traits_type Ty> struct traits_type_impl<Ty> { using type = typename Ty::traits_type; };
        template<class Ty> struct string_type_impl { using type = Ty; };
        template<has_string_type Ty> struct string_type_impl<Ty> { using type = typename Ty::string_type; };
        template<class Ty> struct format_impl { using type = Ty; };
        template<has_format Ty> struct format_impl<Ty> { using type = typename Ty::format; };
        template<class Ty> struct iterator_category_impl { using type = Ty; };
        template<has_iterator_category Ty> struct iterator_category_impl<Ty> { using type = typename Ty::iterator_category; };
        template<class Ty> struct iterator_concept_impl { using type = Ty; };
        template<has_iterator_concept Ty> struct iterator_concept_impl<Ty> { using type = typename Ty::iterator_concept; };
        template<class Ty> struct key_type_impl { using type = Ty; };
        template<has_key_type Ty> struct key_type_impl<Ty> { using type = typename Ty::key_type; };
        template<class Ty> struct mapped_type_impl { using type = Ty; };
        template<has_mapped_type Ty> struct mapped_type_impl<Ty> { using type = typename Ty::mapped_type; };
        template<class Ty> struct key_compare_impl { using type = Ty; };
        template<has_key_compare Ty> struct key_compare_impl<Ty> { using type = typename Ty::key_compare; };
        template<class Ty> struct value_compare_impl { using type = Ty; };
        template<has_value_compare Ty> struct value_compare_impl<Ty> { using type = typename Ty::value_compare; };
        template<class Ty> struct node_type_impl { using type = Ty; };
        template<has_node_type Ty> struct node_type_impl<Ty> { using type = typename Ty::node_type; };
        template<class Ty> struct insert_return_type_impl { using type = Ty; };
        template<has_insert_return_type Ty> struct insert_return_type_impl<Ty> { using type = typename Ty::insert_return_type; };
        template<class Ty> struct value_type_impl { using type = Ty; };
        template<has_value_type Ty> struct value_type_impl<Ty> { using type = typename Ty::value_type; };
        template<class Ty> struct allocator_type_impl { using type = Ty; };
        template<has_allocator_type Ty> struct allocator_type_impl<Ty> { using type = typename Ty::allocator_type; };
        template<class Ty> struct size_type_impl { using type = Ty; };
        template<has_size_type Ty> struct size_type_impl<Ty> { using type = typename Ty::size_type; };
        template<class Ty> struct difference_type_impl { using type = Ty; };
        template<has_difference_type Ty> struct difference_type_impl<Ty> { using type = typename Ty::difference_type; };
        template<class Ty> struct reference_impl { using type = Ty; };
        template<has_reference Ty> struct reference_impl<Ty> { using type = typename Ty::reference; };
        template<class Ty> struct const_reference_impl { using type = Ty; };
        template<has_const_reference Ty> struct const_reference_impl<Ty> { using type = typename Ty::const_reference; };
        template<class Ty> struct pointer_impl { using type = Ty; };
        template<has_pointer Ty> struct pointer_impl<Ty> { using type = typename Ty::pointer; };
        template<class Ty> struct const_pointer_impl { using type = Ty; };
        template<has_const_pointer Ty> struct const_pointer_impl<Ty> { using type = typename Ty::const_pointer; };
        template<class Ty> struct iterator_impl { using type = Ty; };
        template<has_iterator Ty> struct iterator_impl<Ty> { using type = typename Ty::iterator; };
        template<class Ty> struct const_iterator_impl { using type = Ty; };
        template<has_const_iterator Ty> struct const_iterator_impl<Ty> { using type = typename Ty::const_iterator; };
        template<class Ty> struct reverse_iterator_impl { using type = Ty; };
        template<has_reverse_iterator Ty> struct reverse_iterator_impl<Ty> { using type = typename Ty::reverse_iterator; };
        template<class Ty> struct const_reverse_iterator_impl { using type = Ty; };
        template<has_const_reverse_iterator Ty> struct const_reverse_iterator_impl<Ty> { using type = typename Ty::const_reverse_iterator; };

        template<class Ty> using type = type_impl<Ty>::type;
        template<class Ty> using value = value_impl<Ty>::type;
        template<class Ty> using size = size_impl<Ty>::type;
        template<class Ty> using off_type = off_type_impl<Ty>::type;
        template<class Ty> using state_type = state_type_impl<Ty>::type;
        template<class Ty> using int_type = int_type_impl<Ty>::type;
        template<class Ty> using pos_type = pos_type_impl<Ty>::type;
        template<class Ty> using char_type = char_type_impl<Ty>::type;
        template<class Ty> using comparison_category = comparison_category_impl<Ty>::type;
        template<class Ty> using traits_type = traits_type_impl<Ty>::type;
        template<class Ty> using string_type = string_type_impl<Ty>::type;
        template<class Ty> using format = format_impl<Ty>::type;
        template<class Ty> using iterator_category = iterator_category_impl<Ty>::type;
        template<class Ty> using iterator_concept = iterator_concept_impl<Ty>::type;
        template<class Ty> using key_type = key_type_impl<Ty>::type;
        template<class Ty> using mapped_type = mapped_type_impl<Ty>::type;
        template<class Ty> using key_compare = key_compare_impl<Ty>::type;
        template<class Ty> using value_compare = value_compare_impl<Ty>::type;
        template<class Ty> using node_type = node_type_impl<Ty>::type;
        template<class Ty> using insert_return_type = insert_return_type_impl<Ty>::type;
        template<class Ty> using value_type = value_type_impl<Ty>::type;
        template<class Ty> using allocator_type = allocator_type_impl<Ty>::type;
        template<class Ty> using size_type = size_type_impl<Ty>::type;
        template<class Ty> using difference_type = difference_type_impl<Ty>::type;
        template<class Ty> using reference = reference_impl<Ty>::type;
        template<class Ty> using const_reference = const_reference_impl<Ty>::type;
        template<class Ty> using pointer = pointer_impl<Ty>::type;
        template<class Ty> using const_pointer = const_pointer_impl<Ty>::type;
        template<class Ty> using iterator = iterator_impl<Ty>::type;
        template<class Ty> using const_iterator = const_iterator_impl<Ty>::type;
        template<class Ty> using reverse_iterator = reverse_iterator_impl<Ty>::type;
        template<class Ty> using const_reverse_iterator = const_reverse_iterator_impl<Ty>::type;
    }
}