#pragma once
#include "info.hpp"

namespace kaixo {
    /**
     * Helper for dealing with the actual values in a template pack.
     * Used like this:
     *
     * template<class ...Tys>
     * void my_fun(Tys&&...tys) {
     *     template_pack<Tys...> vals{ tys... };
     *     vals.get<0>(); // etc...
     *
     * @tparam ...Args types
     */
    template<class ...Args>
    struct template_pack : std::tuple<Args&&...> {
        constexpr template_pack(Args&...args) : std::tuple<Args&&...>{ std::forward<Args>(args)... } {}
    };

    template<class Ty> concept has_decayed_tuple = requires (Ty) { typename Ty::decayed_tuple; };

    namespace has {
        template<class Ty> struct decayed_tuple_impl : std::bool_constant<has_decayed_tuple<Ty>> {};

        constexpr auto decayed_tuple = type_trait<decayed_tuple_impl>{};
    }

    namespace grab {
        template<class Ty> struct decayed_tuple_impl { using type = Ty; };
        template<has_decayed_tuple Ty> struct decayed_tuple_impl<Ty> { using type = typename Ty::decayed_tuple; };

        template<class Ty> using decayed_tuple = decayed_tuple_impl<Ty>::type;
    }

    /**
     * This tuple is used for perfect forwarding in the tuple operations
     * defined down below. This will keep forwarding temporaries without moving.
     * Use the defined tuple conversion operator to convert temporaries to
     * values to prevent keeping the reference after lifetime.
     * 
     * Use case:
     * std::tuple vals = std::tuple{ 1, 2, 3 } | take<2>;
     * 
     * In the use case above, the int&& are not copied during the take<2>,
     * instead, take<2> keeps it as int&&. Then, defining 'vals' as an std::tuple
     * triggers the conversion operator, which ensures the temporaries are
     * turned into values.
     */
    template<class ...Args>
    struct forwarding_tuple : std::tuple<Args...> {
        constexpr forwarding_tuple(Args...args) 
            : std::tuple<Args...>{ std::forward<Args>(args)... } {}

        using decayed_tuple = info<Args...>
            ::template when<is_rvalue_reference>
                ::template transform<remove_reference_t>
            ::template transform<grab::decayed_tuple>
            ::template as<std::tuple>;

        constexpr operator decayed_tuple() && {
            return std::move(*static_cast<std::tuple<Args...>*>(this)); 
        }
    };
}

namespace std {
    template<class ...Args>
    struct tuple_size<kaixo::template_pack<Args...>> : std::integral_constant<std::size_t, sizeof...(Args)> {};
    template<std::size_t I, class ...Args>
    struct tuple_element<I, kaixo::template_pack<Args...>> : std::tuple_element<I, std::tuple<Args&&...>> {};

    template<class ...Args>
    struct tuple_size<kaixo::forwarding_tuple<Args...>> : std::integral_constant<std::size_t, sizeof...(Args)> {};
    template<std::size_t I, class ...Args>
    struct tuple_element<I, kaixo::forwarding_tuple<Args...>> : std::tuple_element<I, std::tuple<Args...>> {};

    // This deduction guide ensures when constructing a tuple with a forwarding tuple
    // triggers the decayed tuple conversion operator in the forwarding tuple.
    template<class ...Args>
    tuple(kaixo::forwarding_tuple<Args...>&&) 
        -> tuple<kaixo::grab::decayed_tuple<conditional_t<is_rvalue_reference_v<Args>, remove_reference_t<Args>, Args>>...>;
}

/**
 * Tuple manipulation helpers.
 */
namespace kaixo::tuple {
    /**
     * Copies qualifiers from type if not a reference.
     * @tparam Ty type to copy from
     */
    template<class Ty>
    struct _tpl_ref {
        template<class T>
        using type = std::conditional_t<!reference<T>, add_cvref_t<T, Ty>, T>;
    };

    template<class Ty, class Tuple>
    concept is_tuple_modifier = std::invocable<decay_t<Ty>, Tuple> && requires () {
        typename decay_t<Ty>::tuple_modifier;
    };

    template<std::size_t I> 
    struct get_impl {
        using tuple_modifier = int;

        template<class Ty> 
            requires structured_binding<decay_t<Ty>>
        constexpr decltype(auto) operator()(Ty&& tuple) const {
            using type = binding_types_t<decay_t<Ty>>
                ::template element<I>
                ::template transform<typename _tpl_ref<Ty&&>::type>
                ::type;

            return static_cast<type>(kaixo::get_binding_element<I>(std::forward<Ty>(tuple)));
        }
    };

    template<std::size_t I> constexpr auto get = get_impl<I>{};

    template<class T, is_tuple_modifier<T> Ty>
        requires structured_binding<decay_t<Ty>>
    constexpr decltype(auto) operator|(T&& tuple, Ty&& val) {
        return std::forward<Ty>(val)(std::forward<T>(tuple));
    }

    template<std::size_t I> 
    struct take_impl {
        using tuple_modifier = int;

        template<class Ty> 
            requires structured_binding<decay_t<Ty>>
        constexpr auto operator()(Ty&& tuple) const {
            using tuple_type = binding_types_t<decay_t<Ty>>
                ::template take<I>
                ::template transform<typename _tpl_ref<Ty&&>::type>
                ::template as<forwarding_tuple>;

            return sequence<I>([&]<std::size_t ...Is>{
                return tuple_type(get<Is>(std::forward<Ty>(tuple))...);
            });
        }
    };

    template<std::size_t I> 
    struct drop_impl {
        using tuple_modifier = int;

        template<class Ty> 
            requires structured_binding<decay_t<Ty>>
        constexpr auto operator()(Ty&& tuple) const {
            using types = binding_types_t<decay_t<Ty>>;
            using tuple_type = types
                ::template drop<I>
                ::template transform<typename _tpl_ref<Ty&&>::type>
                ::template as<forwarding_tuple>;

            return sequence<I, types::size>([&]<std::size_t ...Is>{
                return tuple_type(get<Is>(std::forward<Ty>(tuple))...);
            });
        }
    };

    template<std::size_t I> constexpr auto take = take_impl<I>{};
    template<std::size_t I> constexpr auto drop = drop_impl<I>{};

    template<std::size_t I> 
    struct last_impl {
        using tuple_modifier = int;

        template<class Ty> 
            requires structured_binding<decay_t<Ty>>
        constexpr auto operator()(Ty&& tuple) const {
            using types = binding_types_t<decay_t<Ty>>;
            return drop<types::size - I>(std::forward<Ty>(tuple));
        }
    };

    template<std::size_t I> 
    struct drop_last_impl {
        using tuple_modifier = int;

        template<class Ty> 
            requires structured_binding<decay_t<Ty>>
        constexpr auto operator()(Ty&& tuple) const {
            using types = binding_types_t<decay_t<Ty>>;
            return take<types::size - I>(std::forward<Ty>(tuple));
        }
    };

    template<std::size_t I>
    struct erase_impl {
        using tuple_modifier = int;

        template<class Ty>
            requires structured_binding<decay_t<Ty>>
        constexpr auto operator()(Ty&& tuple) const {
            using types = binding_types_t<decay_t<Ty>>;
            using tuple_type = types
                ::template erase<I>
                ::template transform<typename _tpl_ref<Ty&&>::type>
                ::template as<forwarding_tuple>;

            return iterate<generate_indices_v<0, types::size, I>>([&]<std::size_t ...Is>{
                return tuple_type(get<Is>(std::forward<Ty>(tuple))...);
            });
        }
    };

    template<std::size_t I> 
    struct insert_impl {
        template<class ...Args> 
        struct result {
            using tuple_modifier = int;
            std::tuple<Args...> _data{};
    
            template<class Self, class Ty> 
                requires structured_binding<decay_t<Ty>>
            constexpr auto operator()(this Self&& self, Ty&& tuple) {
                using types = binding_types_t<decay_t<Ty>>;
                using head = types
                    ::template take<I>
                    ::template transform<typename _tpl_ref<Ty&&>::type>;
                using middle = info<Args...>;
                using tail = types
                    ::template last<types::size - I>
                    ::template transform<typename _tpl_ref<Ty&&>::type>;
                using tuple_type = concat_t<head, middle, tail>
                    ::template as<forwarding_tuple>;

                return [&]<
                    std::size_t ...Is, 
                    std::size_t ...Ns, 
                    std::size_t ...Qs> (
                        std::index_sequence<Is...>, 
                        std::index_sequence<Ns...>, 
                        std::index_sequence<Qs...>) 
                {
                    return tuple_type(
                        get<Is>(std::forward<Ty>(tuple))..., 
                        get<Qs>(std::forward<Self>(self)._data)..., 
                        get<I + Ns>(std::forward<Ty>(tuple))...);
                }(
                    std::make_index_sequence<I>{}, 
                    std::make_index_sequence<types::size - I>{},
                    std::index_sequence_for<Args...>{});
            }
        };
    
        template<class ...Tys>
        constexpr result<Tys...> operator()(Tys&&...args) const {
            return result<Tys...>{._data{ std::forward<Tys>(args)... } };
        }

        template<class Ty, class ...Tys> 
            requires (structured_binding<decay_t<Ty>> && sizeof...(Tys) > 0)
        constexpr auto operator()(Ty&& tuple, Tys&&...args) const {
            using types = binding_types_t<decay_t<Ty>>;
            using head = types
                ::template take<I>
                ::template transform<typename _tpl_ref<Ty&&>::type>;
            using middle = info<Tys...>;
            using tail = types
                ::template last<types::size - I>
                ::template transform<typename _tpl_ref<Ty&&>::type>;
            using tuple_type = concat_t<head, middle, tail>
                ::template as<forwarding_tuple>;

            return[&]<
                std::size_t ...Is,
                std::size_t ...Ns> (
                    std::index_sequence<Is...>,
                    std::index_sequence<Ns...>)
            {
                return tuple_type(
                    get<Is>(std::forward<Ty>(tuple))...,
                    std::forward<Tys>(args)...,
                    get<I + Ns>(std::forward<Ty>(tuple))...);
            }(
                std::make_index_sequence<I>{},
                std::make_index_sequence<types::size - I>{});
        }
    };
    
    template<std::size_t I> constexpr auto erase = erase_impl<I>{};
    template<std::size_t I> constexpr auto insert = insert_impl<I>{};

    template<std::size_t I>
    struct swap_impl {
        template<class Arg> 
        struct result {
            using tuple_modifier = int;
            Arg _data;

            template<class Self, class Ty> 
                requires structured_binding<decay_t<Ty>>
            constexpr auto operator()(this Self&& self, Ty&& tuple) {
                return insert<I>(erase<I>(std::forward<Ty>(tuple)), 
                    std::forward<Arg&&>((std::forward<Self>(self)._data)));
            }
        };

        template<class Ty>
        constexpr result<Ty> operator()(Ty&& arg) const {
            return result<Ty>{._data = std::forward<Ty>(arg) };
        }
    };

    template<std::size_t A, std::size_t B> 
    struct sub_impl {
        using tuple_modifier = int;

        template<class Self, class Ty>
            requires structured_binding<decay_t<Ty>>
        constexpr auto operator()(this Self&& self, Ty&& tuple) {
            using tuple_type = binding_types_t<decay_t<Ty>>
                ::template sub<A, B>
                ::template transform<typename _tpl_ref<Ty&&>::type>
                ::template as<forwarding_tuple>;

            return iterate<generate_indices_v<A, B>>([&]<std::size_t ...Is>{
                return tuple_type(get<Is>(std::forward<Ty>(tuple))...);
            });
        }
    };

    template<class ...Args> 
    struct remove_impl {
        using tuple_modifier = int;

        template<class Self, class Ty>
            requires structured_binding<decay_t<Ty>>
        constexpr auto operator()(this Self&& self, Ty&& tuple) {
            using types = binding_types_t<decay_t<Ty>>;
            constexpr auto Indices = types::decay
                ::template indices_except<info<Args...>>;
            using tuple_type = types
                ::template keep_indices<Indices>
                ::template transform<typename _tpl_ref<Ty&&>::type>
                ::template as<forwarding_tuple>;

            return iterate<Indices>([&]<std::size_t ...Is>{
                return tuple_type(get<Is>(std::forward<Ty>(tuple))...);
            });
        }
    };

    template<std::size_t ...Is> 
    struct remove_indices_impl {
        using tuple_modifier = int;

        template<class Self, class Ty>
            requires structured_binding<decay_t<Ty>>
        constexpr auto operator()(this Self&& self, Ty&& tuple) {
            using tuple_type = binding_types_t<decay_t<Ty>>
                ::template remove_indices<as_array<Is...>>
                ::template transform<typename _tpl_ref<Ty&&>::type>
                ::template as<forwarding_tuple>;

            return iterate<as_array<Is...>>([&]<std::size_t ...Ns>{
                return tuple_type(get<Ns>(std::forward<Ty>(tuple))...);
            });
        }
    };

    template<class ...Args> 
    struct remove_raw_impl {
        using tuple_modifier = int;

        template<class Self, class Ty>
            requires structured_binding<decay_t<Ty>>
        constexpr auto operator()(this Self&& self, Ty&& tuple) {
            using types = binding_types_t<decay_t<Ty>>;
            constexpr auto Indices = types
                ::template indices_except<info<Args...>>;
            using tuple_type = types
                ::template keep_indices<Indices>
                ::template transform<typename _tpl_ref<Ty&&>::type>
                ::template as<forwarding_tuple>;

            return iterate<Indices>([&]<std::size_t ...Is>{
                return tuple_type(get<Is>(std::forward<Ty>(tuple))...);
            });
        }
    };

    template<class ...Args> 
    struct keep_impl {
        using tuple_modifier = int;

        template<class Self, class Ty>
            requires structured_binding<decay_t<Ty>>
        constexpr auto operator()(this Self&& self, Ty&& tuple) {
            using types = binding_types_t<decay_t<Ty>>;
            constexpr auto Indices = types::decay
                ::template indices_except<info<Args...>>;
            using tuple_type = types
                ::template remove_indices<Indices>
                ::template transform<typename _tpl_ref<Ty&&>::type>
                ::template as<forwarding_tuple>;

            return iterate<Indices>([&]<std::size_t ...Is>{
                return tuple_type(get<Is>(std::forward<Ty>(tuple))...);
            });
        }
    };

    template<std::size_t ...Is> 
    struct keep_indices_impl {
        using tuple_modifier = int;

        template<class Self, class Ty>
            requires structured_binding<decay_t<Ty>>
        constexpr auto operator()(this Self&& self, Ty&& tuple) {
            using tuple_type = binding_types_t<decay_t<Ty>>
                ::template keep_indices<as_array<Is...>>
                ::template transform<typename _tpl_ref<Ty&&>::type>
                ::template as<forwarding_tuple>;

            return iterate<as_array<Is...>>([&]<std::size_t ...Ns>{
                return tuple_type(get<Ns>(std::forward<Ty>(tuple))...);
            });
        }
    };

    template<class ...Args> 
    struct keep_raw_impl {
        using tuple_modifier = int;

        template<class Self, class Ty>
            requires structured_binding<decay_t<Ty>>
        constexpr auto operator()(this Self&& self, Ty&& tuple) {
            using types = binding_types_t<decay_t<Ty>>;
            constexpr auto Indices = types
                ::template indices_except<info<Args...>>;
            using tuple_type = types
                ::template remove_indices<Indices>
                ::template transform<typename _tpl_ref<Ty&&>::type>
                ::template as<forwarding_tuple>;

            return iterate<Indices>([&]<std::size_t ...Is>{
                return tuple_type(get<Is>(std::forward<Ty>(tuple))...);
            });
        }
    };

    struct append_impl {
        using tuple_modifier = int;

        template<class ...Args>
        struct result {
            using tuple_modifier = int;
            std::tuple<Args...> _data{};

            template<class Self, class Ty>
                requires structured_binding<decay_t<Ty>>
            constexpr auto operator()(this Self&& self, Ty&& tuple) {
                using types = binding_types_t<decay_t<Ty>>;
                using head = types
                    ::template transform<typename _tpl_ref<Ty&&>::type>;
                using middle = info<Args...>;
                using tuple_type = concat_t<head, middle>
                    ::template as<forwarding_tuple>;

                return[&]<
                    std::size_t ...Is,
                    std::size_t ...Qs> (
                        std::index_sequence<Is...>,
                        std::index_sequence<Qs...>) {
                    return tuple_type(
                        get<Is>(std::forward<Ty>(tuple))...,
                        get<Qs>(std::forward<Self>(self)._data)...);
                }(
                    std::make_index_sequence<types::size>{},
                    std::index_sequence_for<Args...>{});
            }
        };

        template<class ...Tys>
        constexpr result<Tys...> operator()(Tys&&...args) const {
            return result<Tys...>{._data{ std::forward<Tys>(args)... } };
        }

        template<class Ty, class ...Tys>
            requires (structured_binding<decay_t<Ty>> && sizeof...(Tys) > 0)
        constexpr auto operator()(Ty&& tuple, Tys&&...args) const {
            using types = binding_types_t<decay_t<Ty>>;
            using head = types
                ::template transform<typename _tpl_ref<Ty&&>::type>;
            using middle = info<Tys...>;
            using tuple_type = concat_t<head, middle>
                ::template as<forwarding_tuple>;

            return[&]<std::size_t ...Is> (std::index_sequence<Is...>) {
                return tuple_type(
                    get<Is>(std::forward<Ty>(tuple))...,
                    std::forward<Tys>(args)...);
            }(std::make_index_sequence<types::size>{});
        }
    };

    struct prepend_impl {
        using tuple_modifier = int;

        template<class ...Args>
        struct result {
            using tuple_modifier = int;
            std::tuple<Args...> _data{};

            template<class Self, class Ty>
                requires structured_binding<decay_t<Ty>>
            constexpr auto operator()(this Self&& self, Ty&& tuple) {
                using types = binding_types_t<decay_t<Ty>>;
                using middle = info<Args...>;
                using tail = types
                    ::template transform<typename _tpl_ref<Ty&&>::type>;
                using tuple_type = concat_t<middle, tail>
                    ::template as<forwarding_tuple>;

                return[&]<
                    std::size_t ...Is,
                    std::size_t ...Qs> (
                        std::index_sequence<Is...>,
                        std::index_sequence<Qs...>) {
                    return tuple_type(
                        get<Qs>(std::forward<Self>(self)._data)...,
                        get<Is>(std::forward<Ty>(tuple))...);
                }(
                    std::make_index_sequence<types::size>{},
                    std::index_sequence_for<Args...>{});
            }
        };

        template<class ...Tys>
        constexpr result<Tys...> operator()(Tys&&...args) const {
            return result<Tys...>{._data{ std::forward<Tys>(args)... } };
        }

        template<class Ty, class ...Tys>
            requires (structured_binding<decay_t<Ty>> && sizeof...(Tys) > 0)
        constexpr auto operator()(Ty&& tuple, Tys&&...args) const {
            using types = binding_types_t<decay_t<Ty>>;
            using middle = info<Tys...>;
            using tail = types
                ::template transform<typename _tpl_ref<Ty&&>::type>;
            using tuple_type = concat_t<middle, tail>
                ::template as<forwarding_tuple>;

            return[&]<std::size_t ...Is> (std::index_sequence<Is...>) {
                return tuple_type(
                    std::forward<Tys>(args)...,
                    get<Is>(std::forward<Ty>(tuple))...);
            }(std::make_index_sequence<types::size>{});
        }
    };

    struct unique_impl {
        using tuple_modifier = int;

        template<class Self, class Ty>
            requires structured_binding<decay_t<Ty>>
        constexpr auto operator()(this Self&& self, Ty&& tuple) {
            using types = binding_types_t<decay_t<Ty>>;
            using tuple_type = types::unique
                ::template transform<typename _tpl_ref<Ty&&>::type>
                ::template as<forwarding_tuple>;

            return iterate<first_indices_v<typename types::decay>>([&]<std::size_t ...Is>{
                return tuple_type(get<Is>(std::forward<Ty>(tuple))...);
            });
        }
    };

    struct reverse_impl {
        using tuple_modifier = int;

        template<class Self, class Ty>
            requires structured_binding<decay_t<Ty>>
        constexpr auto operator()(this Self&& self, Ty&& tuple) {
            using types = binding_types_t<decay_t<Ty>>;
            using tuple_type = types::reverse
                ::template transform<typename _tpl_ref<Ty&&>::type>
                ::template as<forwarding_tuple>;

            return reverse_sequence<0, types::size>([&]<std::size_t ...Is>{
                return tuple_type(get<Is>(std::forward<Ty>(tuple))...);
            });
        }
    };

    template<auto Filter> 
    struct filter_impl {
        using tuple_modifier = int;

        template<class Self, class Ty>
            requires structured_binding<decay_t<Ty>>
        constexpr auto operator()(this Self&& self, Ty&& tuple) {
            using types = binding_types_t<decay_t<Ty>>;
            constexpr auto Indices = types
                ::template indices_filter<Filter>;
            using tuple_type = types
                ::template keep_indices<Indices>
                ::template transform<typename _tpl_ref<Ty&&>::type>
                ::template as<forwarding_tuple>;

            return iterate<Indices>([&]<std::size_t ...Is>{
                return tuple_type(get<Is>(std::forward<Ty>(tuple))...);
            });
        }
    };

    struct call_impl {
        template<class Functor> 
        struct result {
            using tuple_modifier = int;
            Functor _functor;

            template<class Self, class Ty>
                requires structured_binding<decay_t<Ty>>
            constexpr auto operator()(this Self&& self, Ty&& tuple) {
                using types = binding_types_t<decay_t<Ty>>;
                return sequence<0, types::size>([&]<std::size_t ...Is>() -> decltype(auto) {
                    return std::forward<Functor>(std::forward<Self>(self)._functor)(get<Is>(std::forward<Ty>(tuple))...);
                });
            }
        };

        template<class Functor>
        constexpr result<Functor> operator()(Functor&& functor) const {
            return result<Functor>{ std::forward<Functor>(functor) };
        }

        template<class Functor, class Ty>
            requires structured_binding<decay_t<Ty>>
        constexpr result<Functor> operator()(Functor&& functor, Ty&& tuple) const {
            using types = binding_types_t<decay_t<Ty>>;
            return sequence<0, types::size>([&]<std::size_t ...Is>() -> decltype(auto) {
                return std::forward<Functor>(functor)(get<Is>(std::forward<Ty>(tuple))...);
            });
        }
    };

    template<std::size_t I> constexpr auto drop_last = drop_last_impl<I>{};
    template<std::size_t I> constexpr auto last = last_impl<I>{};
    template<std::size_t I> constexpr auto swap = swap_impl<I>{};
    template<std::size_t A, std::size_t B> constexpr auto sub = sub_impl<A, B>{};
    template<class ...Tys> constexpr auto remove = remove_impl<Tys...>{};
    template<class ...Tys> constexpr auto remove_raw = remove_raw_impl<Tys...>{};
    template<class ...Tys> constexpr auto keep = keep_impl<Tys...>{};
    template<class ...Tys> constexpr auto keep_raw = keep_raw_impl<Tys...>{};
    template<std::size_t ...Is> constexpr auto keep_indices = keep_indices_impl<Is...>{};
    constexpr auto append = append_impl{};
    constexpr auto prepend = prepend_impl{};
    constexpr auto unique = unique_impl{};
    constexpr auto reverse = reverse_impl{};
    template<auto Filter> constexpr auto filter = filter_impl<Filter>{};
    constexpr auto call = call_impl{};

    template<class Ty>
    using _type_ref_impl = binding_types_t<decay_t<Ty>>
        ::template transform<typename _tpl_ref<Ty&&>::type>;

    constexpr auto zip = []<class ...Tys>
        requires (structured_binding<decay_t<Tys>> && ...) (Tys&&... tuples)
    {
        constexpr std::size_t min_size = std::min({ binding_types_t<decay_t<Tys>>::size... });
        using zipped = zip_t<_type_ref_impl<Tys>...>;
        
        auto _one = [&]<std::size_t I>(value_t<I>){
            using tuple_type = typename zipped
                ::template element<I>
                ::template as<forwarding_tuple>;

            return tuple_type(get<I>(std::forward<Tys>(tuples))...);
        };

        using tuple_type = zipped
            ::template transform<typename move_tparams<forwarding_tuple>::type>
            ::template as<forwarding_tuple>;

        return sequence<min_size>([&]<std::size_t ...Is> {
            return tuple_type(_one(value_t<Is>{})...);
        });
    };

    template<std::size_t, class> struct concat_helper;
    template<std::size_t I, std::size_t ...Is>
    struct concat_helper<I, std::index_sequence<Is...>> {
        using type = info<info<value_t<I>, value_t<Is>>...>;
    };

    constexpr auto concat = []<class ...Tys>
        requires (structured_binding<decay_t<Tys>> && ...) (Tys&&... tuples)
    {
        using types = info<binding_types_t<decay_t<Tys>>...>;
        using tuple_type = concat_t<_type_ref_impl<Tys>...>
            ::template as<forwarding_tuple>;
        template_pack<Tys...> _tuples{ tuples... };

        return sequence<sizeof...(Tys)>([&]<std::size_t ...Is>() {
            using indices = concat_t<typename concat_helper<Is, 
                std::make_index_sequence<types::template element<Is>::size>>::type...>;
            return indices::for_each([&]<class ...Index>{ 
                return tuple_type(
                    get<Index::template value<1>>(
                        get<Index::template value<0>>(_tuples))...
                );
            });
        });
    };

    constexpr auto cartesian = []<class ...Tys>
        requires (structured_binding<decay_t<Tys>> && ...) (Tys&&... tuples)
    {
        using cartesian_type = cartesian_t<_type_ref_impl<Tys>...>;
        template_pack<Tys...> _tuples{ tuples... };

        auto eval_at = [&]<std::size_t I>(value_t<I>) {
            return sequence<sizeof...(Tys)>([&]<std::size_t ...Is>{
                constexpr auto _indices = sequence<sizeof...(Tys)>([&]<std::size_t ...Ns>() {
                    constexpr std::array sizes{ binding_size_v<decay_t<Tys>>... };
                    std::size_t _t_pos = 0, _p_pos = I;
                    return std::array{ (_t_pos = _p_pos % sizes[Ns], _p_pos /= sizes[Ns], _t_pos)... };
                });;

                using tuple_type = typename cartesian_type
                    ::template element<I>
                    ::template as<forwarding_tuple>;

                return tuple_type(get<_indices[Is]>(get<Is>(_tuples))...);
            });
        };

        using tuple_type = cartesian_type
            ::template transform<typename move_tparams<forwarding_tuple>::type>
            ::template as<forwarding_tuple>;

        return sequence<(binding_size_v<decay_t<Tys>> * ... * 1)>([&]<std::size_t ...Is>{
            return tuple_type(eval_at(value_t<Is>{})...);
        });
    };
}