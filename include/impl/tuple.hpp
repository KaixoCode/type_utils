#pragma once
#include "info.hpp"

/**
 * Tuple manipulation helpers.
 */
namespace kaixo {

    template<class Ty, class Tuple>
    concept is_tuple_modifier = std::invocable<decay_t<Ty>, Tuple>;

    template<std::size_t I> struct get_v_impl {
        template<class ...Tys>
        constexpr decltype(auto) operator()(const std::tuple<Tys...>& tuple) const {
            using forward_types = info<Tys...>::
                template iff<not is_reference>::template transform<add_lvalue_reference_t>::
                template iff<is_rvalue_reference>::template transform<remove_reference_t>;
            using type = std::conditional_t<reference<typename info<Tys...>::template element<I>::type>,
                typename forward_types::template element<I>::type,
                const typename info<Tys...>::template element<I>::type&>;
            return std::forward<type>(std::get<I>(tuple));
        }
    };

    template<std::size_t I> constexpr auto get_v = get_v_impl<I>{};

    template<class T, is_tuple_modifier<T> Ty>
    constexpr decltype(auto) operator|(T&& tuple, Ty&& val) {
        return std::forward<Ty>(val)(std::forward<T>(tuple));
    }

    template<std::size_t I> struct take_v_impl {
        template<class ...Tys, class Type
            = typename info<Tys...>::template take<I>::template as<std::tuple>>
            constexpr auto operator()(const std::tuple<Tys...>& tuple) const -> Type {
            return sequence<I>([&]<std::size_t ...Is>{
                return Type{ (get_v<Is>(tuple))... };
            });
        }
    };

    template<std::size_t I> struct drop_v_impl {
        template<class ...Tys, class Type
            = typename info<Tys...>::template drop<I>::template as<std::tuple>>
            constexpr auto operator()(const std::tuple<Tys...>& tuple) const -> Type {
            return sequence<I, info<Tys...>::size>([&]<std::size_t ...Is>{
                return Type{ (get_v<Is>(tuple))... };
            });
        }
    };

    template<std::size_t I> constexpr auto take_v = take_v_impl<I>{};
    template<std::size_t I> constexpr auto drop_v = drop_v_impl<I>{};

    template<std::size_t I> struct last_v_impl {
        template<class ...Tys, class Type
            = typename info<Tys...>::template drop<info<Tys...>::size - I>::template as<std::tuple>>
            constexpr auto operator()(const std::tuple<Tys...>& tuple) const -> Type {
            return drop_v<info<Tys...>::size - I>(tuple);
        }
    };

    template<std::size_t I> struct drop_last_v_impl {
        template<class ...Tys, class Type
            = typename info<Tys...>::template drop_last<I>::template as<std::tuple>>
            constexpr auto operator()(const std::tuple<Tys...>& tuple) const -> Type {
            return take_v<info<Tys...>::size - I>(tuple);
        }
    };

    template<std::size_t I> struct erase_v_impl {
        template<class ...Tys, class Type
            = typename info<Tys...>::template erase<I>::template as<std::tuple>>
            constexpr auto operator()(const std::tuple<Tys...>& tuple) const -> Type {
            return iterate<generate_indices_v<0, info<Tys...>::size, I>>([&]<std::size_t ...Is>{
                return Type{ (get_v<Is>(tuple))... };
            });
        }
    };

    template<std::size_t I> struct insert_v_impl {
        template<class ...Args> struct result {
            std::tuple<Args&&...> _data{};

            template<class ...Tys, class Type
                = typename info<Tys...>::template insert<I, info<decay_t<Args>...>>::template as<std::tuple>>
                constexpr Type operator()(const std::tuple<Tys...>& tuple) const {
                return[&]<std::size_t ...Is, std::size_t ...Ns, std::size_t ...Qs>
                    (std::index_sequence<Is...>, std::index_sequence<Ns...>, std::index_sequence<Qs...>) {
                    return Type{ (get_v<Is>(tuple))..., (get_v<Qs>(_data))..., (get_v<I + Ns>(tuple))... };
                }(std::make_index_sequence<I>{}, std::make_index_sequence<info<Tys...>::size - I>{},
                    std::index_sequence_for<Args...>{});
            }
        };

        template<class ...Tys>
        constexpr result<Tys...> operator()(Tys&&...args) const {
            return result<Tys...>{._data{ std::forward<Tys>(args)... } };
        }
    };

    template<std::size_t I> constexpr auto erase_v = erase_v_impl<I>{};
    template<std::size_t I> constexpr auto insert_v = insert_v_impl<I>{};

    template<std::size_t I> struct swap_v_impl {
        template<class Ty> struct result {
            Ty&& _data;
            template<class ...Tys, class Type
                = typename info<Tys...>::template swap<I, decay_t<Ty>>::template as<std::tuple>>
                constexpr Type operator()(const std::tuple<Tys...>& tuple) const {
                return erase_v<I>(tuple) | insert_v<I>(std::forward<Ty>(_data));
            }
        };

        template<class Ty>
        constexpr result<Ty> operator()(Ty&& arg) const {
            return result<Ty>{._data = std::forward<Ty>(arg) };
        }
    };

    template<std::size_t A, std::size_t B> struct sub_v_impl {
        template<class ...Tys, class Type
            = typename info<Tys...>::template sub<A, B>::template as<std::tuple>>
            constexpr auto operator()(const std::tuple<Tys...>& tuple) const -> Type {
            return iterate<generate_indices_v<A, B>>([&]<std::size_t ...Is>{
                return Type{ (get_v<Is>(tuple))... };
            });
        }
    };

    template<class ...Args> struct remove_v_impl {
        template<class ...Tys, auto Indices = info<Tys...>::decay::template indices_except<info<Args...>>,
            class Type = typename keep_indices_t<Indices, info<Tys...>>::template as<std::tuple>>
            constexpr auto operator()(const std::tuple<Tys...>& tuple) const -> Type {
            return iterate<Indices>([&]<std::size_t ...Is>{
                return Type{ (get_v<Is>(tuple))... };
            });
        }
    };

    //template<std::size_t ...Is> struct remove_indices_v_impl {
    //    template<class T,
    //        class Type = typename remove_indices_t<as_array<Is...>, as_info<T>>::template as<std::tuple>>
    //    constexpr auto operator()(T&& tuple) const -> Type {
    //        return iterate<as_array<Is...>>([&]<std::size_t ...Ns>{
    //            return Type{ std::get<Ns>(tuple)... };
    //        });
    //    }
    //};

    ///**
    // * Only keep at Indices.
    // * @tparam R type to remove, or info<Types...> for multiple
    // */
    //template<std::size_t ...Is> constexpr auto remove_indices_v = remove_indices_v_impl<Is...>{};

    template<class ...Args> struct remove_raw_v_impl {
        template<class ...Tys, auto Indices = info<Tys...>::decay::template indices_except<info<Args...>>,
            class Type = typename keep_indices_t<Indices, info<Tys...>>::template as<std::tuple>>
            constexpr auto operator()(const std::tuple<Tys...>& tuple) const -> Type {
            return iterate<Indices>([&]<std::size_t ...Is>{
                return Type{ (get_v<Is>(tuple))... };
            });
        }
    };

    template<class ...Args> struct keep_v_impl {
        template<class ...Tys, auto Indices = info<Tys...>::decay::template indices<info<Args...>>,
            class Type = typename keep_indices_t<Indices, info<Tys...>>::template as<std::tuple>>
            constexpr auto operator()(const std::tuple<Tys...>& tuple) const -> Type {
            return iterate<Indices>([&]<std::size_t ...Is>{
                return Type{ (get_v<Is>(tuple))... };
            });
        }
    };

    template<std::size_t ...Is> struct keep_indices_v_impl {
        template<class ...Tys, class Type = std::tuple<typename info<Tys...>
        ::template element<Is>::type...>>
            constexpr auto operator()(const std::tuple<Tys...>& tuple) const -> Type {
            return Type{ (get_v<Is>(tuple))... };
        }
    };

    template<class ...Args> struct keep_raw_v_impl {
        template<class ...Tys, auto Indices = info<Tys...>::template indices<info<Args...>>,
            class Type = typename keep_indices_t<Indices, info<Tys...>>::template as<std::tuple>>
            constexpr auto operator()(const std::tuple<Tys...>& tuple) const -> Type {
            return iterate<Indices>([&]<std::size_t ...Is>{
                return Type{ (get_v<Is>(tuple))... };
            });
        }
    };

    struct append_v_impl {
        template<class ...Args> struct result {
            std::tuple<Args&&...> _data{};

            template<class ...Tys, class Type = typename info<Tys...>
            ::template append<info<decay_t<Args>...>>::template as<std::tuple>>
                constexpr Type operator()(const std::tuple<Tys...>& tuple) const {
                return[&]<std::size_t ...Is, std::size_t ...Ns>
                    (std::index_sequence<Is...>, std::index_sequence<Ns...>) {
                    return Type{ (get_v<Is>(tuple))..., (get_v<Ns>(_data))... };
                }(std::make_index_sequence<info<Tys...>::size>{},
                    std::index_sequence_for<Args...>{});
            }
        };

        template<class ...Tys>
        constexpr result<Tys...> operator()(Tys&&...args) const {
            return result<Tys...>{._data{ std::forward<Tys>(args)... } };
        }
    };

    struct prepend_v_impl {
        template<class ...Args> struct result {
            std::tuple<Args&&...> _data{};

            template<class ...Tys, class Type = typename info<Tys...>
            ::template prepend<info<decay_t<Args>...>>::template as<std::tuple>>
                constexpr Type operator()(const std::tuple<Tys...>& tuple) const {
                return[&]<std::size_t ...Is, std::size_t ...Ns>
                    (std::index_sequence<Is...>, std::index_sequence<Ns...>) {
                    return Type{ (get_v<Ns>(_data))..., (get_v<Is>(tuple))... };
                }(std::make_index_sequence<info<Tys...>::size>{},
                    std::index_sequence_for<Args...>{});
            }
        };

        template<class ...Tys>
        constexpr result<Tys...> operator()(Tys&&...args) const {
            return result<Tys...>{._data{ std::forward<Tys>(args)... } };
        }
    };

    struct unique_v_impl {
        template<class ...Tys, class Type = typename keep_indices_t<
            first_indices_v<typename info<Tys...>::decay>, info<Tys...>>::template as<std::tuple>>
            constexpr auto operator()(const std::tuple<Tys...>& tuple) const -> Type {
            return iterate<first_indices_v<typename info<Tys...>::decay>>([&]<std::size_t ...Is>{
                return Type{ (get_v<Is>(tuple))... };
            });
        }
    };

    struct reverse_v_impl {
        template<class ...Tys, class Type = typename info<Tys...>::reverse::template as<std::tuple>>
        constexpr auto operator()(const std::tuple<Tys...>& tuple) const -> Type {
            return sequence<0, info<Tys...>::size>([&]<std::size_t ...Is>{
                constexpr std::size_t size = info<Tys...>::size;
                return Type{ (get_v<size - Is - 1>(tuple))... };
            });
        }
    };

    template<auto Filter> struct filter_v_impl {
        template<class ...Tys>
        constexpr auto operator()(const std::tuple<Tys...>& tuple) const {
            return iterate<info<Tys...>::decay::template indices_filter<Filter>>([&]<std::size_t ...Is>{
                return std::tuple{ (get_v<Is>(tuple))... };
            });
        }
    };

    struct call_v_impl {
        template<class Functor> struct result {
            Functor&& _functor;
            template<class ...Tys>
            constexpr decltype(auto) operator()(const std::tuple<Tys...>& tuple) const {
                return sequence<0, info<Tys...>::size>([&]<std::size_t ...Is>() -> decltype(auto) {
                    return std::forward<Functor>(_functor)((get_v<Is>(tuple))...);
                });
            }
        };

        template<class Functor>
        constexpr result<Functor> operator()(Functor&& functor) const {
            return result<Functor>{ std::forward<Functor>(functor) };
        }
    };

    template<std::size_t I> constexpr auto drop_last_v = drop_last_v_impl<I>{};
    template<std::size_t I> constexpr auto last_v = last_v_impl<I>{};
    template<std::size_t I> constexpr auto swap_v = swap_v_impl<I>{};
    template<std::size_t A, std::size_t B> constexpr auto sub_v = sub_v_impl<A, B>{};
    template<class ...Tys> constexpr auto remove_v = remove_v_impl<Tys...>{};
    template<class ...Tys> constexpr auto remove_raw_v = remove_raw_v_impl<Tys...>{};
    template<class ...Tys> constexpr auto keep_v = keep_v_impl<Tys...>{};
    template<class ...Tys> constexpr auto keep_raw_v = keep_raw_v_impl<Tys...>{};
    template<std::size_t ...Is> constexpr auto keep_indices_v = keep_indices_v_impl<Is...>{};
    constexpr auto append_v = append_v_impl{};
    constexpr auto prepend_v = prepend_v_impl{};
    constexpr auto unique_v = unique_v_impl{};
    constexpr auto reverse_v = reverse_v_impl{};
    template<auto Filter> constexpr auto filter_v = filter_v_impl<Filter>{};
    constexpr auto call_v = call_v_impl{};

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

        /**
         * Get the template pack as a tuple.
         */
        template<class Self>
        constexpr std::tuple<Args&&...> as_tuple(this Self&& self) { return std::forward<Self>(self); }
    };

    constexpr auto zip_v = []<class ...Tys>(Tys&&... tuples) {
        constexpr std::size_t min_size = std::min({ as_info<Tys>::size... });
        using zipped = as_info<zip_t<decay_t<Tys>...>>;
        auto _one = [&]<std::size_t I>(value_t<I>)
            -> typename zipped::template type<I> {
            return { (tuples | get_v<I>)... };
        };
        return sequence<min_size>([&]<std::size_t ...Is> {
            return std::tuple{ _one(value_t<Is>{})... };
        });
    };

    template<std::size_t, class> struct concat_v_helper;
    template<std::size_t I, std::size_t ...Is>
    struct concat_v_helper<I, std::index_sequence<Is...>> {
        using type = info<info<value_t<I>, value_t<Is>>...>;
    };

    constexpr auto concat_v = []<class ...Tys>(Tys&&... tuples) -> concat_t<decay_t<Tys>...> {
        template_pack<Tys...> _tuples{ tuples... };
        return sequence<sizeof...(Tys)>([&]<std::size_t ...Is>() -> concat_t<decay_t<Tys>...> {
            using types = info<as_info<Tys>...>;
            using indices = concat_t<typename concat_v_helper<Is,
                std::make_index_sequence<types::template element<Is>::size>>::type...>;
            return indices::for_each([&]<class ...Index>{ return concat_t<decay_t<Tys>...>{
                (_tuples | get_v<Index::template value<0>> | get_v<Index::template value<1>>)...
            }; });
        });
    };

    constexpr auto cartesian_v = []<class ...Tys>(Tys&&... tpls) {
        template_pack<Tys...> _tuples{ tpls... };
        auto eval_at = [&_tuples]<std::size_t I>(value_t<I>) {
            return sequence<sizeof...(Tys)>([&_tuples]<std::size_t ...Is>{
                constexpr auto indices_at_index = [](std::size_t pos) {
                    return sequence<sizeof...(Tys)>([&]<std::size_t ...Ns>() {
                        constexpr std::array sizes{ as_info<Tys>::size... };
                        std::size_t _t_pos = 0;
                        return std::array{ (_t_pos = pos % sizes[Ns], pos /= sizes[Ns], _t_pos)... };
                    });
                };
                constexpr auto _indices = indices_at_index(I);
                using infos = info<Tys...>;
                return std::tuple<typename as_info<typename infos::
                    template element<Is>::type>::template element<_indices[Is]>::type...>{
                    (_tuples | get_v<Is> | get_v<_indices[Is]>)...
                };
            });
        };

        return sequence<(as_info<Tys>::size * ... * 1)>([&]<std::size_t ...Is>{
            return std::tuple{ eval_at(value_t<Is>{})... };
        });
    };

}