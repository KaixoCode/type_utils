#pragma once
#include "type_traits.hpp"
#include "struct.hpp"

namespace kaixo {
    /**
     * Type manipulator:
     * - transform<T<...>>                      Transform type to T<Ty>
     * - conditional_transform<Filter, T<...>>  Conditionally transform to T<Ty> if match Filter
     * - tparams                                Get the template parameters of Ty
     * - copy_tparams<T<...>>                   Copy tparams to T<Tys...>
     * - uninstantiate                          Remove the template parameters from Ty
     * - instantiate<T<...>>                    Add template parameters to T<Tys...>
     * - reinstantiate<T>                       Replace the template parameters from T like T<Tys...>
     * 
     * Pack info:
     * - element<I>                             Get I'th element
     * - contains<Ty>                           Contains Ty
     * - contains_all<Tys...>                   Contains all of Tys...
     * - contains_any<Tys...>                   Contains any of Tys...
     * - count<Ty>                              Number of occurences of Ty
     * - count_all<Tys...>                      Number of occurences of all Tys...
     * - count_filter<Filter>                   Number of occurences that match Filter
     * - count_unique                           Number of unique types
     * - index<Ty>                              First index of Ty
     * - index_filter<Filter>                   First index that matches Filter
     * - index_not_filter<Filter>               First index that does not match Filter
     * - indices<Tys...>                        All indices of all Tys...
     * - indices_except<Tys...>                 All indices except all of Tys...
     * - indices_filter<Filter>                 All indices that match Filter
     * - indices_except_filter<Filter>          All indices except all that match Filter
     * - first_index<Tys...>                    First index of any in Tys...
     * - first_indices<Tys...>                  First indices of all Tys...
     * 
     * Pack manipulators:
     * - reverse                                Reverse pack
     * - unique                                 Only keep first occurence of type
     * - join                                   Flatten pack of packs
     * - split<Tys...>                          Split pack at all Tys... (consumes)
     * - split_after<Tys...>                    Split pack after all Tys... (does not consume)
     * - split_before<Tys...>                   Split pack before all Tys... (does not consume)
     * - split_filter<Filter>                   Split pack at all Filter matches (consumes)
     * - split_after_filter<Filter>             Split pack after all Filter matches (does not consume)
     * - split_before_filter<Filter>            Split pack before all Filter matches (does not consume)
     * - sub<A, B>                              Only keep indices between A and B
     * - take<I>                                Take first I types
     * - take_while<Filter>                     Take while Filter matches
     * - drop<I>                                Drop first I types
     * - drop_while<Filter>                     Drop while Filter matches
     * - take_last<I>                           Take last I types
     * - take_last_while<Filter>                Take last while Filter matches
     * - drop_last<I>                           Drop last I types
     * - drop_last_while<Filter>                Drop last while Filter matches
     * - keep_indices<Is...>                    Only keep indices Is...
     * - keep<Tys...>                           Only keep types Tys...
     * - remove_indices<Is...>                  Remove indices Is...
     * - remove<Tys...>                         Remove types Tys...
     * - append<Tys...>                         Append types Tys...
     * - prepend<Tys...>                        Prepend types Tys...
     * - erase<I>                               Remove index I
     * - insert<I, Tys...>                      Insert types Tys... at index I
     * - swap<I, B>                             Swap index I with B
     * - replace<A, B...>                       Replace all B... with A
     * - replace_filter<A, Filter>              Replace Filter matches with A
     * - filter<Filter>                         Only keep types that match Filter
     * - sort<Sorter>                           Sort the types using the Sorter
     *                                          
     * Pack combiners:                          
     * - concat<Tys...>                         Concat all packs Tys...
     * - zip<Tys...>                            Zip all packs Tys...
     * - cartesian<Tys...>                      Cartesian product of all packs Tys...
     * 
     */
    namespace pack {

        // =======================================================

        template<auto Array, template<std::size_t ...> class Ty>
        struct array_to_pack {
            template<class> struct helper;
            template<std::size_t ...Is>
            struct helper<std::index_sequence<Is...>> {
                using type = Ty<Array[Is]...>;
            };

            using type = typename helper<std::make_index_sequence<Array.size()>>::type;
        };

        /**
         * Convert an array of indices to a template pack.
         * @tparam Array array of std::size_t
         * @tparam Ty templated type that takes std::size_t's
         */
        template<auto Array, template<std::size_t ...> class Ty>
        using array_to_pack_t = typename array_to_pack<Array, Ty>::type;

        // =======================================================

        /**
         * Convert template pack of indices to an array.
         * @tparam ...Is indices
         */
        template<std::size_t ...Is>
        constexpr std::array<std::size_t, sizeof...(Is)> as_array{ Is... };

        // =======================================================

        namespace detail {
            template<auto ...Is>
            struct convert_to_array_impl;

            template<auto Array>
                requires (!std::integral<decltype(Array)>)
            struct convert_to_array_impl<Array> {
                constexpr static auto value = Array;
            };

            template<auto ...Is>
                requires (std::integral<decltype(Is)> && ...)
            struct convert_to_array_impl<Is...> {
                constexpr static auto value = as_array<Is...>;
            };

            template<auto ...Is>
            constexpr auto convert_to_array = convert_to_array_impl<Is...>::value;

            template<class ...Args>
            struct convert_to_info_impl {
                using type = info<Args...>;
            };

            template<class ...Args>
            struct convert_to_info_impl<info<Args...>> {
                using type = info<Args...>;
            };

            template<class ...Args>
            using convert_to_info = convert_to_info_impl<Args...>::type;

            template<template<class...> class Ty, class A, class ...Args>
            struct apply_pack : Ty<A, Args...> {};
            template<template<class...> class Ty, class A, class ...Args>
            struct apply_pack<Ty, A, info<Args...>> : Ty<A, Args...> {};
            
            template<template<template<class...> class, class...> class Ty, template<class...> class A, class ...Args>
            struct apply_pack_t : Ty<A, Args...> {};
            template<template<template<class...> class, class...> class Ty, template<class...> class A, class ...Args>
            struct apply_pack_t<Ty, A, info<Args...>> : Ty<A, Args...> {};
            
            template<template<auto, template<class...> class, class...> class Ty, auto V, template<class...> class A, class ...Args>
            struct apply_pack_tv : Ty<V, A, Args...> {};
            template<template<auto, template<class...> class, class...> class Ty, auto V, template<class...> class A, class ...Args>
            struct apply_pack_tv<Ty, V, A, info<Args...>> : Ty<V, A, Args...> {};

            template<template<class...> class Ty, class A, class B, class ...Args>
            struct apply_pack2 : Ty<A, B, Args...> {};
            template<template<class...> class Ty, class A, class B, class ...Args>
            struct apply_pack2<Ty, A, B, info<Args...>> : Ty<A, B, Args...> {};

            template<template<class...> class Ty, class ...Args>
            struct apply_pack_d : Ty<Args...> {};
            template<template<class...> class Ty, class ...Args>
            struct apply_pack_d<Ty, info<Args...>> : Ty<Args...> {};

            template<template<auto, class...> class Ty, auto A, class ...Args>
            struct apply_pack_v : Ty<A, Args...> {};
            template<template<auto, class...> class Ty, auto A, class ...Args>
            struct apply_pack_v<Ty, A, info<Args...>> : Ty<A, Args...> {};

            template<template<auto, auto, class...> class Ty, auto A, auto B, class ...Args>
            struct apply_pack_v2 : Ty<A, B, Args...> {};
            template<template<auto, auto, class...> class Ty, auto A, auto B, class ...Args>
            struct apply_pack_v2<Ty, A, B, info<Args...>> : Ty<A, B, Args...> {};

            template<template<auto, class, class...> class Ty, auto A, class B, class ...Args>
            struct apply_pack_dv : Ty<A, B, Args...> {};
            template<template<auto, class, class...> class Ty, auto A, class B, class ...Args>
            struct apply_pack_dv<Ty, A, B, info<Args...>> : Ty<A, B, Args...> {};
        }

        // =======================================================

        /**
         * A filter object can be one of the following:
         *  - []<class Ty>{ return ... };                      return a boolean based on type
         *  - []<std::size_t Index, class Ty>{ return ... };   return a boolean based on index and type
         *  - []<std::size_t Index>{ return ... };             return a boolean based on index
         *  - []<concept constraint Ty>{};                     match concept constraint
         *  - type_filter specialization                       type filter evaluation
         *  - Type::value                                      std::bool_constant for example
         *  - value == Ty::value                               a value that will be compared
         *  - convertible_to<bool>                             any value that is convertible to bool
         */
        namespace detail {
            template<class L>
            struct filter_object_wrapper {
                consteval filter_object_wrapper(L value) : value(value) {}
                L value;
            };

            template<class L, std::size_t I, class Ty>
            concept _call_type0 = requires (L l) { // Only type -> bool
                { l.template operator() < Ty > () } -> concepts::convertible_to<bool>;
            };

            template<class L, std::size_t I, class Ty>
            concept _call_type1 = requires (L l) { // Index, type -> bool
                { l.template operator() < I, Ty > () } -> concepts::convertible_to<bool>;
            };

            template<class L, std::size_t I, class Ty>
            concept _call_type2 = requires (L l) { // Only index -> bool
                { l.template operator() < I > () } -> concepts::convertible_to<bool>;
            };

            template<class L, std::size_t I, class Ty>
            concept _call_type3 = requires (L l) { // Type -> void (type constraint)
                { l.template operator() < Ty > () } -> concepts::same_as<void>;
            };

            template<class L, std::size_t I, class Ty> // type filter
            concept _call_type4 = requires (L l) { { l.template evaluate<Ty>() } -> concepts::convertible_to<bool>; };

            template<class L, std::size_t I, class Ty> // Test value equality
            concept _call_type5 = requires(L l) { { l == Ty::value } -> concepts::convertible_to<bool>; };
        
            template<class L, std::size_t I, class Ty> // Boolean
            concept _call_type6 = std::convertible_to<L, bool>;

            template<class L, std::size_t I, class Ty> // bool_constant
            concept _call_type7 = requires(L l) { { L::value } -> concepts::convertible_to<bool>; };

            template<class L>
            struct filter_object : detail::filter_object_wrapper<L> {
                template<std::size_t I, class Ty> consteval bool call() {
                    if constexpr (detail::_call_type5<L, I, Ty>) return this->value == Ty::value;
                    else if constexpr (detail::_call_type6<L, I, Ty>) return this->value;
                    else if constexpr (detail::_call_type7<L, I, Ty>) return L::value;
                    else if constexpr (detail::_call_type0<L, I, Ty>) return this->value.template operator() < Ty > ();
                    else if constexpr (detail::_call_type1<L, I, Ty>) return this->value.template operator() < I, Ty > ();
                    else if constexpr (detail::_call_type2<L, I, Ty>) return this->value.template operator() < I > ();
                    else if constexpr (detail::_call_type3<L, I, Ty>) return true;
                    else if constexpr (detail::_call_type4<L, I, Ty>) return this->value.template evaluate<Ty>();
                    else return false; // Otherwise always return false
                }
            };

            template<class T> filter_object(T) -> filter_object<T>;
        }

        /**
         * Test whether Ty and optional index I match Filter.
         * @tparam Filter filter object
         * @tparam Ty type to filter
         * @tparam I index of type
         */
        template<auto Filter, class Ty, std::size_t I = 0>
        constexpr bool matches_filter = static_cast<bool>(detail::filter_object{ Filter }.template call<I, Ty>());

        // =======================================================

        template<class ...Tys>
            requires (sizeof...(Tys) <= 1)
        struct tparams;

        template<class Ty> 
        struct tparams<Ty> { 
            using type = Ty;
        };

        template<template<class...> class T, class ...Tys>
        struct tparams<T<Tys...>> {
            using type = info<Tys...>;
        };

        template<>
        struct tparams<> {
            template<class ...Tys>
            using type = tparams<Tys...>::type;
        };

        /**
         * Get the template parameters from a templated type. 
         * If the type does not have template parameters, the result is the type itself.
         * @tparam Ty templated type
         */
        template<class ...Ty> requires (sizeof...(Ty) <= 1)
        using tparams_t = typename tparams<Ty...>::type;

        // =======================================================

        template<class ...Ty> 
            requires (sizeof...(Ty) <= 1)
        struct uninstantiate;
        
        template<class Ty> 
        struct uninstantiate<Ty> { 
            using type = Ty; 
            using _type = type;
        };

        template<template<class...> class T, class ...Tys>
        struct uninstantiate<T<Tys...>> {
            using type = templated_t<T>;
            using _type = type;
        };
        
        template<>
        struct uninstantiate<> {
            template<class Ty>
            using type = uninstantiate<Ty>::type;
            using _type = templated_t<type>;
        };

        /**
         * Remove template parameters from templated type.
         * @tparam Ty templated type
         */
        template<class ...Ty> requires (sizeof...(Ty) <= 1)
        using uninstantiate_t = typename uninstantiate<Ty...>::_type;

        // =======================================================

        namespace detail {
            template<template<class...> class T, class ...Tys>
            struct instantiate_impl {
                using type = T<Tys...>;
                using _type = type;
            };
        }

        template<template<class...> class T, class ...Tys>
        struct instantiate : detail::apply_pack_t<detail::instantiate_impl, T, Tys...> {};

        template<template<class...> class T>
        struct instantiate<T> {
            template<class ...Tys>
            using type = instantiate<T, Tys...>::type;
            using _type = templated_t<type>;
        };

        template<template<class...> class T, class ...Tys>
        using instantiate_t = instantiate<T, Tys...>::_type;
        
        // =======================================================

        namespace detail {
            template<class, class ...>
            struct reinstantiate_impl;

            template<template<class...> class T, class ...Args, class ...Tys>
            struct reinstantiate_impl<T<Args...>, Tys...> {
                using type = T<Tys...>;
                using _type = type;
            };
        }

        template<class T, class ...Tys>
        struct reinstantiate : detail::apply_pack<detail::reinstantiate_impl, T, Tys...> {};

        template<class T>
        struct reinstantiate<T> {
            template<class ...Tys>
            using type = reinstantiate<T, Tys...>::type;
            using _type = templated_t<type>;
        };

        template<class T, class ...Tys>
        using reinstantiate_t = reinstantiate<T, Tys...>::_type;
        
        // =======================================================

        template<template<class...> class, class ...T> requires (sizeof...(T) <= 1)
        struct copy_tparams;

        template<template<class...> class Ty, class T>
        struct copy_tparams<Ty, T> {
            using type = Ty<T>;
            using _type = type;
        };

        template<template<class...> class T, class ...Args, template<class...> class Ty>
        struct copy_tparams<Ty, T<Args...>> {
            using type = Ty<Args...>;
            using _type = type;
        };

        template<template<class...> class Ty>
        struct copy_tparams<Ty> {
            template<class T>
            using type = copy_tparams<Ty, T>::type;
            using _type = templated_t<type>;
        };

        /**
         * Copy the template parameters from T to Ty. If T is
         * not templated, it will itself be used as the template parameter.
         * @tparam Ty templated type
         * @tparam T type or type with template parameters
         */
        template<template<class...> class Ty, class ...T> requires (sizeof...(T) <= 1)
        using copy_tparams_t = typename copy_tparams<Ty, T...>::type;

        // =======================================================

        namespace detail {
            template<template<class...> class T, class ...Tys>
            struct transform_impl {
                using type = T<Tys...>;
                using _type = type;
            };
        }

        template<template<class...> class T, class... Tys> 
        struct transform : detail::apply_pack_t<detail::transform_impl, T, Tys...> {};

        template<template<class...> class T>
        struct transform<T> {
            template<class ...Tys>
            using type = transform<T, Tys...>::type;
            using _type = templated_t<type>;
        };

        /**
         * Transform using T.
         * @tparam T transform
         */
        template<template<class...> class T, class ...Tys>
        using transform_t = typename transform<T, Tys...>::_type;

        // =======================================================

        namespace detail {
            template<auto, template<class...> class, class, class ...>
            struct conditional_transform_impl_base;
            template<auto Filter, template<class...> class T, std::size_t ...Is, class ...Tys>
                requires (matches_filter<Filter, Tys, Is> && ...)
            struct conditional_transform_impl_base<Filter, T, std::index_sequence<Is...>, Tys...> {
                using type = T<Tys...>;
                using _type = type;
            };
            
            template<auto Filter, template<class...> class T, std::size_t ...Is, class ...Tys>
                requires (!(matches_filter<Filter, Tys, Is> && ...))
            struct conditional_transform_impl_base<Filter, T, std::index_sequence<Is...>, Tys...> {
                using type = info<Tys...>;
                using _type = type;
            };

            template<auto Filter, template<class...> class T, class ...Tys>
            struct conditional_transform_impl : conditional_transform_impl_base<Filter, T, std::index_sequence_for<Tys...>, Tys...>{};
        }

        template<auto Filter, template<class...> class T, class... Tys>
        struct conditional_transform : detail::apply_pack_tv<detail::conditional_transform_impl, Filter, T, Tys...> {};

        template<auto Filter, template<class...> class T>
        struct conditional_transform<Filter, T> {
            template<class ...Tys>
            using type = conditional_transform<Filter, T, Tys...>::type;
            using _type = templated_t<type>;
        };

        /**
         * Transform using T whem Filter matches.
         * @tparam T transform
         */
        template<auto Filter, template<class...> class T, class ...Tys>
        using conditional_transform_t = typename conditional_transform<Filter, T, Tys...>::_type;

        // =======================================================

        namespace detail {
            template<std::size_t I, class ...Args>
            struct element_impl;

            template<class T, class ...Args>
            struct element_impl<0, T, Args...> {
                using type = T;
                using _type = T;
            };

            template<std::size_t I, class A, class ...Args>
            struct element_impl<I, A, Args...> : element_impl<I - 1, Args...> {};
        }

        template<std::size_t I, class ...Args>
        struct element : detail::apply_pack_v<detail::element_impl, I, Args...> {};

        template<std::size_t I>
        struct element<I> {
            template<class ...Args>
            using type = element<I, Args...>::type;
            using _type = templated_t<type>;
        };

        /**
         * Get I'th element.
         * @tparam I index
         */
        template<std::size_t I, class ...Args>
        using element_t = element<I, Args...>::_type;

        // =======================================================

        namespace detail {
            template<class Ty, class ...Args>
            struct contains_impl : std::bool_constant<(std::is_same_v<Ty, Args> || ...)> {};
        }

        template<class Ty, class ...Args>
        struct contains : detail::apply_pack<detail::contains_impl, Ty, Args...> {};
        
        template<class Ty>
        struct contains<Ty> {
            template<class ...Args>
            using type = contains<Ty, Args...>;
            constexpr static type_filter<type> value{};
        };

        /**
         * Contains Ty.
         * @tparam Ty type
         */
        template<class Ty, class ...Args>
        constexpr auto contains_v = contains<Ty, Args...>::value;

        // =======================================================

        namespace detail {
            template<class, class ...Args> struct contains_all_impl;

            template<class ...Tys, class ...Args>
            struct contains_all_impl<info<Tys...>, Args...>
                : std::bool_constant<(contains_v<Tys, Args...> && ...)> {};
        }

        template<class Ty, class ...Args>
        struct contains_all : detail::apply_pack<detail::contains_all_impl, Ty, Args...> {};

        template<class ...Tys>
        struct contains_all<info<Tys...>> {
            template<class ...Args>
            using type = contains_all<info<Tys...>, Args...>;
            constexpr static type_filter<type> value{};
        };
        
        template<class Ty, class ...Tys>
            requires (!concepts::specialization<Ty, info>)
        struct contains_all<Ty, Tys...> : contains_all<info<Ty, Tys...>> {};

        /**
         * Contains all in Ty, requires Ty is specialization of info.
         * @tparam Ty types
         */
        template<class Ty, class ...Args>
        constexpr auto contains_all_v = contains_all<Ty, Args...>::value;

        // =======================================================

        namespace detail {
            template<class, class ...Args> struct contains_any_impl;

            template<class ...Tys, class ...Args>
            struct contains_any_impl<info<Tys...>, Args...> 
                : std::bool_constant<(contains_v<Tys, Args...> || ...)> {};
        }

        template<class A, class ...Args>
        struct contains_any : detail::apply_pack<detail::contains_any_impl, A, Args...>{};

        template<class ...Tys>
        struct contains_any<info<Tys...>> {
            template<class ...Args>
            using type = contains_any<info<Tys...>, Args...>;
            constexpr static type_filter<type> value{};
        };
        
        template<class Ty, class ...Tys>
            requires (!concepts::specialization<Ty, info>)
        struct contains_any<Ty, Tys...> : contains_any<info<Ty, Tys...>> {};

        /**
         * Contains any of Ty, requires Ty is specialization of info.
         * @tparam Ty types
         */
        template<class Ty, class ...Args>
        constexpr auto contains_any_v = contains_any<Ty, Args...>::value;

        // =======================================================

        namespace detail {
            template<class Ty, class ...Args>
            struct count_impl : std::integral_constant<std::size_t, (std::is_same_v<Ty, Args> + ...)> {};
        }

        template<class Ty, class ...Args>
        struct count : detail::apply_pack<detail::count_impl, Ty, Args...> {};

        template<class Ty>
        struct count<Ty> {
            template<class ...Args>
            using type = count<Ty, Args...>;
            constexpr static type_filter<type> value{};
        };

        /**
         * Count number of occurences of Ty.
         * @tparam Ty type
         */
        template<class Ty, class ...Args>
        constexpr auto count_v = count<Ty, Args...>::value;

        // =======================================================
        
        namespace detail {
            template<class, class ...Args> struct count_all_impl;

            template<class ...Tys, class ...Args>
            struct count_all_impl<info<Tys...>, Args...>
                : std::integral_constant<std::size_t, (count_v<Tys, Args...> + ...)> {};
        }

        template<class A, class ...Args>
        struct count_all : detail::apply_pack<detail::count_all_impl, A, Args...>{};

        template<class ...Tys>
        struct count_all<info<Tys...>> {
            template<class ...Args>
            using type = count_all<info<Tys...>, Args...>;
            constexpr static type_filter<type> value{};
        };
        
        template<class Ty, class ...Tys>
            requires (!concepts::specialization<Ty, info>)
        struct count_all<Ty, Tys...> : count_all<info<Ty, Tys...>> {};

        /**
         * Count number of occurences of all in Ty, requires Ty is specialization of info.
         * @tparam Ty types
         */
        template<class Ty, class ...Args>
        constexpr auto count_all_v = count_all<Ty, Args...>::value;

        // =======================================================

        namespace detail {
            template<auto Filter, class ...Args>
            struct count_filter_impl : std::integral_constant<std::size_t, (matches_filter<Filter, Args> +...)> {};
        }

        template<auto Filter, class ...Args>
        struct count_filter : detail::apply_pack_v<detail::count_filter_impl, Filter, Args...> {};

        template<auto Filter>
        struct count_filter<Filter> {
            template<class ...Args>
            using type = count_filter<Filter, Args...>;
            constexpr static type_filter<type> value{};
        };

        /**
         * Count number of matches of Filter.
         * @tparam Filter filter object
         */
        template<auto Filter, class ...Args>
        constexpr auto count_filter_v = count_filter<Filter, Args...>::value;
        
        // =======================================================

        namespace detail {
            template<class T, class ...Tys>
            constexpr double _unq_cnt = 1.0 / (std::is_same_v<T, Tys> + ...);

            template<class... Tys>
            struct count_unique_impl : std::integral_constant<std::size_t, static_cast<std::size_t>((_unq_cnt<Tys, Tys...> + ... + 0) + 0.5)> {};
        }
        
        template<class ...Tys>
        struct count_unique : detail::apply_pack_d<detail::count_unique_impl, Tys...> {};

        template<>
        struct count_unique<> {
            template<class ...Tys>
            using type = count_unique<Tys...>;
            constexpr static type_filter<type> value{};
        };

        /**
         * Number of unique types.
         */
        template<class ...Tys>
        constexpr auto count_unique_v = count_unique<Tys...>::value;

        // =======================================================

        namespace detail {
            template<class Ty, class ...Args>
            struct index_impl_base : std::integral_constant<std::size_t, 0> {};
            template<class Ty, class ...Args>
            struct index_impl_base<Ty, Ty, Args...> : std::integral_constant<std::size_t, sizeof...(Args) + 1> {};
            template<class Ty, class A, class ...Args>
            struct index_impl_base<Ty, A, Args...> : index_impl_base<Ty, Args...> {};

            template<class Ty, class ...Args>
            struct index_impl : std::integral_constant<std::size_t, (sizeof...(Args) - index_impl_base<Ty, Args...>::value)> {};
        }

        template<class Ty, class ...Args>
        struct index : detail::apply_pack<detail::index_impl, Ty, Args...> {};

        template<class Ty>
        struct index<Ty> {
            template<class ...Args>
            using type = index<Ty, Args...>;
            constexpr static type_filter<type> value{};
        };

        /**
         * Index of Ty.
         * @tparam Ty type
         */
        template<class Ty, class ...Args>
        constexpr auto index_v = index<Ty, Args...>::value;
        
        // =======================================================

        namespace detail {
            struct _idx_filter_match {};
            struct _idx_filter_not {};

            template<class M, auto Filter, class ...Args>
            struct index_filter_impl_base : std::integral_constant<std::size_t, 0> {};
            template<auto Filter, class A, class ...Args> requires matches_filter<Filter, A>
            struct index_filter_impl_base<_idx_filter_match, Filter, A, Args...> : std::integral_constant<std::size_t, sizeof...(Args) + 1> {};
            template<auto Filter, class A, class ...Args> requires (!matches_filter<Filter, A>)
            struct index_filter_impl_base<_idx_filter_not, Filter, A, Args...> : std::integral_constant<std::size_t, sizeof...(Args) + 1> {};
            template<class M, auto Filter, class A, class ...Args>
            struct index_filter_impl_base<M, Filter, A, Args...> : index_filter_impl_base<M, Filter, Args...> {};

            template<auto Filter, class ...Args>
            struct index_filter_impl : std::integral_constant<std::size_t, (sizeof...(Args) - index_filter_impl_base<_idx_filter_match, Filter, Args...>::value)> {};
        }

        template<auto Filter, class ...Args>
        struct index_filter : detail::apply_pack_v<detail::index_filter_impl, Filter, Args...> {};

        template<auto Filter>
        struct index_filter<Filter> {
            template<class ...Args>
            using type = index_filter<Filter, Args...>;
            constexpr static type_filter<type> value{};
        };

        /**
         * Index of first Filter match.
         * @tparam Filter filter
         */
        template<auto Filter, class ...Args>
        constexpr auto index_filter_v = index_filter<Filter, Args...>::value;

        // =======================================================

        namespace detail {
            template<auto Filter, class ...Args>
            struct index_not_filter_impl : std::integral_constant<std::size_t, (sizeof...(Args) - index_filter_impl_base<_idx_filter_not, Filter, Args...>::value)> {};
        }

        template<auto Filter, class ...Args>
        struct index_not_filter : detail::apply_pack_v<detail::index_not_filter_impl, Filter, Args...> {};

        template<auto Filter>
        struct index_not_filter<Filter> {
            template<class ...Args>
            using type = index_not_filter<Filter, Args...>;
            constexpr static type_filter<type> value{};
        };

        /**
         * Index of first not Filter match.
         * @tparam Filter filter
         */
        template<auto Filter, class ...Args>
        constexpr auto index_not_filter_v = index_not_filter<Filter, Args...>::value;
        
        // =======================================================

        namespace detail {
            template<class Ty, class...Args>
            struct indices_impl : indices_impl<info<Ty>> {};

            template<class ...Tys, class...Args>
            struct indices_impl<info<Tys...>, Args...> {
                constexpr static std::array<std::size_t, count_all_v<info<Tys...>, Args...>> value = []{
                    std::array<std::size_t, count_all_v<info<Tys...>, Args...>> _result{};
                    std::size_t _index = 0, _match = 0;
                    ((contains_v<Args, Tys...> ? _result[_match++] = _index++ : ++_index), ...);
                    return _result;
                }();
            };
        }

        template<class Ty, class ...Args>
        struct indices : detail::apply_pack<detail::indices_impl, Ty, Args...> {};

        template<class Ty>
        struct indices<Ty> {
            template<class ...Args>
            using type = indices<Ty, Args...>;
            constexpr static type_filter<type> value{};
        };

        template<class Ty, class ...Tys>
            requires (!concepts::specialization<Ty, info>)
        struct indices<Ty, Tys...> : indices<info<Ty, Tys...>> {};

        /**
         * Indices of Ty, requires Ty is specialization of info.
         * @tparam Ty types
         */
        template<class Ty, class ...Args>
        constexpr auto indices_v = indices<Ty, Args...>::value;
        
        // =======================================================

        namespace detail {
            template<class Ty, class...Args>
            struct indices_except_impl : indices_impl<info<Ty>> {};

            template<class ...Tys, class...Args>
            struct indices_except_impl<info<Tys...>, Args...> {
                constexpr static std::array<std::size_t, sizeof...(Args) - count_all_v<info<Tys...>, Args...>> value = []{
                    std::array<std::size_t, sizeof...(Args) - count_all_v<info<Tys...>, Args...>> _result{};
                    std::size_t _index = 0, _match = 0;
                    ((contains_v<Args, Tys...> ? ++_index : _result[_match++] = _index++), ...);
                    return _result;
                }();
            };
        }

        template<class Ty, class ...Args>
        struct indices_except : detail::apply_pack<detail::indices_except_impl, Ty, Args...> {};

        template<class Ty>
        struct indices_except<Ty> {
            template<class ...Args>
            using type = indices_except<Ty, Args...>;
            constexpr static type_filter<type> value{};
        };

        template<class Ty, class ...Tys>
            requires (!concepts::specialization<Ty, info>)
        struct indices_except<Ty, Tys...> : indices_except<info<Ty, Tys...>> {};

        /**
         * Indices except Ty, requires Ty is specialization of info.
         * @tparam Ty types
         */
        template<class Ty, class ...Args>
        constexpr auto indices_except_v = indices_except<Ty, Args...>::value;
        
        // =======================================================

        namespace detail {
            template<auto Filter, class...Args>
            struct indices_filter_impl {
                constexpr static std::array<std::size_t, count_filter_v<Filter, Args...>> value = []{
                    std::array<std::size_t, count_filter_v<Filter, Args...>> _result{};
                    std::size_t _index = 0, _match = 0;
                    ((matches_filter<Filter, Args> ? _result[_match++] = _index++ : ++_index), ...);
                    return _result;
                }();
            };
        }

        template<auto Filter, class ...Args>
        struct indices_filter : detail::apply_pack_v<detail::indices_filter_impl, Filter, Args...> {};

        template<auto Filter>
        struct indices_filter<Filter> {
            template<class ...Args>
            using type = indices_filter<Filter, Args...>;
            constexpr static type_filter<type> value{};
        };

        /**
         * Indices of all Filter matches.
         * @tparam Filter filter
         */
        template<auto Filter, class ...Args>
        constexpr auto indices_filter_v = indices_filter<Filter, Args...>::value;

        // =======================================================

        namespace detail {
            template<auto Filter, class...Args>
            struct indices_except_filter_impl {
                constexpr static std::array<std::size_t, sizeof...(Args) - count_filter_v<Filter, Args...>> value = [] {
                    std::array<std::size_t, sizeof...(Args) - count_filter_v<Filter, Args...>> _result{};
                    std::size_t _index = 0, _match = 0;
                    ((matches_filter<Filter, Args> ? ++_index : _result[_match++] = _index++), ...);
                    return _result;
                }();
            };
        }

        template<auto Filter, class ...Args>
        struct indices_except_filter : detail::apply_pack_v<detail::indices_except_filter_impl, Filter, Args...> {};

        template<auto Filter>
        struct indices_except_filter<Filter> {
            template<class ...Args>
            using type = indices_except_filter<Filter, Args...>;
            constexpr static type_filter<type> value{};
        };

        /**
         * Indices of all Filter matches.
         * @tparam Filter filter
         */
        template<auto Filter, class ...Args>
        constexpr auto indices_except_filter_v = indices_except_filter<Filter, Args...>::value;
        
        // =======================================================

        namespace detail {
            template<class Ty, class ...Args>
            struct first_index_impl_base : std::integral_constant<std::size_t, 0> {};
            template<class ...Tys, class A, class ...Args> requires (std::is_same_v<Tys, A> || ...)
            struct first_index_impl_base<info<Tys...>, A, Args...> : std::integral_constant<std::size_t, sizeof...(Args) + 1> {};
            template<class ...Tys, class A, class ...Args>
            struct first_index_impl_base<info<Tys...>, A, Args...> : first_index_impl_base<info<Tys...>, Args...> {};

            template<class Ty, class ...Args>
            struct first_index_impl;
            template<class ...Tys, class ...Args>
            struct first_index_impl<info<Tys...>, Args...> : std::integral_constant<std::size_t, (sizeof...(Args) - first_index_impl_base<info<Tys...>, Args...>::value)> {};
        }

        template<class Ty, class ...Args>
        struct first_index : detail::apply_pack<detail::first_index_impl, Ty, Args...> {};

        template<class Ty>
        struct first_index<Ty> {
            template<class ...Args>
            using type = first_index<Ty, Args...>;
            constexpr static type_filter<type> value{};
        };
        
        template<class Ty, class ...Tys>
            requires (!concepts::specialization<Ty, info>)
        struct first_index<Ty, Tys...> : first_index<info<Ty, Tys...>> {};

        /**
         * First index of Ty, requires Ty is specialization of info.
         * @tparam Ty type
         */
        template<class Ty, class ...Args>
        constexpr auto first_index_v = first_index<Ty, Args...>::value;

        // =======================================================

        namespace detail {
            template<class, class... Args>
            struct first_indices_impl;

            template<class ...Tys, class... Args>
            struct first_indices_impl<info<Tys...>, Args...> {
                constexpr static std::array<std::size_t, sizeof...(Tys)> value{ first_index_v<info<Tys>, Args...>... };
            };
        }

        template<class Ty, class ...Tys>
        struct first_indices : detail::apply_pack<detail::first_indices_impl, Ty, Tys...> {};

        template<class Ty>
        struct first_indices<Ty> {
            template<class ...Tys>
            using type = first_indices<Ty, Tys...>;
            constexpr static type_filter<type> value{};
        };
        
        template<class Ty, class ...Tys>
            requires (!concepts::specialization<Ty, info>)
        struct first_indices<Ty, Tys...> : first_indices<info<Ty, Tys...>> {};

        /**
         * First indices.
         */
        template<class Ty, class ...Tys>
        constexpr auto first_indices_v = first_indices<Ty, Tys...>::value;

        // =======================================================
        
        namespace detail {
            template<class ...Tys>
            constexpr static std::array<std::size_t, count_unique_v<info<Tys...>>> _unq_fst_i = [] {
                std::array<std::size_t, count_unique_v<info<Tys...>>> _result{};
                std::size_t _index = 0, _match = 0;
                (((index_v<Tys, Tys...> == _index) ? _result[_match++] = _index++ : ++_index), ...);
                return _result;
            }();

            template<class... Args>
            struct unique_indices_impl {
                constexpr static std::array<std::size_t, count_unique_v<info<Args...>>> value = _unq_fst_i<Args...>;;
            };
        }

        template<class ...Tys>
        struct unique_indices : detail::apply_pack_d<detail::unique_indices_impl, Tys...> {};

        template<>
        struct unique_indices<> {
            template<class ...Tys>
            using type = unique_indices<Tys...>;
            constexpr static type_filter<type> value{};
        };

        /**
         * Unique indices.
         */
        template<class ...Tys>
        constexpr auto unique_indices_v = unique_indices<Tys...>::value;

        // =======================================================

        namespace detail {
            template<class, class...> 
            struct reverse_impl_base;
            template<std::size_t ...Is, class ...Tys>
            struct reverse_impl_base<std::index_sequence<Is...>, Tys...> {
                using type = info<element_t<sizeof...(Tys) - Is - 1, Tys...>...>;
                using _type = type;
            };

            template<class ...Tys>
            struct reverse_impl : reverse_impl_base<std::index_sequence_for<Tys...>, Tys...> {};
        }

        template<class ...Tys>
        struct reverse : detail::apply_pack_d<detail::reverse_impl, Tys...> {};

        template<>
        struct reverse<> {
            template<class ...Tys>
            using type = reverse<Tys...>::type;
            using _type = templated_t<type>;
        };

        /**
         * Reverse pack.
         */
        template<class ...Tys>
        using reverse_t = reverse<Tys...>::_type;

        // =======================================================

        namespace detail {
            template<class, class ...>
            struct unique_impl_base;
            template<std::size_t ...Is, class ...Tys>
            struct unique_impl_base<std::index_sequence<Is...>, Tys...> {
                using type = info<element_t<_unq_fst_i<Tys...>[Is], Tys...>...>;
            };

            template<class... Tys> 
            struct unique_impl {
                using type = unique_impl_base<std::make_index_sequence<count_unique_v<info<Tys...>>>, Tys...>::type;
                using _type = type;
            };
        }

        template<class ...Tys>
        struct unique : detail::apply_pack_d<detail::unique_impl, Tys...> {};

        template<>
        struct unique<> {
            template<class Ty>
            using type = unique<Ty>::type;
            using _type = templated_t<type>;
        };

        /**
         * Remove duplicate types.
         */
        template<class ...Tys>
        using unique_t = typename unique<Tys...>::_type;

        // =======================================================

        namespace detail {
            template<class ...Tys>
            struct join_impl_base {};
            
            template<class ...As, class ...Bs, class ...Tys>
            struct join_impl_base<info<As...>, info<Bs...>, Tys...>
                : join_impl_base<info<As..., Bs...>, Tys...> {};
            
            template<class ...As>
            struct join_impl_base<info<As...>> {
                using type = info<As...>;
                using _type = type;
            };
            
            template<>
            struct join_impl_base<> {
                using type = info<>;
                using _type = type;
            };

            template<class ...Tys>
            struct join_impl : join_impl_base<convert_to_info<Tys>...> {};
        }

        template<class ...Tys>
        struct join : detail::apply_pack_d<detail::join_impl, Tys...> {};

        template<>
        struct join<> {
            template<class ...Tys>
            using type = join<Tys...>::type;
            using _type = templated_t<type>;
        };

        /**
         * Flattens a pack of packs
         */
        template<class ...Tys>
        using join_t = join<Tys...>::_type;

        // =======================================================

        namespace detail {
            struct _split_at {};
            struct _split_after {};
            struct _split_before {};

            template<class, class, class, class, class ...Tys>
            struct split_impl_base;

            template<class M, class ...Res, class ...Cur, class ...Args, class T, class ...Tys>
                requires (!contains_v<T, Args...>)
            struct split_impl_base<M, info<Res...>, info<Cur...>, info<Args...>, T, Tys...> 
                : split_impl_base<M, info<Res...>, info<Cur..., T>, info<Args...>, Tys...> {};

            template<class ...Res, class ...Cur, class ...Args, class T, class ...Tys>
                requires contains_v<T, Args...>
            struct split_impl_base<_split_at, info<Res...>, info<Cur...>, info<Args...>, T, Tys...> 
                : split_impl_base<_split_at, info<Res..., info<Cur...>>, info<>, info<Args...>, Tys...> {};
            
            template<class ...Res, class ...Cur, class ...Args, class T, class ...Tys>
                requires contains_v<T, Args...>
            struct split_impl_base<_split_after, info<Res...>, info<Cur...>, info<Args...>, T, Tys...> 
                : split_impl_base<_split_after, info<Res..., info<Cur..., T>>, info<>, info<Args...>, Tys...> {};
            
            template<class ...Res, class ...Cur, class ...Args, class T, class ...Tys>
                requires contains_v<T, Args...>
            struct split_impl_base<_split_before, info<Res...>, info<Cur...>, info<Args...>, T, Tys...> 
                : split_impl_base<_split_before, info<Res..., info<Cur...>>, info<T>, info<Args...>, Tys...> {};
            
            template<class M, class ...Res, class ...Cur, class ...Args>
            struct split_impl_base<M, info<Res...>, info<Cur...>, info<Args...>> {
                using type = info<Res..., info<Cur...>>;
                using _type = type;
            };

            template<class, class...>
            struct split_impl;

            template<class ...Args, class ...Tys>
            struct split_impl<info<Args...>, Tys...> : split_impl_base<_split_at, info<>, info<>, info<Args...>, Tys...> {};
        }

        template<class Ty, class ...Args>
        struct split : detail::apply_pack<detail::split_impl, Ty, Args...> {};

        template<class Ty>
        struct split<Ty> {
            template<class ...Args>
            using type = split<Ty, Args...>::type;
            using _type = templated_t<type>;
        };

        template<class Ty, class ...Tys>
            requires (!concepts::specialization<Ty, info>)
        struct split<Ty, Tys...> : split<info<Ty, Tys...>> {};

        /**
         * Split at Ty, can be specialized info.
         * @tparam Ty types
         */
        template<class Ty, class ...Args>
        using split_t = split<Ty, Args...>::_type;
        
        // =======================================================

        namespace detail {
            template<class, class...>
            struct split_after_impl;

            template<class ...Args, class ...Tys>
            struct split_after_impl<info<Args...>, Tys...> : split_impl_base<_split_after, info<>, info<>, info<Args...>, Tys...> {};
        }

        template<class Ty, class ...Args>
        struct split_after : detail::apply_pack<detail::split_after_impl, Ty, Args...> {};

        template<class Ty>
        struct split_after<Ty> {
            template<class ...Args>
            using type = split_after<Ty, Args...>::type;
            using _type = templated_t<type>;
        };

        template<class Ty, class ...Tys>
            requires (!concepts::specialization<Ty, info>)
        struct split_after<Ty, Tys...> : split_after<info<Ty, Tys...>> {};

        /**
         * Split after Ty, can be specialized info.
         * @tparam Ty types
         */
        template<class Ty, class ...Args>
        using split_after_t = split_after<Ty, Args...>::_type;
        
        // =======================================================

        namespace detail {
            template<class, class...>
            struct split_before_impl;

            template<class ...Args, class ...Tys>
            struct split_before_impl<info<Args...>, Tys...> : split_impl_base<_split_before, info<>, info<>, info<Args...>, Tys...> {};
        }

        template<class Ty, class ...Args>
        struct split_before : detail::apply_pack<detail::split_before_impl, Ty, Args...> {};

        template<class Ty>
        struct split_before<Ty> {
            template<class ...Args>
            using type = split_before<Ty, Args...>::type;
            using _type = templated_t<type>;
        };

        template<class Ty, class ...Tys>
            requires (!concepts::specialization<Ty, info>)
        struct split_before<Ty, Tys...> : split_before<info<Ty, Tys...>> {};

        /**
         * Split after Ty, can be specialized info.
         * @tparam Ty types
         */
        template<class Ty, class ...Args>
        using split_before_t = split_before<Ty, Args...>::_type;

        // =======================================================

        namespace detail {
            template<class, class, class, auto, class ...Tys>
            struct split_filter_impl_base;

            template<class M, class ...Res, class ...Cur, auto Filter, class T, class ...Tys>
                requires (!matches_filter<Filter, T>)
            struct split_filter_impl_base<M, info<Res...>, info<Cur...>, Filter, T, Tys...>
                : split_filter_impl_base<M, info<Res...>, info<Cur..., T>, Filter, Tys...> {};

            template<class ...Res, class ...Cur, auto Filter, class T, class ...Tys>
                requires matches_filter<Filter, T>
            struct split_filter_impl_base<_split_at, info<Res...>, info<Cur...>, Filter, T, Tys...>
                : split_filter_impl_base<_split_at, info<Res..., info<Cur...>>, info<>, Filter, Tys...> {};

            template<class ...Res, class ...Cur, auto Filter, class T, class ...Tys>
                requires matches_filter<Filter, T>
            struct split_filter_impl_base<_split_after, info<Res...>, info<Cur...>, Filter, T, Tys...>
                : split_filter_impl_base<_split_after, info<Res..., info<Cur..., T>>, info<>, Filter, Tys...> {};

            template<class ...Res, class ...Cur, auto Filter, class T, class ...Tys>
                requires matches_filter<Filter, T>
            struct split_filter_impl_base<_split_before, info<Res...>, info<Cur...>, Filter, T, Tys...>
                : split_filter_impl_base<_split_before, info<Res..., info<Cur...>>, info<T>, Filter, Tys...> {};

            template<class M, class ...Res, class ...Cur, auto Filter>
            struct split_filter_impl_base<M, info<Res...>, info<Cur...>, Filter> {
                using type = info<Res..., info<Cur...>>;
                using _type = type;
            };

            template<auto Filter, class ...Tys>
            struct split_filter_impl : split_filter_impl_base<_split_at, info<>, info<>, Filter, Tys...> {};
        }

        template<auto Filter, class ...Args>
        struct split_filter : detail::apply_pack_v<detail::split_filter_impl, Filter, Args...> {};

        template<auto Filter>
        struct split_filter<Filter> {
            template<class ...Args>
            using type = split_filter<Filter, Args...>::type;
            using _type = templated_t<type>;
        };

        /**
         * Split at Filter matches.
         * @tparam Filter filter
         */
        template<auto Filter, class ...Args>
        using split_filter_t = split_filter<Filter, Args...>::_type;

        // =======================================================

        namespace detail {
            template<auto Filter, class ...Tys>
            struct split_after_filter_impl : split_filter_impl_base<_split_after, info<>, info<>, Filter, Tys...> {};
        }

        template<auto Filter, class ...Args>
        struct split_after_filter : detail::apply_pack_v<detail::split_after_filter_impl, Filter, Args...> {};

        template<auto Filter>
        struct split_after_filter<Filter> {
            template<class ...Args>
            using type = split_after_filter<Filter, Args...>::type;
            using _type = templated_t<type>;
        };

        /**
         * Split after Filter matches.
         * @tparam Filter filter
         */
        template<auto Filter, class ...Args>
        using split_after_filter_t = split_after_filter<Filter, Args...>::_type;
        
        // =======================================================

        namespace detail {
            template<auto Filter, class ...Tys>
            struct split_before_filter_impl : split_filter_impl_base<_split_before, info<>, info<>, Filter, Tys...> {};
        }

        template<auto Filter, class ...Args>
        struct split_before_filter : detail::apply_pack_v<detail::split_before_filter_impl, Filter, Args...> {};

        template<auto Filter>
        struct split_before_filter<Filter> {
            template<class ...Args>
            using type = split_before_filter<Filter, Args...>::type;
            using _type = templated_t<type>;
        };

        /**
         * Split before Filter matches.
         * @tparam Filter filter
         */
        template<auto Filter, class ...Args>
        using split_before_filter_t = split_before_filter<Filter, Args...>::_type;

        // =======================================================

        namespace detail {
            template<std::size_t, class, class ...Tys>
            struct sub_impl_base;

            template<std::size_t A, std::size_t ...Is, class ...Tys>
            struct sub_impl_base<A, std::index_sequence<Is...>, Tys...> {
                using type = info<element_t<A + Is, Tys...>...>;
                using _type = type;
            };

            template<std::size_t A, std::size_t B, class ...Tys>
            struct sub_impl : sub_impl_base<A, std::make_index_sequence<B - A>, Tys...> {};
        }

        template<std::size_t A, std::size_t B, class ...Args> requires (A <= B)
        struct sub : detail::apply_pack_v2<detail::sub_impl, A, B, Args...> {};

        template<std::size_t A, std::size_t B> requires (A <= B)
        struct sub<A, B> {
            template<class ...Args>
            using type = sub<A, B, Args...>::type;
            using _type = templated_t<type>;
        };

        /**
         * Take from A to B.
         * @tparam A first index
         * @tparam B last index (non-inclusive)
         */
        template<std::size_t A, std::size_t B, class ...Args> requires (A <= B)
        using sub_t = sub<A, B, Args...>::_type;
        
        // =======================================================

        namespace detail {
            template<std::size_t I, class ...Tys>
            struct take_impl : sub_impl<0, I, Tys...> {};
        }

        template<std::size_t I, class ...Args>
        struct take : detail::apply_pack_v<detail::take_impl, I, Args...> {};

        template<std::size_t I>
        struct take<I> {
            template<class ...Args>
            using type = take<I, Args...>::type;
            using _type = templated_t<type>;
        };

        /**
         * Take first I.
         * @tparam I take
         */
        template<std::size_t I, class ...Args>
        using take_t = take<I, Args...>::_type;
        
        // =======================================================

        namespace detail {
            template<auto Filter, class ...Tys>
            struct take_while_impl : sub_impl<0, index_not_filter_v<Filter, Tys...>, Tys...> {};
        }

        template<auto Filter, class ...Args>
        struct take_while : detail::apply_pack_v<detail::take_while_impl, Filter, Args...> {};

        template<auto Filter>
        struct take_while<Filter> {
            template<class ...Args>
            using type = take_while<Filter, Args...>::type;
            using _type = templated_t<type>;
        };

        /**
         * Take while Filter matches.
         * @tparam I take
         */
        template<auto Filter, class ...Args>
        using take_while_t = take_while<Filter, Args...>::_type;
        
        // =======================================================

        namespace detail {
            template<std::size_t I, class ...Tys>
            struct take_last_impl : sub_impl<sizeof...(Tys) - I, sizeof...(Tys), Tys...> {};
        }

        template<std::size_t I, class ...Args>
        struct take_last : detail::apply_pack_v<detail::take_last_impl, I, Args...> {};

        template<std::size_t I>
        struct take_last<I> {
            template<class ...Args>
            using type = take_last<I, Args...>::type;
            using _type = templated_t<type>;
        };

        /**
         * Take last I.
         * @tparam I take
         */
        template<std::size_t I, class ...Args>
        using take_last_t = take_last<I, Args...>::_type;
        
        // =======================================================

        namespace detail {
            template<auto Filter, class ...Tys>
            struct take_last_while_impl : reverse<typename sub_impl<0, index_not_filter_v<Filter, Tys...>, Tys...>::type> {};
        }

        template<auto Filter, class ...Args>
        struct take_last_while : detail::apply_pack_v<detail::take_last_while_impl, Filter, reverse_t<Args...>> {};

        template<auto Filter>
        struct take_last_while<Filter> {
            template<class ...Args>
            using type = take_last_while<Filter, Args...>::type;
            using _type = templated_t<type>;
        };

        /**
         * Take last while Filter matches.
         * @tparam I take
         */
        template<auto Filter, class ...Args>
        using take_last_while_t = take_last_while<Filter, Args...>::_type;
        
        // =======================================================

        namespace detail {
            template<std::size_t I, class ...Tys>
            struct drop_impl : sub_impl<I, sizeof...(Tys), Tys...> {};
        }

        template<std::size_t I, class ...Args>
        struct drop : detail::apply_pack_v<detail::drop_impl, I, Args...> {};

        template<std::size_t I>
        struct drop<I> {
            template<class ...Args>
            using type = drop<I, Args...>::type;
            using _type = templated_t<type>;
        };

        /**
         * Drop first I.
         * @tparam I drop
         */
        template<std::size_t I, class ...Args>
        using drop_t = drop<I, Args...>::_type;
        
        // =======================================================

        namespace detail {
            template<auto Filter, class ...Tys>
            struct drop_while_impl : sub_impl<index_not_filter_v<Filter, Tys...>, sizeof...(Tys), Tys...> {};
        }

        template<auto Filter, class ...Args>
        struct drop_while : detail::apply_pack_v<detail::drop_while_impl, Filter, Args...> {};

        template<auto Filter>
        struct drop_while<Filter> {
            template<class ...Args>
            using type = drop_while<Filter, Args...>::type;
            using _type = templated_t<type>;
        };

        /**
         * Drop while Filter matches.
         * @tparam I drop
         */
        template<auto Filter, class ...Args>
        using drop_while_t = drop_while<Filter, Args...>::_type;
        
        // =======================================================

        namespace detail {
            template<std::size_t I, class ...Tys>
            struct drop_last_impl : sub_impl<0, sizeof...(Tys) - I, Tys...> {};
        }

        template<std::size_t I, class ...Args>
        struct drop_last : detail::apply_pack_v<detail::drop_last_impl, I, Args...> {};

        template<std::size_t I>
        struct drop_last<I> {
            template<class ...Args>
            using type = drop_last<I, Args...>::type;
            using _type = templated_t<type>;
        };

        /**
         * Drop last I.
         * @tparam I drop
         */
        template<std::size_t I, class ...Args>
        using drop_last_t = drop_last<I, Args...>::_type;
        
        // =======================================================

        namespace detail {
            template<auto Filter, class ...Tys>
            struct drop_last_while_impl : reverse<typename sub_impl<index_not_filter_v<Filter, Tys...>, sizeof...(Tys), Tys...>::type> {};
        }

        template<auto Filter, class ...Args>
        struct drop_last_while : detail::apply_pack_v<detail::drop_last_while_impl, Filter, reverse_t<Args...>> {};

        template<auto Filter>
        struct drop_last_while<Filter> {
            template<class ...Args>
            using type = drop_last_while<Filter, Args...>::type;
            using _type = templated_t<type>;
        };

        /**
         * Drop last while Filter matches.
         * @tparam I drop
         */
        template<auto Filter, class ...Args>
        using drop_last_while_t = drop_last_while<Filter, Args...>::_type;

        // =======================================================

        namespace detail {
            template<auto Array, class ...Tys>
            struct keep_indices_impl {
                template<std::size_t ...Is> struct _helper {
                    using type = info<element_t<Is, Tys...>...>;
                };
                using type = typename array_to_pack_t<Array, _helper>::type;
                using _type = type;
            };
        }

        template<auto Array, class ...Args>
        struct keep_indices : detail::apply_pack_v<detail::keep_indices_impl, Array, Args...> {};

        template<auto Array>
        struct keep_indices<Array> {
            template<class ...Args>
            using type = keep_indices<Array, Args...>::type;
            using _type = templated_t<type>;
        };

        /**
         * Only keep indices.
         * @tparam Array indices
         */
        template<auto Array, class ...Args>
        using keep_indices_t = keep_indices<Array, Args...>::_type;

        // =======================================================

        namespace detail {
            template<class, class...>
            struct keep_impl;

            template<class ...Tys, class ...Args>
            struct keep_impl<info<Tys...>, Args...> {
                template<std::size_t ...Is> struct _helper {
                    using type = info<element_t<Is, Args...>...>;
                };
                using type = typename array_to_pack_t<indices_v<info<Tys...>, Args...>, _helper>::type;
                using _type = type;
            };
        }

        template<class Ty, class ...Args>
        struct keep : detail::apply_pack<detail::keep_impl, Ty, Args...> {};

        template<class Ty>
        struct keep<Ty> {
            template<class ...Args>
            using type = keep<Ty, Args...>::type;
            using _type = templated_t<type>;
        };

        /**
         * Only keep Ty, requires Ty is specialization of info.
         * @tparam Ty types
         */
        template<class Ty, class ...Args>
        using keep_t = keep<Ty, Args...>::_type;
        
        // =======================================================

        namespace detail {
            template<auto Array, class ...Tys>
            struct remove_indices_impl {
                constexpr static std::array<std::size_t, sizeof...(Tys) - Array.size()> _indices = [] {
                    std::array<std::size_t, sizeof...(Tys) - Array.size()> _indices{};
                    std::size_t _index = 0ull, _match = 0ull;
                    for (std::size_t _index = 0ull; _index < sizeof...(Tys); ++_index) {
                        if (!std::ranges::contains(Array, _index)) _indices[_match++] = _index;
                    }
                    return _indices;
                }();

                template<std::size_t ...Is> struct _helper {
                    using type = info<element_t<Is, Tys...>...>;
                };
                using type = typename array_to_pack_t<_indices, _helper>::type;
                using _type = type;
            };
        }

        template<auto Array, class ...Args>
        struct remove_indices : detail::apply_pack_v<detail::remove_indices_impl, Array, Args...> {};

        template<auto Array>
        struct remove_indices<Array> {
            template<class ...Args>
            using type = remove_indices<Array, Args...>::type;
            using _type = templated_t<type>;
        };

        /**
         * Remove indices.
         * @tparam Array indices
         */
        template<auto Array, class ...Args>
        using remove_indices_t = remove_indices<Array, Args...>::_type;

        // =======================================================

        namespace detail {
            template<class, class...>
            struct remove_impl;

            template<class ...Tys, class ...Args>
            struct remove_impl<info<Tys...>, Args...> {
                template<std::size_t ...Is> struct _helper {
                    using type = info<element_t<Is, Args...>...>;
                };
                using type = typename array_to_pack_t<indices_except_v<info<Tys...>, Args...>, _helper>::type;
                using _type = type;
            };
        }

        template<class Ty, class ...Args>
        struct remove : detail::apply_pack<detail::remove_impl, Ty, Args...> {};

        template<class Ty>
        struct remove<Ty> {
            template<class ...Args>
            using type = remove<Ty, Args...>::type;
            using _type = templated_t<type>;
        };

        /**
         * Remove Ty, requires Ty is specialization of info.
         * @tparam Ty types
         */
        template<class Ty, class ...Args>
        using remove_t = remove<Ty, Args...>::_type;

        // =======================================================

        namespace detail {
            template<class Ty, class...Tys>
            struct append_impl : append_impl<info<Ty>, Tys...> {};

            template<class...Args, class...Tys>
            struct append_impl<info<Args...>, Tys...> {
                using type = info<Tys..., Args...>;
                using _type = type;
            };
        }

        template<class Ty, class ...Tys>
        struct append : detail::apply_pack<detail::append_impl, Ty, Tys...> {};

        template<class Ty>
        struct append<Ty> {
            template<class ...Tys>
            using type = append<Ty, Tys...>::type;
            using _type = templated_t<type>;
        };

        template<class Ty, class ...Tys>
            requires (!concepts::specialization<Ty, info>)
        struct append<Ty, Tys...> : append<info<Ty, Tys...>> {};

        /**
         * Append all in Ty, requires Ty is specialization of info.
         * @tparam Ty types
         */
        template<class Ty, class ...Tys>
        using append_t = append<Ty, Tys...>::type;

        // =======================================================

        namespace detail {
            template<class Ty, class...Tys>
            struct prepend_impl : prepend_impl<info<Ty>, Tys...> {};

            template<class...Args, class...Tys>
            struct prepend_impl<info<Args...>, Tys...> {
                using type = info<Args..., Tys...>;
                using _type = type;
            };
        }

        template<class Ty, class ...Tys>
        struct prepend : detail::apply_pack<detail::prepend_impl, Ty, Tys...> {};

        template<class Ty>
        struct prepend<Ty> {
            template<class ...Tys>
            using type = prepend<Ty, Tys...>::type;
            using _type = templated_t<type>;
        };

        template<class Ty, class ...Tys>
            requires (!concepts::specialization<Ty, info>)
        struct prepend<Ty, Tys...> : prepend<info<Ty, Tys...>> {};

        /**
         * Prepend all in Ty, requires Ty is specialization of info.
         * @tparam Ty types
         */
        template<class Ty, class ...Tys>
        using prepend_t = prepend<Ty, Tys...>::type;

        // =======================================================

        namespace detail {
            template<class, class>
            struct erase_impl_base;

            template<class ...As, class ...Bs>
            struct erase_impl_base<info<As...>, info<Bs...>> {
                using type = info<As..., Bs...>;
            };

            template<std::size_t I, class...Tys>
            struct erase_impl {
                using _a = sub_impl<0, I, Tys...>::type;
                using _b = sub_impl<I + 1, sizeof...(Tys), Tys...>::type;

                using type = erase_impl_base<_a, _b>::type;
                using _type = type;
            };
        }

        template<std::size_t I, class ...Tys>
        struct erase : detail::apply_pack_v<detail::erase_impl, I, Tys...> {};

        template<std::size_t I>
        struct erase<I> {
            template<class ...Tys>
            using type = erase<I, Tys...>::type;
            using _type = templated_t<type>;
        };

        /**
         * Erase index I.
         * @tparam I index
         */
        template<std::size_t I, class ...Tys>
        using erase_t = erase<I, Tys...>::type;

        // =======================================================
        
        namespace detail {
            template<class, class, class>
            struct insert_impl_base;

            template<class ...As, class ...Bs, class ...Cs>
            struct insert_impl_base<info<As...>, info<Bs...>, info<Cs...>> {
                using type = info<As..., Bs..., Cs...>;
            };

            template<std::size_t I, class Ty, class...Tys>
            struct insert_impl : insert_impl<I, info<Ty>, Tys...> {};

            template<std::size_t I, class...Args, class...Tys>
            struct insert_impl<I, info<Args...>, Tys...> {
                using _a = sub_impl<0, I, Tys...>::type;
                using _b = sub_impl<I, sizeof...(Tys), Tys...>::type;

                using type = insert_impl_base<_a, info<Args...>, _b>::type;
                using _type = type;
            };
        }

        template<std::size_t I, class Ty, class ...Tys>
        struct insert : detail::apply_pack_dv<detail::insert_impl, I, Ty, Tys...> {};

        template<std::size_t I, class Ty>
        struct insert<I, Ty> {
            template<class ...Tys>
            using type = insert<I, Ty, Tys...>::type;
            using _type = templated_t<type>;
        };

        template<std::size_t I, class Ty, class ...Tys>
            requires (!concepts::specialization<Ty, info>)
        struct insert<I, Ty, Tys...> : insert<I, info<Ty, Tys...>> {};

        /**
         * Insert all in Ty at I, requires Ty is specialization of info.
         * @tparam I index
         * @tparam Ty types
         */
        template<std::size_t I, class Ty, class ...Tys>
        using insert_t = insert<I, Ty, Tys...>::type;

        // =======================================================

        namespace detail {
            template<std::size_t I, class R, class ...Tys>
            struct swap_impl {
                using type = apply_pack_dv<insert_impl, I, R, typename erase_impl<I, Tys...>::type>::type;
                using _type = type;
            };
        }

        template<std::size_t I, class R, class ...Tys>
        struct swap : detail::apply_pack_dv<detail::swap_impl, I, R, Tys...> {};

        template<std::size_t I, class R>
        struct swap<I, R> {
            template<class ...Tys>
            using type = swap<I, R, Tys...>::type;
            using _type = templated_t<type>;
        };
        
        /**
         * Swap index I with R.
         * @tparam I index
         * @tparam R replacement
         */
        template<std::size_t I, class R, class ...Tys>
        using swap_t = swap<I, R, Tys...>::_type;
        
        // =======================================================

        namespace detail {
            template<class T, class ...Tys>
            constexpr bool _rplc_cnt = (std::is_same_v<T, Tys> || ...);

            template<class, class, class...>
            struct replace_impl;

            template<class R, class ...Args, class ...Tys>
            struct replace_impl<R, info<Args...>, Tys...> {
                using type = info<std::conditional_t<_rplc_cnt<Tys, Args...>, R, Tys>...>;
                using _type = type;
            };
        }

        template<class R, class Ty, class ...Tys>
        struct replace : detail::apply_pack2<detail::replace_impl, R, Ty, Tys...> {};

        template<class R, class Ty>
        struct replace<R, Ty> {
            template<class ...Tys>
            using type = replace<R, Ty, Tys...>::type;
            using _type = templated_t<type>;
        };
        
        template<class R, class Ty, class ...Tys>
            requires (!concepts::specialization<Ty, info>)
        struct replace<R, Ty, Tys...> : replace<R, info<Ty, Tys...>> {};

        /**
         * Replace Ty with R, requires Ty is specialization of info.
         * @tparam R replacement
         * @tparam Ty types
         */
        template<class R, class Ty, class ...Tys>
        using replace_t = replace<R, Ty, Tys...>::_type;

        // =======================================================

        namespace detail {
            // TODO: index support
            template<auto Filter, class R, class ...Tys>
            struct replace_filter_impl {
                using type = info<std::conditional_t<matches_filter<Filter, Tys>, R, Tys>...>;
                using _type = type;
            };
        }

        template<class R, auto Filter, class ...Tys>
        struct replace_filter : detail::apply_pack_dv<detail::replace_filter_impl, Filter, R, Tys...> {};

        template<class R, auto Filter>
        struct replace_filter<R, Filter> {
            template<class ...Tys>
            using type = replace_filter<R, Filter, Tys...>::type;
            using _type = templated_t<type>;
        };
        
        template<class R, auto Filter, class ...Tys>
        using replace_filter_t = replace_filter<R, Filter, Tys...>::_type;

        // =======================================================

        namespace detail {
            template<auto Filter, class ...Tys>
            struct filter_impl {
                template<std::size_t ...Is> struct _helper {
                    using type = info<element_t<Is, Tys...>...>;
                };
                using type = typename array_to_pack_t<indices_filter_v<Filter, Tys...>, _helper>::type;
                using _type = type;
            };
        }

        template<auto Filter, class ...Args>
        struct filter : detail::apply_pack_v<detail::filter_impl, Filter, Args...> {};

        template<auto Filter>
        struct filter<Filter> {
            template<class ...Args>
            using type = filter<Filter, Args...>::type;
            using _type = templated_t<type>;
        };

        /**
         * Keep all Filter matches.
         * @tparam Filter filter
         */
        template<auto Filter, class ...Args>
        using filter_t = filter<Filter, Args...>::_type;

        // =======================================================

        namespace detail {
            template<auto Sorter, class A, class B>
            constexpr bool sorter_result = Sorter.operator() < A, B > ();

            template<auto, class, class> 
            struct merge_sort_merge;

            template<auto Sorter, class A, class ...As, class B, class ...Bs>
                requires sorter_result<Sorter, A, B>
            struct merge_sort_merge<Sorter, info<A, As...>, info<B, Bs...>> {
                using _recurse = typename merge_sort_merge<Sorter, info<As...>, info<B, Bs...>>::type;
                using type = append_impl<_recurse, A>::type;
            };

            template<auto Sorter, class A, class ...As, class B, class ...Bs>
                requires (!sorter_result<Sorter, A, B>)
            struct merge_sort_merge<Sorter, info<A, As...>, info<B, Bs...>> {
                using _recurse = typename merge_sort_merge<Sorter, info<A, As...>, info<Bs...>>::type;
                using type = append_impl<_recurse, B>::type;
            };

            template<auto Sorter, class ...As>
            struct merge_sort_merge<Sorter, info<As...>, info<>> { using type = info<As...>; };

            template<auto Sorter, class ...Bs>
            struct merge_sort_merge<Sorter, info<>, info<Bs...>> { using type = info<Bs...>; };

            template<auto Sorter, class Ty> 
            struct merge_sort;

            template<auto Sorter, class ...Tys>
            struct merge_sort<Sorter, info<Tys...>> {
                constexpr static std::size_t _mid = sizeof...(Tys) / 2;
                using _left = typename merge_sort<Sorter, take_t<_mid, Tys...>>::type;
                using _right = typename merge_sort<Sorter, drop_t<_mid, Tys...>>::type;
                using type = typename merge_sort_merge<Sorter, _left, _right>::type;
            };

            template<auto Sorter, class Ty>
            struct merge_sort<Sorter, info<Ty>> {
                using type = info<Ty>;
                using _type = type;
            };

            template<auto Sorter, class ...Tys>
            using sort_impl = merge_sort<Sorter, info<Tys...>>;
        }

        template<auto Sorter, class ...Tys>
        struct sort : detail::apply_pack_v<detail::sort_impl, Sorter, Tys...>{};

        template<auto Sorter>
        struct sort<Sorter> {
            template<class ...Tys>
            using type = sort<Sorter, Tys...>::type;
            using _type = templated_t<type>;
        };

        /**
         * Sort types.
         * @tparam Sorter sorter
         */
        template<auto Sorter, class ...Tys>
        using sort_t = sort<Sorter, Tys...>::_type;

        namespace sorters {
            constexpr auto size = []<class A, class B>{ return sizeof_v<A> < sizeof_v<B>; };
            constexpr auto rsize = []<class A, class B>{ return sizeof_v<A> > sizeof_v<B>; };
            constexpr auto alignment = []<class A, class B>{ return alignof_v<A> < alignof_v<B>; };
            constexpr auto ralignment = []<class A, class B>{ return alignof_v<A> > alignof_v<B>; };
        }

        // =======================================================

        template<class ...As>
        struct concat : detail::join_impl<detail::convert_to_info<As>...> {};

        template<class ...As>
        using concat_t = concat<As...>::type;

        // =======================================================

        namespace detail {
            template<std::size_t I, class ...As>
            using _zip_el = info<typename element<I, As>::type...>;

            template<class, class...>
            struct zip_impl_base;

            template<std::size_t ...Is, class ...As>
            struct zip_impl_base<std::index_sequence<Is...>, As...> {
                using type = info<_zip_el<Is, As...>...>;
                using _type = type;
            };

            template<class... As>
            struct zip_impl : zip_impl_base<std::make_index_sequence<std::min({ As::size... })>, As...> {};
        }

        template<class ...As>
        struct zip : detail::zip_impl<detail::convert_to_info<As>...> {};

        template<class ...As>
        using zip_t = zip<As...>::type;

        // =======================================================

        namespace detail {
            template<class...> 
            struct cartesian_impl_base;

            template<class ...As>
            struct cartesian_impl_base<info<As...> > {
                using type = info<As...>;
            };

            template<class...As>
            struct cartesian_impl_base<info<info<>>, As... > {
                using type = info<>;
            };

            template<class...As, class ...Cs>
            struct cartesian_impl_base<info<As...>, info<>, Cs... > {
                using type = info<>;
            };

            template<class ...As, class B, class ...Cs>
            struct cartesian_impl_base<info<As...>, info<B>, Cs...>
                : cartesian_impl_base<info<typename join_impl<As, info<B>>::type...>, Cs...> {};

            template<class ...As, class B, class ...Bs, class ...Cs>
            struct cartesian_impl_base<info<As...>, info<B, Bs...>, Cs...>
                : cartesian_impl_base<typename join_impl<
                     info<typename join_impl<As, info<B>>::type...>,
                     typename cartesian_impl_base<info<As...>, info<Bs...>>::type
                 >::type, Cs...> {};

            template<class, class...>
            struct cartesian_impl;

            template<class ...As, class ...Bs>
            struct cartesian_impl<info<As...>, Bs...> : cartesian_impl_base<info<info<As>...>, Bs...> {};
        }

        template<class ...As>
        struct cartesian : detail::cartesian_impl<detail::convert_to_info<As>...> {};
        
        template<class ...Tys>
        using cartesian_t = typename cartesian<Tys...>::type;
    }
}