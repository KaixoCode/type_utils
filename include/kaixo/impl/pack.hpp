#pragma once
#include "type_traits.hpp"

namespace kaixo {

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

    /**
     * Partially specialize T by setting the first
     * template parameters to ...As
     * @tparam T templated type
     * @tparam ...As types to
     */
    template<template<class...> class T, class ...As>
    struct partial_first {
        template<class ...Bs>
        using type = T<As..., Bs...>;
    };
    template<template<class...> class T, class ...As>
    using partial = partial_first<T, As...>;

    /**
     * Partially specialize T by setting the last
     * template parameters to ...As
     * @tparam T templated type
     * @tparam ...As types to
     */
    template<template<class...> class T, class ...As>
    struct partial_last {
        template<class ...Bs>
        using type = T<Bs..., As...>;
    };

    template<class ...Tys>
    struct tparams {
        using type = info<typename tparams<Tys>::type...>;
    };
    template<class Ty> struct tparams<Ty> { using type = Ty; };
    template<template<class...> class T, class ...Tys>
    struct tparams<T<Tys...>> {
        using type = info<Tys...>;
    };

    /**
     * Get the template parameters from a templated type.
     * @tparam Ty templated type
     */
    template<class ...Tys>
    using tparams_t = typename tparams<Tys...>::type;

    template<class Ty> struct uninstantiate { using type = Ty; };
    template<template<class...> class T, class ...Tys>
    struct uninstantiate<T<Tys...>> {
        using type = templated_t<T>;
    };

    /**
     * Remove template parameters from templated type.
     * @tparam Ty templated type
     */
    template<class Ty>
    using uninstantiate_t = typename uninstantiate<Ty>::type;

    template<class, class...> struct instantiate;

    template<template<class...> class T, class ...Tys>
    struct instantiate<info<Tys...>, templated_t<T>> {
        using type = T<Tys...>;
    };

    template<class T>
    struct instantiate<T> {
        template<class Ty>
        using type = instantiate<T, Ty>::type;
    };

    /**
     * Specialize a templated type.
     * @tparam T pack of types to instantiate with
     * @tparam Ty templated type
     */
    template<class T, class Ty>
    using instantiate_t = typename instantiate<T, Ty>::type;

    template<class, class...> struct reinstantiate;
    template<template<class...> class T, class ...Args, class Ty>
    struct reinstantiate<Ty, T<Args...>> {
        using type = T<Ty>;
    };

    template<template<class...> class T, class ...Args, class ...Tys>
    struct reinstantiate<info<Tys...>, T<Args...>> {
        using type = T<Tys...>;
    };

    template<class Ty>
    struct reinstantiate<Ty> {
        template<class T>
        using type = reinstantiate<Ty, T>::type;
    };

    /**
     * Specialize a templated type with new parameters.
     * @tparam T pack of types to reinstantiate with
     * @tparam Ty templated type
     */
    template<class T, class Ty>
    using reinstantiate_t = typename reinstantiate<T, Ty>::type;

    template<class T, template<class...> class...> struct move_tparams;
    template<class T, template<class...> class Ty>
    struct move_tparams<T, Ty> { 
        using type = Ty<T>; 
    };

    template<template<class...> class T, class ...Args, template<class...> class Ty>
    struct move_tparams<T<Args...>, Ty> { 
        using type = Ty<Args...>; 
    };

    template<class ...Args, template<class...> class Ty>
    struct move_tparams<template_pack<Args...>, Ty> { 
        using type = Ty<Args&&...>; 
    };

    /**
     * Move the template parameters from T to Ty. If T is
     * not templated, it will itself be used as the template parameter.
     * @tparam T type or type with template parameters
     * @tparam Ty templated type
     */
    template<class T, template<class...> class Ty>
    using move_tparams_t = typename move_tparams<T, Ty>::type;

    /**
     * Type linked to an index.
     * @tparam I index
     * @tparam Ty type
     */
    template<std::size_t I, class Ty> struct indexed {
        constexpr static std::size_t index = I;
        using type = Ty;
    };

    template<class, class...> struct indexer_impl;
    template<std::size_t ...Is, class ...Args>
    struct indexer_impl<std::index_sequence<Is...>, Args...> : indexed<Is, Args>... {};

    /**
     * Maps indices to all types in ...Args.
     * @tparam ...Args types
     */
    template<class ...Args>
    using indexer = indexer_impl<std::index_sequence_for<Args...>, Args...>;

    template<class, class...> struct reverse_indexer_impl;
    template<std::size_t ...Is, class ...Args>
    struct reverse_indexer_impl<std::index_sequence<Is...>, Args...>
        : indexed<sizeof...(Args) - Is - 1, Args>... {};

    /**
     * Maps indices in decending order to all types in ...Args.
     * @tparam ...Args types
     */
    template<class ...Args>
    using reverse_indexer = reverse_indexer_impl<
        std::index_sequence_for<Args...>, Args...>;

    template<std::size_t I, class Ty> // Overload resolution to get element at
    consteval indexed<I, Ty> element_impl(indexed<I, Ty>) {}; // index I

    template<std::size_t I, class ...Args>
    struct element {
        using type = typename decltype(element_impl<I>(indexer<Args...>{}))::type;
    };
    
    template<std::size_t I, class ...Args>
    struct element<I, info<Args...>> {
        using type = typename decltype(element_impl<I>(indexer<Args...>{}))::type;
    };

    template<std::size_t I>
    struct element<I> {
        template<class ...Args>
        using type = element<I, Args...>::type;
    };

    /**
     * Get the I'th type in ...Args.
     * @tparam I index
     * @tparam ...Args types
     */
    template<std::size_t I, class ...Args>
    using element_t = typename element<I, Args...>::type;

    template<class Ty, class ...Args>
    struct index {
        constexpr static std::size_t _get() {
            std::size_t index = 0;
            ((++index, std::is_same_v<Ty, Args>) || ...);
            return index - 1;
        }

        constexpr static std::size_t value = _get();
    };

    template<class Ty, class ...Args>
    struct index<Ty, info<Args...>> : index<Ty, Args...> {};

    /**
     * Get the first index of Ty in ...Args.
     * @tparam Ty type to find index of
     * @tparam ...Args types
     */
    template<class Ty, class ...Args>
    constexpr std::size_t index_v = index<Ty, Args...>::value;

    template<class Ty, class ...Args>
    struct last_index {
        constexpr static std::size_t _get() {
            std::size_t _fromEnd = 0; // increment, but reset on match
            ((std::is_same_v<Ty, Args> ? _fromEnd = 0 : ++_fromEnd), ...);
            std::size_t _index = sizeof...(Args) - _fromEnd - 1;
            return _fromEnd == sizeof...(Args) ? npos : _index;
        }

        constexpr static std::size_t value = _get();
    };

    template<class Ty, class ...Args>
    struct last_index<Ty, info<Args...>> : last_index<Ty, Args...> {};

    /**
     * Get the last index of Ty in ...Args.
     * @tparam Ty type to find index of
     * @tparam ...Args types
     */
    template<class Ty, class ...Args>
    constexpr std::size_t last_index_v = last_index<Ty, Args...>::value;

    template<class Ty, class ...Args>
    struct count : std::integral_constant<std::size_t, ((std::is_same_v<Ty, Args>) + ... + 0)> {};

    template<class Ty, class ...Args>
    struct count<Ty, info<Args...>> : count<Ty, Args...> {};

    /**
     * Count the number of occurences of Ty in ...Args.
     * @tparam Ty type to count
     * @tparam ...Args types
     */
    template<class Ty, class ...Args>
    constexpr std::size_t count_v = count<Ty, Args...>::value;

    template<class Ty, class ...Args>
    struct occurs : std::bool_constant<(count_v<Ty, Args...> > 0)> {};

    template<class Ty, class ...Args>
    struct occurs<Ty, info<Args...>> : occurs<Ty, Args...> {};

    /**
     * Check if Ty occurs in ...Args.
     * @tparam Ty type to check
     * @tparam ...Args types
     */
    template<class Ty, class ...Args>
    constexpr bool occurs_v = occurs<Ty, Args...>::value;

    template<std::size_t From, std::size_t To, std::size_t ...Except>
    struct generate_indices {
        constexpr static std::array<std::size_t, To - From - sizeof...(Except)> value = [] {
            std::array<std::size_t, To - From - sizeof...(Except)> _indices{};
            std::size_t _index = 0;
            std::size_t _match = 0;
            for (std::size_t _index = From; _index < To; ++_index)
                if (((_index != Except) && ...)) _indices[_match++] = _index;
            return _indices;
        }();
    };

    /**
     * Generate an array of indices from From to To,
     * leaving out all in indices ...Except.
     * @tparam From start of range
     * @tparam To end of range
     * @tparam ...Except values to skip
     */
    template<std::size_t From, std::size_t To, std::size_t ...Except>
    constexpr std::array<std::size_t, To - From - sizeof...(Except)>
        generate_indices_v = generate_indices<From, To, Except...>::value;

    template<class Ty, class ...Args>
    struct indices {
        constexpr static std::array<std::size_t, count_v<Ty, Args...>> value = [] {
            std::array<std::size_t, count_v<Ty, Args...>> _result{};
            std::size_t _index = 0, _match = 0;
            ((std::is_same_v<Ty, Args> ?
                _result[_match++] = _index++ : ++_index), ...);
            return _result;
        }();
    };

    template<class ...As, class ...Args>
    struct indices<info<As...>, Args...> {
        constexpr static auto _count = (count_v<As, Args...> +... + 0);
        constexpr static std::array<std::size_t, _count> value = [] {
            std::array<std::size_t, _count> _result{};
            std::size_t _index = 0, _match = 0;
            ((occurs_v<Args, As...> ?
                _result[_match++] = _index++ : ++_index), ...);
            return _result;
        }();
    };

    template<class A, class ...Args>
    struct indices<A, info<Args...>> : indices<A, Args...> {};

    template<class ...As, class ...Args>
    struct indices<info<As...>, info<Args...>> : indices<info<As...>, Args...> {};

    /**
     * Indices of Ty in ...Args.
     * @tparam Ty type, or info<Types...> for multiple
     * @tparam ...Args types
     */
    template<class Ty, class ...Args>
    constexpr auto indices_v = indices<Ty, Args...>::value;

    template<class Ty, class ...Args>
    struct indices_except {
        constexpr static std::array<std::size_t, sizeof...(Args) - count_v<Ty, Args...>> value = [] {
            std::array<std::size_t, sizeof...(Args) - count_v<Ty, Args...>> _result{};
            std::size_t _index = 0, _match = 0;
            ((std::is_same_v<Ty, Args> ?
                ++_index : _result[_match++] = _index++), ...);
            return _result;
        }();
    };

    template<class ...As, class ...Args>
    struct indices_except<info<As...>, Args...> {
        constexpr static auto _count = sizeof...(Args) - (count_v<As, Args...> +... + 0);
        constexpr static std::array<std::size_t, _count> value = [] {
            std::array<std::size_t, _count> _result{};
            std::size_t _index = 0, _match = 0;
            ((occurs_v<Args, As...> ?
                ++_index : _result[_match++] = _index++), ...);
            return _result;
        }();
    };

    template<class A, class ...Args>
    struct indices_except<A, info<Args...>> : indices_except<A, Args...> {};

    template<class ...As, class ...Args>
    struct indices_except<info<As...>, info<Args...>> : indices_except<info<As...>, Args...> {};

    /**
     * Indices of Ty in ...Args.
     * @tparam Ty type, or info<Types...> for multiple
     * @tparam ...Args types
     */
    template<class Ty, class ...Args>
    constexpr auto indices_except_v = indices_except<Ty, Args...>::value;

    template<class...> struct reverse;
    template<template<class...> class T, class ...As>
    struct reverse<T<As...>> {
        template<class> struct helper;
        template<std::size_t... Is>
        struct helper<std::index_sequence<Is...>> {
            using type = T<typename decltype(
                element_impl<Is>(reverse_indexer<As...>{}))::type... > ;
        };
        using type = typename helper<std::index_sequence_for<As...>>::type;
    };

    template<>
    struct reverse<> {
        template<class T>
        using type = reverse<T>::type;
    };

    /**
     * Reverse the template parameters of Ty.
     * @tparam Ty templated type
     */
    template<class Ty>
    using reverse_t = typename reverse<Ty>::type;

    template<class ...Args>
    struct unique_count {
        constexpr static std::size_t _get() {
            std::size_t _index = 0, _match = 0;
            ((_match += index_v<Args, Args...> == _index++), ...);
            return _match;
        }

        constexpr static std::size_t value = _get();
    };

    template<class ...Args>
    struct unique_count<info<Args...>> : unique_count<Args...> {};

    /**
     * Amount of unique types in ...Args.
     * @tparam ...Args types
     */
    template<class ...Args>
    constexpr std::size_t unique_count_v = unique_count<Args...>::value;

    template<class ...Args>
    struct first_indices {
        constexpr static std::array<std::size_t, unique_count_v<Args...>> value = [] {
            std::array<std::size_t, unique_count_v<Args...>> _result{};
            std::size_t _index = 0, _match = 0;
            (((index_v<Args, Args...> == _index) ?
                _result[_match++] = _index++ : ++_index), ...);
            return _result;
        }();
    };

    template<class ...Args>
    struct first_indices<info<Args...>> : first_indices<Args...> {};

    /**
     * Get the index of the first occurence of each type in ...Args.
     * @tparam ...Args types
     */
    template<class ...Args>
    constexpr std::array<std::size_t, unique_count_v<Args...>>
        first_indices_v = first_indices<Args...>::value;

    template<std::size_t, class...> struct take;
    template<std::size_t N, template<class...> class T, class ...As>
    struct take<N, T<As...>> {
        template<class> struct helper;
        template<std::size_t ...Is>
        struct helper<std::index_sequence<Is...>> {
            using type = T<element_t<Is, As...>...>;
        };
        using type = typename helper<std::make_index_sequence<N>>::type;
    };

    template<std::size_t N>
    struct take<N> {
        template<class Ty>
        using type = take<N, Ty>::type;
    };

    /**
     * Take N types from the templated type Ty.
     * @tparam N amount of types to take
     * @tparam Ty templated type
     */
    template<std::size_t N, class Ty>
    using take_t = typename take<N, Ty>::type;

    template<std::size_t, class...> struct drop;
    template<std::size_t N, template<class...> class T, class ...As>
    struct drop<N, T<As...>> {
        template<class> struct helper;
        template<std::size_t ...Is>
        struct helper<std::index_sequence<Is...>> {
            using type = T<element_t<Is + N, As...>...>;
        };
        using type = typename helper<std::make_index_sequence<sizeof...(As) - N>>::type;
    };

    template<std::size_t N>
    struct drop<N> {
        template<class Ty>
        using type = drop<N, Ty>::type;
    };

    /**
     * Drop N types from the templated type Ty.
     * @tparam N amount of types to drop
     * @tparam Ty templated type
     */
    template<std::size_t N, class Ty>
    using drop_t = typename drop<N, Ty>::type;

    template<std::size_t, class...> struct drop_last;
    template<std::size_t N, template<class...> class T, class ...As>
    struct drop_last<N, T<As...>> {
        template<class> struct helper;
        template<std::size_t ...Is>
        struct helper<std::index_sequence<Is...>> {
            using type = T<element_t<Is, As...>...>;
        };
        using type = typename helper<std::make_index_sequence<sizeof...(As) - N>>::type;
    };    
    
    template<std::size_t N>
    struct drop_last<N> {
        template<class Ty>
        using type = drop_last<N, Ty>::type;
    };

    /**
     * Drop last N types from the templated type Ty.
     * @tparam N amount of types to drop
     * @tparam Ty templated type
     */
    template<std::size_t N, class Ty>
    using drop_last_t = typename drop_last<N, Ty>::type;

    template<auto, class...> struct keep_indices;
    template<auto Array, template<class...> class T, class ...As>
    struct keep_indices<Array, T<As...>> {
        template<std::size_t ...Is> struct helper {
            using type = T<element_t<Is, As...>...>;
        };
        using type = typename array_to_pack_t<Array, helper>::type;
    };    
    
    template<auto Array>
    struct keep_indices<Array> {
        template<class T>
        using type = keep_indices<Array, T>::type;
    };

    /**
     * Only keep the types at the indices in Array.
     * @tparam Array std::array of indices
     * @tparam T templated type
     */
    template<auto Array, class T>
    using keep_indices_t = typename keep_indices<Array, T>::type;

    template<auto, class...> struct remove_indices;
    template<auto Array, template<class...> class T, class ...As>
    struct remove_indices<Array, T<As...>> {
        template<std::size_t ...Is> struct helper {
            using type = keep_indices_t<generate_indices_v<0, sizeof...(As), Is...>, T<As...>>;
        };
        using type = typename array_to_pack_t<Array, helper>::type;
    };    

    template<auto Array>
    struct remove_indices<Array> {
        template<class T>
        using type = remove_indices<Array, T>::type;
    };

    /**
     * Remove the types at the indices in Array.
     * @tparam Array std::array of indices
     * @tparam T templated type
     */
    template<auto Array, class T>
    using remove_indices_t = typename remove_indices<Array, T>::type;

    template<class, class...> struct remove;
    template<class Ty, template<class...> class T, class ...As>
    struct remove<Ty, T<As...>> {
        using type = typename keep_indices<
            indices_except_v<Ty, As...>, T<As...>>::type;
    };    
    
    template<class Ty>
    struct remove<Ty> {
        template<class T>
        using type = remove<Ty, T>::type;
    };

    /**
     * Remove the type T from the template parameters of Ty.
     * @tparam T type, or info<Types...> for multiple
     * @tparam Ty templated type
     */
    template<class T, class Ty>
    using remove_t = typename remove<T, Ty>::type;

    template<class, class...> struct keep;
    template<class Ty, template<class...> class T, class ...As>
    struct keep<Ty, T<As...>> {
        using type = typename keep_indices<
            indices_v<Ty, As...>, T<As...>>::type;
    };    
    
    template<class Ty>
    struct keep<Ty> {
        template<class T>
        using type = keep<Ty, T>::type;
    };

    /**
     * Only keep the type T in the template parameters of Ty.
     * @tparam T type, or info<Types...> for multiple
     * @tparam Ty templated type
     */
    template<class T, class Ty>
    using keep_t = typename keep<T, Ty>::type;

    template<std::size_t, class...> struct erase;
    template<std::size_t I, template<class...> class T, class ...As>
    struct erase<I, T<As...>> {
        using type = keep_indices_t<
            generate_indices_v<0, sizeof...(As), I>, T<As... >>;
    };    
    
    template<std::size_t I>
    struct erase<I> {
        template<class T>
        using type = erase<I, T>::type;
    };

    /**
     * Erase the type at index I from the template parameters of Ty.
     * @tparam I index
     * @tparam Ty templated type
     */
    template<std::size_t I, class Ty>
    using erase_t = typename erase<I, Ty>::type;

    template<class, class...> struct append;
    template<class Ty, template<class...> class T, class ...As>
    struct append<Ty, T<As...>> {
        using type = T<As..., Ty>;
    };

    template<class ...Bs, template<class...> class T, class ...As>
    struct append<info<Bs...>, T<As...>> {
        using type = T<As..., Bs...>;
    };

    template<class Ty>
    struct append<Ty> {
        template<class T>
        using type = append<Ty, T>::type;
    };

    /**
     * Append T to the template parameters of Ty.
     * @tparam T type or info<Types...> for multiple
     * @tparam Ty templated type
     */
    template<class T, class Ty>
    using append_t = typename append<T, Ty>::type;

    template<class, class...> struct prepend;
    template<class Ty, template<class...> class T, class ...As>
    struct prepend<Ty, T<As...>> {
        using type = T<Ty, As...>;
    };

    template<class ...Bs, template<class...> class T, class ...As>
    struct prepend<info<Bs...>, T<As...>> {
        using type = T<Bs..., As...>;
    };

    template<class Ty>
    struct prepend<Ty> {
        template<class T>
        using type = prepend<Ty, T>::type;
    };

    /**
     * Prepend T to the template parameters of Ty.
     * @tparam T type or info<Types...> for multiple
     * @tparam Ty templated type
     */
    template<class T, class Ty>
    using prepend_t = typename prepend<T, Ty>::type;

    template<std::size_t, class, class...> struct insert;

    template<std::size_t I, class T, class Ty>
    struct insert<I, T, Ty> {
        using _as_info = move_tparams_t<Ty, info>;
        using _result = append_t<drop_t<I, _as_info>, append_t<T, take_t<I, _as_info>>>;

        template<class, class> struct helper;
        template<class Info, template<class...> class Ty, class ...As>
        struct helper<Info, Ty<As...>> {
            using type = move_tparams_t<Info, Ty>;
        };

        using type = typename helper<_result, Ty>::type;
    };

    template<std::size_t I, class T>
    struct insert<I, T> {
        template<class Ty>
        using type = insert<I, T, Ty>::type;
    };

    /**
     * Insert T at I in the template parameters of Ty.
     * @tparam I index
     * @tparam T type or info<Types...> for multiple
     * @tparam Ty templated type
     */
    template<std::size_t I, class T, class Ty>
    using insert_t = typename insert<I, T, Ty>::type;

    template<class...> struct unique;
    template<template<class...> class T, class ...As>
    struct unique<T<As...>> {
        template<class> struct helper;
        template<std::size_t ...Is>
        struct helper<std::index_sequence<Is...>> {
            using type = T<element_t<
                first_indices_v<As...>[Is], As...>...>;
        };
        using type = typename helper<
            std::make_index_sequence<unique_count_v<As...>>>::type;
    };

    template<>
    struct unique<> {
        template<class Ty>
        using type = unique<Ty>::type;
    };

    /**
     * Only keep unique types in the template parameters of Ty.
     * @tparam Ty templated type
     */
    template<class Ty>
    using unique_t = typename unique<Ty>::type;

    /**
     * Only keep the types from index S to E in the
     * template parameters of Ty.
     * @tparam Ty templated type
     */
    template<std::size_t S, std::size_t E, class Ty>
    using sub_t = take_t<(E - S), drop_t<S, Ty>>;

    template<std::size_t, std::size_t, class...> struct sub;
    template<std::size_t S, std::size_t E, class Ty>
    struct sub<S, E, Ty> {
        using type = sub_t<S, E, Ty>;
    };

    template<std::size_t S, std::size_t E>
    struct sub<S, E> {
        template<class Ty>
        using type = sub<S, E, Ty>::type;
    };

    template<class L>
    struct filter_object_wrapper {
        consteval filter_object_wrapper(L value) : value(value) {}
        L value;
    };

    template<class L, std::size_t I, class Ty>
    concept _call_type0 = requires (L l) { // Only type -> bool
        { l.template operator() < Ty > () } -> convertible_to<bool>;
    };

    template<class L, std::size_t I, class Ty>
    concept _call_type1 = requires (L l) { // Index, type -> bool
        { l.template operator() < I, Ty > () } -> convertible_to<bool>;
    };

    template<class L, std::size_t I, class Ty>
    concept _call_type2 = requires (L l) { // Only index -> bool
        { l.template operator() < I > () } -> convertible_to<bool>;
    };

    template<class L, std::size_t I, class Ty>
    concept _call_type3 = requires (L l) { // Type -> void (type constraint)
        { l.template operator() < Ty > () } -> same_as<void>;
    };

    template<class L, std::size_t I, class Ty> // Type trait
    concept _call_type4 = requires (L) { L::template value<Ty>; };

    template<class L, std::size_t I, class Ty>// Value
    concept _call_type5 = requires(L l) { { l == Ty::value } -> convertible_to<bool>; };

    template<class L, std::size_t I, class Ty>// Value
    concept _call_type6 = std::same_as<L, bool>;

    template<class L>
    struct wrap_filter_object { using type = filter_object_wrapper<L>; };
    template<class L> requires is_type_trait<L>
    struct wrap_filter_object<L> { using type = L; };

    /**
     * Filter object with operator overloads for several cases
     * for the type filter.
     * @tparam L filter type
     */
    template<class L>
    struct filter_object : wrap_filter_object<L>::type {
        template<std::size_t I, class Ty> consteval bool call() {
            if constexpr (_call_type6<L, I, Ty>) return this->value;
            else if constexpr (_call_type0<L, I, Ty>) return this->value.template operator() < Ty > ();
            else if constexpr (_call_type1<L, I, Ty>) return this->value.template operator() < I, Ty > ();
            else if constexpr (_call_type2<L, I, Ty>) return this->value.template operator() < I > ();
            else if constexpr (_call_type3<L, I, Ty>) return true;
            else if constexpr (_call_type4<L, I, Ty>) return L::template value<Ty>;
            else if constexpr (_call_type5<L, I, Ty>) return this->value == Ty::value;
            else return false; // Otherwise always return false
        }
    };

    template<class T> filter_object(T)->filter_object<T>;

    template<auto Filter, class ...Args>
    consteval std::size_t count_filter_impl(info<Args...>) {
        return ((static_cast<std::size_t>(filter_object{ Filter }.
            template call<Args::template element<0>::value,
            typename Args::template element<1>::type>())) + ... + 0);
    }

    template<auto Filter, class ...Args>
    struct count_filter {
        template<class> struct helper;
        template<std::size_t ...Is>
        struct helper<std::index_sequence<Is...>> {
            consteval static std::size_t value() {
                return ((static_cast<std::size_t>(filter_object{ Filter }.template call<Is, Args>())) + ... + 0);
            }
        };

        constexpr static std::size_t value = helper<std::index_sequence_for<Args...>>::value();
    };

    template<auto Filter, class ...Args>
    struct count_filter<Filter, info<Args...>> : count_filter<Filter, Args...> {};

    /**
     * Amount of types in ...Args that match Filter.
     * @tparam Filter lambda or type trait object
     * @tparam ...Args types
     */
    template<auto Filter, class ...Args>
    constexpr std::size_t count_filter_v = count_filter<Filter, Args...>::value;

    template<auto Filter, class ...Args>
    struct indices_filter {
        constexpr static std::array<std::size_t,
            count_filter_v<Filter, Args...>> value = [] {
            std::array<std::size_t, count_filter_v<Filter, Args...>> _indices{};
            std::size_t _index = 0, _match = 0;
            [&] <std::size_t ...Is>(std::index_sequence<Is...>) {
                ((filter_object{ Filter }.template call<Is, Args>() ?
                    _indices[_match++] = _index++ : ++_index), ...);
            }(std::index_sequence_for<Args...>{});
            return _indices;
        }();
    };

    template<auto Filter, class ...Args>
    struct indices_filter<Filter, info<Args...>> : indices_filter<Filter, Args...> {};

    /**
     * Get indices of types in ...Args that match Filter
     * given in the form of a Lambda, or type trait object.
     * @tparam Filter lambda or type trait object
     * @tparam ...Args types
     */
    template<auto Filter, class ...Args>
    constexpr std::array<std::size_t, count_filter_v<Filter, Args...>>
        indices_filter_v = indices_filter<Filter, Args...>::value;

    template<auto, class> struct filter;
    template<auto Filter, template<class...> class T, class ...Args>
    struct filter<Filter, T<Args...>> {
        template<class> struct helper;
        template<std::size_t ...Is>
        struct helper<std::index_sequence<Is...>> {
            using type = T<element_t<indices_filter_v<Filter, Args...>[Is], Args...>...>;
        };
        using type = typename helper<
            std::make_index_sequence<count_filter_v<Filter, Args...>>>::type;
    };

    /**
     * Filter the template parameters of Ty using Filter
     * @tparam Filter lambda or type trait object
     * @tparam Ty templated type
     */
    template<auto Filter, class Ty>
    using filter_t = typename filter<Filter, Ty>::type;

    template<auto Sorter, class A, class B>
    constexpr bool type_sorter_result = Sorter.operator() < A, B > ();

    template<auto, class, class> struct type_merge_sort_merge;
    template<auto Sorter, class A, class ...As, class B, class ...Bs>
        requires type_sorter_result<Sorter, A, B>
    struct type_merge_sort_merge<Sorter, info<A, As...>, info<B, Bs...>> {
        using _recurse = typename type_merge_sort_merge<Sorter, info<As...>, info<B, Bs...>>::type;
        using type = append_t<_recurse, info<A>>;
    };

    template<auto Sorter, class A, class ...As, class B, class ...Bs>
        requires (!type_sorter_result<Sorter, A, B>)
    struct type_merge_sort_merge<Sorter, info<A, As...>, info<B, Bs...>> {
        using _recurse = typename type_merge_sort_merge<Sorter, info<A, As...>, info<Bs...>>::type;
        using type = append_t<_recurse, info<B>>;
    };

    template<auto Sorter, class ...As>
    struct type_merge_sort_merge<Sorter, info<As...>, info<>> { using type = info<As...>; };

    template<auto Sorter, class ...Bs>
    struct type_merge_sort_merge<Sorter, info<>, info<Bs...>> { using type = info<Bs...>; };

    template<auto Sorter, class Ty> struct type_merge_sort
        : type_merge_sort<Sorter, move_tparams<Ty, info>> {};
    template<auto Sorter, class ...Tys>
    struct type_merge_sort<Sorter, info<Tys...>> {
        constexpr static std::size_t _mid = sizeof...(Tys) / 2.;
        using _left = typename type_merge_sort<Sorter, typename info<Tys...>::template take<_mid>>::type;
        using _right = typename type_merge_sort<Sorter, typename info<Tys...>::template drop<_mid>>::type;
        using type = typename type_merge_sort_merge<Sorter, _left, _right>::type;
    };

    template<auto Sorter, class Ty>
    struct type_merge_sort<Sorter, info<Ty>> {
        using type = info<Ty>;
    };

    /**
     * Merge sort algorithm for the template parameters of Ty.
     * @tparam Sorter lambda that takes 2 template parameters and returns a bool
     * @tparam Ty templated type
     */
    template<auto Sorter, class Ty>
    using type_merge_sort_t = typename type_merge_sort<Sorter, Ty>::type;

    template<auto, class...> struct sort_types;

    template<auto Sorter, class Ty>
    struct sort_types<Sorter, Ty> {
        using type = type_merge_sort_t<Sorter, Ty>;
    };

    template<auto Sorter>
    struct sort_types<Sorter> {
        template<class Ty>
        using type = sort_types<Sorter, Ty>::type;
    };

    /**
     * Sorting algorithm for the template parameters of Ty.
     * @tparam Sorter lambda that takes 2 template parameters and returns a bool
     * @tparam Ty templated type
     */
    template<auto Sorter, class Ty>
    using sort_types_t = typename type_merge_sort<Sorter, Ty>::type;

    /**
     * Some often used sorting methods for types.
     */
    namespace type_sorters {
        constexpr auto size = []<class A, class B>{ return sizeof_v<A> < sizeof_v<B>; };
        constexpr auto rsize = []<class A, class B>{ return sizeof_v<A> > sizeof_v<B>; };
        constexpr auto alignment = []<class A, class B>{ return alignof_v<A> < alignof_v<B>; };
        constexpr auto ralignment = []<class A, class B>{ return alignof_v<A> > alignof_v<B>; };
    }

    template<class...> struct concat { using type = info<>; };
    template<template<class...> class A, class ...As>
    struct concat<A<As...>> { using type = A<As...>; };
    template<template<class...> class A, template<class...> class B, class ...As, class ...Bs, class ...Rest>
    struct concat<A<As...>, B<Bs...>, Rest...> { using type = typename concat<A<As..., Bs...>, Rest...>::type; };

    /**
     * Concat all template parameters of all templated
     * types in ...Tys.
     * @tparam ...Tys templated types
     */
    template<class ...Tys>
    using concat_t = typename concat<Tys...>::type;

    template<class... As> struct zip {
        using _first = info<typename info<As...>::template type<0>>;
        template<class A, std::size_t I> using a_a_i = typename info<A>::tparams::template element<I>::type;
        template<std::size_t I> using at_index = typename _first::template reinstantiate<a_a_i<As, I>...>::type;
        template<std::size_t ...Is> struct helper {
            using type = typename _first::template reinstantiate<at_index<Is>...>::type;
        };
        using type = array_to_pack_t < generate_indices_v < 0, std::min({ info<As>::tparams::size... }) > , helper > ::type;
    };

    template<class A> struct zip<A> { using type = A; };
    template<> struct zip<> { using type = info<>; };

    /**
     * Zip all types in the info's ...As
     * @tparam ...As info types
     */
    template<class ...As> using zip_t = typename zip<As...>::type;

    template<class...> struct cartesian_helper;
    template<template<class...> class T, class ...As>
    struct cartesian_helper<T<As...> > {
        using type = T<As...>;
    };

    template<template<class...> class T, class...As>
    struct cartesian_helper<T<T<>>, As... > {
        using type = T<>;
    };

    template<template<class...> class T, class...As, class ...Cs>
    struct cartesian_helper<T<As...>, T<>, Cs... > {
        using type = T<>;
    };

    template<template<class...> class T, class ...As, class B, class ...Cs>
    struct cartesian_helper<T<As...>, T<B>, Cs...> {
        using type1 = T<concat_t<As, T<B>>...>;
        using type = typename cartesian_helper<type1, Cs...>::type;
    };

    template<template<class...> class T, class ...As, class B, class ...Bs, class ...Cs>
    struct cartesian_helper<T<As...>, T<B, Bs...>, Cs...> {
        using type1 = T<concat_t<As, T<B>>...>;
        using type2 = typename cartesian_helper<T<As...>, T<Bs...> >::type;
        using type3 = concat_t<type1, type2>;
        using type = typename cartesian_helper<type3, Cs...>::type;
    };

    template<class...> struct cartesian;
    template<template<class...> class T, class...As, class... Tys>
    struct cartesian<T<As...>, Tys...> {
        using type = typename cartesian_helper<T<T<As>...>, Tys...>::type;
    };

    /**
     * Get the cartesian product of the template types
     * of all templated types ...Tys.
     * @tparam ...Tys templated types
     */
    template<class ...Tys>
    using cartesian_t = typename cartesian<Tys...>::type;

    template<template<class...> class, class...> struct transform;

    template<template<class...> class T, class Ty>
    struct transform<T, Ty> { using type = T<Ty>; };

    template<template<class...> class T, class ...As>
    struct transform<T, info<As...>> { using type = info<T<As>...>; };

    template<template<class...> class T>
    struct transform<T> { 
        template<class Ty>
        using type = transform<T, Ty>::type; 
    };


    /**
     * Transform Ty using T.
     * @tparam T transform
     * @tparam Ty type
     */
    template<template<class...> class T, class Ty>
    using transform_t = typename transform<T, Ty>::type;

    template<auto, template<class...> class, class...>
    struct conditional_transform;
    
    template<auto Filter, template<class...> class T, class Ty>
    struct conditional_transform<Filter, T, Ty> { using type = Ty; };

    template<auto Filter, template<class...> class T, class Ty>
        requires (filter_object{ Filter }.template call<0, Ty>())
    struct conditional_transform<Filter, T, Ty> { using type = T<Ty>; };

    template<auto Filter, template<class...> class T, class ...As>
    struct conditional_transform<Filter, T, info<As...>> {
        using type = info<typename conditional_transform<Filter, T, As>::type...>;
    };

    template<auto Filter, template<class...> class T>
    struct conditional_transform<Filter, T> { 
        template<class Ty>
        using type = conditional_transform<Filter, T, Ty>::type; 
    };

    /**
     * Conditionally transform Ty using T if matched Filter.
     * @tparam Filter filter
     * @tparam T transform
     * @tparam Ty type
     */
    template<auto Filter, template<class...> class T, class Ty>
    using conditional_transform_t = typename conditional_transform<Filter, T, Ty>::type;

    template<class A, class B, class ...Args> 
    struct replace {
        using type = typename conditional_transform<is_same<A>, 
            typename partial_last<change, B>::type, info<Args...>>::type;
    };

    template<class ...As, class B, class ...Args>
    struct replace<info<As...>, B, Args...> {
        using type = typename conditional_transform<(is_same<As> || ...),
            typename partial_last<change, B>::type, info<Args...>>::type;
    };

    template<class A, class B, class ...Args>
    struct replace<A, B, info<Args...>> {
        using type = typename replace<A, B, Args...>::type;
    };

    template<class A, class B>
    struct replace<A, B> {
        template<class ...Args>
        using type = replace<A, B, Args...>::type;
    };

    /**
     * Replace A with B in ...Args
     * @tparam A type to replace, or info containing multiple types
     * @tparam B type to replace with
     * @tparam ...Args pack or single info to replace in
     */
    template<class A, class B, class ...Args>
    using replace_t = typename replace<A, B, Args...>::type;

    /**
     * Allows for a transform inside a filter. Used like:
     *   with<transform>(filter)
     */
    template<template<class...> class Ty> struct with_impl {
        template<class Trait> struct trait {
            template<class...Tys> struct type {
                constexpr static bool value = filter_object{ Trait{} }.template call<0, Ty<Tys...>>();
            };
        };

        template<class Ty> constexpr auto operator()(Ty) const
            -> kaixo::type_trait<typename trait<Ty>::type> {
            return {};
        }
    };

    template<template<class...> class Ty>
    constexpr auto with = with_impl<Ty>{};
}