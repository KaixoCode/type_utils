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

        constexpr auto decayed_tuple = type_filter<decayed_tuple_impl>{};
    }

    namespace grab {
        template<class Ty> struct decayed_tuple_impl { using type = Ty; };
        template<has_decayed_tuple Ty> struct decayed_tuple_impl<Ty> { using type = typename Ty::decayed_tuple; };

        template<class Ty> using decayed_tuple = decayed_tuple_impl<Ty>::type;
    }

    namespace tuples {
        /**
         * Copies qualifiers from type if not a reference.
         * @tparam Ty type to copy from
         */
        template<class Ty>
        struct _tpl_ref {
            template<class T>
            using type = std::conditional_t<!reference<T>, add_cvref_t<T, Ty>, T>;
        };

        template<class Self>
        struct view_interface {
            
            constexpr auto as_tuple() {
                using decayed_tuple = typename Self::types
                    ::template when<is_rvalue_reference>
                        ::template transform<remove_reference_t>
                    ::template transform<grab::decayed_tuple>
                    ::template as<std::tuple>;

                return sequence<Self::types::size>([&]<std::size_t ...Is>{
                    auto& self = *static_cast<Self*>(this);
                    return decayed_tuple{ self.template get<Is>()... };
                });
            }
        };

        template<class Ty>
        concept view = std::derived_from<decay_t<Ty>, view_interface<decay_t<Ty>>>;

        template<class Ty, class T>
        concept pipe = invocable<decay_t<Ty>, T> && requires() {
            typename decay_t<Ty>::tuple_pipe;
        };

        template<class Fn, class ...Captures>
        struct _tuple_closure {
            using tuple_pipe = int;

            template<class ...Args>
            constexpr _tuple_closure(Args&& ...args)
                : captures(std::forward<Args>(args)...) {}

            template<class Self, class Ty>
                requires invocable<Fn, Ty, decltype(std::forward_like<Self>(std::declval<Captures>()))...>
            constexpr decltype(auto) operator()(this Self&& self, Ty&& arg) {
                return sequence<sizeof...(Captures)>([&]<std::size_t ...Is>{
                    return Fn{}(std::forward<Ty>(arg), std::get<Is>(std::forward<Self>(self).captures)...);
                });
            }

            std::tuple<Captures...> captures;
        };
    }
}

namespace std {
    template<class ...Args>
    struct tuple_size<kaixo::template_pack<Args...>> : std::integral_constant<std::size_t, sizeof...(Args)> {};
    template<size_t I, class ...Args>
    struct tuple_element<I, kaixo::template_pack<Args...>> : std::tuple_element<I, std::tuple<Args&&...>> {};

    template<kaixo::tuples::view Ty>
    struct tuple_size<Ty> : std::integral_constant<std::size_t, Ty::types::size> {};
    template<size_t I, kaixo::tuples::view Ty>
    struct tuple_element<I, Ty> : Ty::types::template element<I> {};

    template<size_t I, kaixo::tuples::view Ty>
    constexpr decltype(auto) get(Ty&& val) {
        return std::forward<Ty>(val).template get<I>();
    }
}

/**
 * Tuple manipulation helpers.
 */
namespace kaixo::tuples {

    template<class T, pipe<T> Ty>
        requires (structured_binding<decay_t<T>> || view<T>)
    constexpr decltype(auto) operator|(T&& tuple, Ty&& val) {
        return std::forward<Ty>(val)(std::forward<T>(tuple));
    }

    // =======================================================

    template<std::size_t I> 
    struct _get_fun {
        using tuple_pipe = int;

        template<class Ty> 
            requires (structured_binding<decay_t<Ty>> || view<Ty>)
        constexpr decltype(auto) operator()(Ty&& tuple) const {
            if constexpr (view<Ty>) {
                using type = decay_t<Ty>::types                    
                    ::template element<I>
                    ::template transform<typename _tpl_ref<Ty&&>::type>
                    ::type;
                return std::forward<Ty>(tuple).template get<I>();
            } else {
                using type = binding_types_t<decay_t<Ty>>
                    ::template element<I>
                    ::template transform<typename _tpl_ref<Ty&&>::type>
                    ::type;

                return static_cast<type>(kaixo::get_binding_element<I>(std::forward<Ty>(tuple)));
            }
        }
    };

    template<std::size_t I> 
    constexpr auto get = _get_fun<I>{};

    // =======================================================

    struct empty_view : view_interface<empty_view> {
        using types = info<>;
    };

    // =======================================================

    template<class Ty>
        requires structured_binding<decay_t<Ty>>
    struct owning_view : view_interface<owning_view<Ty>> {
        using types = binding_types_t<decay_t<Ty>>;

        decay_t<Ty> value;

        constexpr owning_view(Ty&& tpl) 
            : value(std::move(tpl)) {}

        template<std::size_t N, class Self>
        constexpr auto&& get(this Self&& self) {
            return tuples::get<N>(std::forward<Self>(self).value);
        }
    };

    // =======================================================

    template<class Ty>
        requires structured_binding<remove_const_t<Ty>>
    struct ref_view : view_interface<ref_view<Ty>> {
        using types = binding_types_t<decay_t<Ty>>;

        Ty* value;

        template<convertible_to<Ty&> T>
        constexpr ref_view(T&& tpl)
            : value(std::addressof(static_cast<Ty&>(std::forward<T>(tpl)))) {}

        template<std::size_t N, class Self>
        constexpr auto&& get(this Self&& self) {
            return tuples::get<N>(*std::forward<Self>(self).value);
        }
    };

    template <class Ty>
    ref_view(Ty&) -> ref_view<Ty>;

    // =======================================================

    struct _all_fun {
        using tuple_pipe = int;

        template<class Tpl>
            requires (structured_binding<decay_t<Tpl>> || view<Tpl>)
        constexpr auto operator()(Tpl&& val) const {
            if constexpr (view<Tpl>) {
                return std::forward<Tpl>(val);
            } else if constexpr (lvalue_reference<Tpl>) {
                return ref_view{ std::forward<Tpl>(val) };
            } else {
                return owning_view{ std::forward<Tpl>(val) };
            }
        }
    };

    constexpr _all_fun all{};

    template<class Ty>
    using all_t = decltype(all(std::declval<Ty>()));

    // =======================================================

    template<std::size_t I, view Tpl>
        requires (I <= decay_t<Tpl>::types::size)
    struct take_view : view_interface<take_view<I, Tpl>> {
        using types = decay_t<Tpl>::types::template take<I>;

        Tpl tpl;

        template<class T> requires constructible<Tpl, T&&>
        constexpr take_view(T&& v) : tpl(std::forward<T>(v)) {}

        template<std::size_t N, class Self>
            requires (N < types::size)
        constexpr decltype(auto) get(this Self&& self) {
            return tuples::get<N>(std::forward<Self>(self).tpl);
        }
    };

    template<std::size_t I>
    struct _take_fun {
        using tuple_pipe = int;
        
        template<class Tpl>
            requires (structured_binding<decay_t<Tpl>> || view<Tpl>)
        constexpr auto operator()(Tpl&& val) const {
            if constexpr (I == 0) {
                return empty_view{};
            } else {
                return take_view<I, all_t<Tpl>>{ std::forward<Tpl>(val) };
            }
        }
    };

    template<std::size_t I>
    constexpr auto take = _take_fun<I>{};
    constexpr auto head = take<1>;

    // =======================================================

    template<std::size_t I, view Tpl>
        requires (I <= decay_t<Tpl>::types::size)
    struct drop_view : view_interface<drop_view<I, Tpl>> {
        using types = decay_t<Tpl>::types::template drop<I>;

        Tpl tpl;

        template<class T> requires constructible<Tpl, T&&>
        constexpr drop_view(T&& v) : tpl(std::forward<T>(v)) {}

        template<std::size_t N, class Self>
            requires (N < types::size)
        constexpr decltype(auto) get(this Self&& self) {
            return tuples::get<N + I>(std::forward<Self>(self).tpl);
        }
    };

    template<std::size_t I>
    struct _drop_fun {
        using tuple_pipe = int;

        template<class Tpl>
            requires (structured_binding<decay_t<Tpl>> || view<Tpl>)
        constexpr auto operator()(Tpl&& val) const {
            if constexpr (I >= all_t<Tpl>::types::size) {
                return empty_view{};
            } else {
                return drop_view<I, all_t<Tpl>>{ std::forward<Tpl>(val) };
            }
        }
    };

    template<std::size_t I>
    constexpr auto drop = _drop_fun<I>{};
    constexpr auto tail = drop<1>;

    // =======================================================

    template<std::size_t I, view Tpl>
        requires (I <= decay_t<Tpl>::types::size)
    struct last_view : view_interface<last_view<I, Tpl>> {
        using types = decay_t<Tpl>::types::template last<I>;

        Tpl tpl;

        template<class T> requires constructible<Tpl, T&&>
        constexpr last_view(T&& v) : tpl(std::forward<T>(v)) {}

        template<std::size_t N, class Self>
            requires (N < types::size)
        constexpr decltype(auto) get(this Self&& self) {
            return tuples::get<N + decay_t<Tpl>::types::size - I>(std::forward<Self>(self).tpl);
        }
    };

    template<std::size_t I>
    struct _last_fun {
        using tuple_pipe = int;

        template<class Tpl>
            requires (structured_binding<decay_t<Tpl>> || view<Tpl>)
        constexpr auto operator()(Tpl&& val) const {
            if constexpr (I == 0) {
                return empty_view{};
            } else {
                return last_view<I, all_t<Tpl>>{ std::forward<Tpl>(val) };
            }
        }
    };

    template<std::size_t I>
    constexpr auto last = _last_fun<I>{};
    
    // =======================================================

    template<std::size_t I, view Tpl>
        requires (I <= decay_t<Tpl>::types::size)
    struct drop_last_view : view_interface<drop_last_view<I, Tpl>> {
        using types = decay_t<Tpl>::types::template drop_last<I>;

        Tpl tpl;

        template<class T> requires constructible<Tpl, T&&>
        constexpr drop_last_view(T&& v) : tpl(std::forward<T>(v)) {}

        template<std::size_t N, class Self>
            requires (N < types::size)
        constexpr decltype(auto) get(this Self&& self) {
            return tuples::get<N>(std::forward<Self>(self).tpl);
        }
    };

    template<std::size_t I>
    struct _drop_last_fun {
        using tuple_pipe = int;

        template<class Tpl>
            requires (structured_binding<decay_t<Tpl>> || view<Tpl>)
        constexpr auto operator()(Tpl&& val) const {
            if constexpr (I >= all_t<Tpl>::types::size) {
                return empty_view{};
            } else {
                return drop_last_view<I, all_t<Tpl>>{ std::forward<Tpl>(val) };
            }
        }
    };

    template<std::size_t I>
    constexpr auto drop_last = _drop_last_fun<I>{};
    constexpr auto init = drop_last<1>;
    
    // =======================================================

    template<view Tpl, std::size_t ...Is>
        requires ((Is < all_t<Tpl>::types::size) && ...)
    struct erase_view : view_interface<erase_view<Tpl, Is...>> {
        using types = decay_t<Tpl>::types::template remove_indices<as_array<Is...>>;

        Tpl tpl;

        template<class T> requires constructible<Tpl, T&&>
        constexpr erase_view(T&& v) : tpl(std::forward<T>(v)) {}

        template<std::size_t N, class Self>
            requires (N < types::size)
        constexpr decltype(auto) get(this Self&& self) {
            constexpr auto Indices = generate_indices_v<0, decay_t<Tpl>::types::size, Is...>;
            return tuples::get<Indices[N]>(std::forward<Self>(self).tpl);
        }
    };

    template<std::size_t ...Is>
    struct _erase_fun {
        using tuple_pipe = int;

        template<class Tpl>
            requires ((structured_binding<decay_t<Tpl>> || view<Tpl>)
                && ((Is < all_t<Tpl>::types::size) && ...))
        constexpr auto operator()(Tpl&& val) const {
            if constexpr (all_t<Tpl>::types
                ::template remove_indices<as_array<Is...>>::size == 0) {
                return empty_view{};
            } else {
                return erase_view<all_t<Tpl>, Is...>{ std::forward<Tpl>(val) };
            }
        }
    };

    template<std::size_t ...Is>
    constexpr auto erase = _erase_fun<Is...>{};
    
    // =======================================================

    template<std::size_t I, view Tpl, class ...Args>
        requires (I <= all_t<Tpl>::types::size)
    struct insert_view : view_interface<insert_view<I, Tpl, Args...>> {
        using types = decay_t<Tpl>::types::template insert<I, info<Args...>>;

        std::tuple<Args...> args;
        Tpl tpl;

        template<class T, class A> requires constructible<Tpl, T&&>
        constexpr insert_view(T&& v, A&& args) 
            : args(std::forward<A>(args)), tpl(std::forward<T>(v)) {}

        template<std::size_t N, class Self>
            requires (N < types::size)
        constexpr decltype(auto) get(this Self&& self) {
            if constexpr (N < I) {
                return tuples::get<N>(std::forward<Self>(self).tpl);
            } else if constexpr (N >= I && N < I + sizeof...(Args)) {
                return tuples::get<N - I>(std::forward<Self>(self).args);
            } else {
                return tuples::get<N - sizeof...(Args)>(std::forward<Self>(self).tpl);
            }
        }
    };

    template<std::size_t I>
    struct _insert_fun {
        using tuple_pipe = int;

        template<class Tpl, class ...Args>
            requires ((structured_binding<decay_t<Tpl>> || view<Tpl>) 
                && sizeof...(Args) != 0 && I <= all_t<Tpl>::types::size)
        constexpr auto operator()(Tpl&& val, Args&& ...args) const {
            return insert_view<I, all_t<Tpl>, Args...>{
                std::forward<Tpl>(val), std::forward_as_tuple(std::forward<Args>(args)...)
            };
        }

        template<class ...Args>
        constexpr auto operator()(Args&& ...args) const {
            return _tuple_closure<_insert_fun, Args...>(std::forward<Args>(args)...);
        }
    };

    template<std::size_t I>
    constexpr auto insert = _insert_fun<I>{};
    
    // =======================================================

    template<std::size_t I, view Tpl, class Arg>
        requires (I < all_t<Tpl>::types::size)
    struct swap_view : view_interface<swap_view<I, Tpl, Arg>> {
        using types = decay_t<Tpl>::types::template swap<I, Arg>;

        Arg arg;
        Tpl tpl;

        template<class T, class A> requires constructible<Tpl, T&&>
        constexpr swap_view(T&& v, A&& arg)
            : arg(std::forward<A>(arg)), tpl(std::forward<T>(v)) {}

        template<std::size_t N, class Self>
            requires (N < types::size)
        constexpr decltype(auto) get(this Self&& self) {
            if constexpr (N < I) {
                return tuples::get<N>(std::forward<Self>(self).tpl);
            } else if constexpr (N == I) {
                return std::forward_like<Self>(std::forward<Self>(self).arg);
            } else {
                return tuples::get<N>(std::forward<Self>(self).tpl);
            }
        }
    };

    template<std::size_t I>
    struct _swap_fun {
        using tuple_pipe = int;

        template<class Tpl, class Arg>
            requires ((structured_binding<decay_t<Tpl>> || view<Tpl>)
                && I < all_t<Tpl>::types::size)
        constexpr auto operator()(Tpl&& val, Arg&& arg) const {
            return swap_view<I, all_t<Tpl>, Arg>{
                std::forward<Tpl>(val), std::forward<Arg>(arg)
            };
        }

        template<class Arg>
        constexpr auto operator()(Arg&& arg) const {
            return _tuple_closure<_swap_fun, Arg>(std::forward<Arg>(arg));
        }
    };

    template<std::size_t I>
    constexpr auto swap = _swap_fun<I>{};
    
    // =======================================================

    template<std::size_t A, std::size_t B, view Tpl>
        requires (A <= decay_t<Tpl>::types::size && B <= decay_t<Tpl>::types::size && A < B)
    struct sub_view : view_interface<sub_view<A, B, Tpl>> {
        using types = decay_t<Tpl>::types::template sub<A, B>;

        Tpl tpl;

        template<class T> requires constructible<Tpl, T&&>
        constexpr sub_view(T&& v) : tpl(std::forward<T>(v)) {}

        template<std::size_t N, class Self>
            requires (N < types::size)
        constexpr decltype(auto) get(this Self&& self) {
            return tuples::get<N + A>(std::forward<Self>(self).tpl);
        }
    };

    template<std::size_t A, std::size_t B>
    struct _sub_fun {
        using tuple_pipe = int;

        template<class Tpl>
            requires ((structured_binding<decay_t<Tpl>> || view<Tpl>) 
                && A <= decay_t<Tpl>::types::size && B <= decay_t<Tpl>::types::size && A < B)
        constexpr auto operator()(Tpl&& val) const {
            if constexpr (A == B) {
                return empty_view{};
            } else {
                return sub_view<A, B, all_t<Tpl>>{ std::forward<Tpl>(val) };
            }
        }
    };

    template<std::size_t A, std::size_t B>
    constexpr auto sub = _sub_fun<A, B>{};
    
    // =======================================================

    template<view Tpl, class ...Args>
    struct remove_view : view_interface<remove_view<Tpl, Args...>> {
        using types = decay_t<Tpl>::types::template remove<typename info<Args...>::decay>;

        Tpl tpl;

        template<class T> requires constructible<Tpl, T&&>
        constexpr remove_view(T&& v) : tpl(std::forward<T>(v)) {}

        template<std::size_t N, class Self>
            requires (N < types::size)
        constexpr decltype(auto) get(this Self&& self) {
            constexpr auto Indices = decay_t<Tpl>::types::decay
                ::template indices_except<typename info<Args...>::decay>;
            return tuples::get<Indices[N]>(std::forward<Self>(self).tpl);
        }
    };

    template<class ...Args>
    struct _remove_fun {
        using tuple_pipe = int;

        template<class Tpl>
            requires (structured_binding<decay_t<Tpl>> || view<Tpl>)
        constexpr auto operator()(Tpl&& val) const {
            if constexpr (all_t<Tpl>::types::decay::template remove<typename info<Args...>::decay>::size == 0) {
                return empty_view{};
            } else {
                return remove_view<all_t<Tpl>, Args...>{ std::forward<Tpl>(val) };
            }
        }
    };

    template<class ...Args>
    constexpr auto remove = _remove_fun<Args...>{};
    
    // =======================================================

    template<view Tpl, class ...Args>
    struct remove_raw_view : view_interface<remove_raw_view<Tpl, Args...>> {
        using types = decay_t<Tpl>::types::template remove<info<Args...>>;

        Tpl tpl;

        template<class T> requires constructible<Tpl, T&&>
        constexpr remove_raw_view(T&& v) : tpl(std::forward<T>(v)) {}

        template<std::size_t N, class Self>
            requires (N < types::size)
        constexpr decltype(auto) get(this Self&& self) {
            constexpr auto Indices = decay_t<Tpl>::types
                ::template indices_except<info<Args...>>;
            return tuples::get<Indices[N]>(std::forward<Self>(self).tpl);
        }
    };

    template<class ...Args>
    struct _remove_raw_fun {
        using tuple_pipe = int;

        template<class Tpl>
            requires (structured_binding<decay_t<Tpl>> || view<Tpl>)
        constexpr auto operator()(Tpl&& val) const {
            if constexpr (all_t<Tpl>::types::template remove<info<Args...>>::size == 0) {
                return empty_view{};
            } else {
                return remove_raw_view<all_t<Tpl>, Args...>{ std::forward<Tpl>(val) };
            }
        }
    };

    template<class ...Args>
    constexpr auto remove_raw = _remove_raw_fun<Args...>{};
    
    // =======================================================

    template<view Tpl, class ...Args>
    struct keep_view : view_interface<keep_view<Tpl, Args...>> {
        using types = decay_t<Tpl>::types::template keep<typename info<Args...>::decay>;

        Tpl tpl;

        template<class T> requires constructible<Tpl, T&&>
        constexpr keep_view(T&& v) : tpl(std::forward<T>(v)) {}

        template<std::size_t N, class Self>
            requires (N < types::size)
        constexpr decltype(auto) get(this Self&& self) {
            constexpr auto Indices = decay_t<Tpl>::types::decay
                ::template indices<typename info<Args...>::decay>;
            return tuples::get<Indices[N]>(std::forward<Self>(self).tpl);
        }
    };

    template<class ...Args>
    struct _keep_fun {
        using tuple_pipe = int;

        template<class Tpl>
            requires (structured_binding<decay_t<Tpl>> || view<Tpl>)
        constexpr auto operator()(Tpl&& val) const {
            if constexpr (all_t<Tpl>::types::decay::template keep<typename info<Args...>::decay>::size == 0) {
                return empty_view{};
            } else {
                return keep_view<all_t<Tpl>, Args...>{ std::forward<Tpl>(val) };
            }
        }
    };

    template<class ...Args>
    constexpr auto keep = _keep_fun<Args...>{};
    
    // =======================================================

    template<view Tpl, class ...Args>
    struct keep_raw_view : view_interface<keep_raw_view<Tpl, Args...>> {
        using types = decay_t<Tpl>::types::template keep<info<Args...>>;

        Tpl tpl;

        template<class T> requires constructible<Tpl, T&&>
        constexpr keep_raw_view(T&& v) : tpl(std::forward<T>(v)) {}

        template<std::size_t N, class Self>
            requires (N < types::size)
        constexpr decltype(auto) get(this Self&& self) {
            constexpr auto Indices = decay_t<Tpl>::types
                ::template indices<info<Args...>>;
            return tuples::get<Indices[N]>(std::forward<Self>(self).tpl);
        }
    };

    template<class ...Args>
    struct _keep_raw_fun {
        using tuple_pipe = int;

        template<class Tpl>
            requires (structured_binding<decay_t<Tpl>> || view<Tpl>)
        constexpr auto operator()(Tpl&& val) const {
            if constexpr (all_t<Tpl>::types::template keep<info<Args...>>::size == 0) {
                return empty_view{};
            } else {
                return keep_raw_view<all_t<Tpl>, Args...>{ std::forward<Tpl>(val) };
            }
        }
    };

    template<class ...Args>
    constexpr auto keep_raw = _keep_raw_fun<Args...>{};
    
    // =======================================================

    template<view Tpl, std::size_t ...Is>
        requires ((Is < all_t<Tpl>::types::size) && ...)
    struct keep_indices_view : view_interface<keep_indices_view<Tpl, Is...>> {
        using types = decay_t<Tpl>::types::template keep_indices<as_array<Is...>>;

        Tpl tpl;

        template<class T> requires constructible<Tpl, T&&>
        constexpr keep_indices_view(T&& v) : tpl(std::forward<T>(v)) {}

        template<std::size_t N, class Self>
            requires (N < types::size)
        constexpr decltype(auto) get(this Self&& self) {
            return tuples::get<as_array<Is...>[N]>(std::forward<Self>(self).tpl);
        }
    };

    template<std::size_t ...Is>
    struct _keep_indices_fun {
        using tuple_pipe = int;

        template<class Tpl>
            requires ((structured_binding<decay_t<Tpl>> || view<Tpl>)
                && ((Is < all_t<Tpl>::types::size) && ...))
        constexpr auto operator()(Tpl&& val) const {
            if constexpr (sizeof...(Is) == 0) {
                return empty_view{};
            } else {
                return keep_indices_view<all_t<Tpl>, Is...>{ std::forward<Tpl>(val) };
            }
        }
    };

    template<std::size_t ...Is>
    constexpr auto keep_indices = _keep_indices_fun<Is...>{};
    
    // =======================================================

    template<view Tpl, class ...Args>
    struct append_view : view_interface<append_view<Tpl, Args...>> {
        using types = decay_t<Tpl>::types::template append<info<Args...>>;

        std::tuple<Args...> args;
        Tpl tpl;

        template<class T, class A> requires constructible<Tpl, T&&>
        constexpr append_view(T&& v, A&& args)
            : args(std::forward<A>(args)), tpl(std::forward<T>(v)) {}

        template<std::size_t N, class Self>
            requires (N < types::size)
        constexpr decltype(auto) get(this Self&& self) {
            if constexpr (N < decay_t<Tpl>::types::size) {
                return tuples::get<N>(std::forward<Self>(self).tpl);
            } else {
                return tuples::get<N - decay_t<Tpl>::types::size>(std::forward<Self>(self).args);
            }
        }
    };

    struct _append_fun {
        using tuple_pipe = int;

        template<class Tpl, class ...Args>
            requires ((structured_binding<decay_t<Tpl>> || view<Tpl>) && sizeof...(Args) != 0)
        constexpr auto operator()(Tpl&& val, Args&& ...args) const {
            return append_view<all_t<Tpl>, Args...>{
                std::forward<Tpl>(val), std::forward_as_tuple(std::forward<Args>(args)...)
            };
        }

        template<class ...Args>
        constexpr auto operator()(Args&& ...args) const {
            return _tuple_closure<_append_fun, Args...>(std::forward<Args>(args)...);
        }
    };

    constexpr auto append = _append_fun{};
    
    // =======================================================

    template<view Tpl, class ...Args>
    struct prepend_view : view_interface<prepend_view<Tpl, Args...>> {
        using types = decay_t<Tpl>::types::template prepend<info<Args...>>;

        std::tuple<Args...> args;
        Tpl tpl;

        template<class T, class A> requires constructible<Tpl, T&&>
        constexpr prepend_view(T&& v, A&& args)
            : args(std::forward<A>(args)), tpl(std::forward<T>(v)) {}

        template<std::size_t N, class Self>
            requires (N < types::size)
        constexpr decltype(auto) get(this Self&& self) {
            if constexpr (N < sizeof...(Args)) {
                return tuples::get<N>(std::forward<Self>(self).args);
            } else {
                return tuples::get<N - sizeof...(Args)>(std::forward<Self>(self).tpl);
            }
        }
    };

    struct _prepend_fun {
        using tuple_pipe = int;

        template<class Tpl, class ...Args>
            requires ((structured_binding<decay_t<Tpl>> || view<Tpl>) && sizeof...(Args) != 0)
        constexpr auto operator()(Tpl&& val, Args&& ...args) const {
            return prepend_view<all_t<Tpl>, Args...>{
                std::forward<Tpl>(val), std::forward_as_tuple(std::forward<Args>(args)...)
            };
        }

        template<class ...Args>
        constexpr auto operator()(Args&& ...args) const {
            return _tuple_closure<_prepend_fun, Args...>(std::forward<Args>(args)...);
        }
    };

    constexpr auto prepend = _prepend_fun{};
    
    // =======================================================

    template<view Tpl>
    struct unique_view : view_interface<unique_view<Tpl>> {
        using types = decay_t<Tpl>::types::unique;

        Tpl tpl;

        template<class T> requires constructible<Tpl, T&&>
        constexpr unique_view(T&& v) : tpl(std::forward<T>(v)) {}

        template<std::size_t N, class Self>
            requires (N < types::size)
        constexpr decltype(auto) get(this Self&& self) {
            constexpr auto Indices = first_indices_v<typename decay_t<Tpl>::types>;
            return tuples::get<Indices[N]>(std::forward<Self>(self).tpl);
        }
    };

    struct _unique_fun {
        using tuple_pipe = int;

        template<class Tpl>
            requires (structured_binding<decay_t<Tpl>> || view<Tpl>)
        constexpr auto operator()(Tpl&& val) const {
            return unique_view<all_t<Tpl>>{ std::forward<Tpl>(val) };
        }
    };

    constexpr auto unique = _unique_fun{};
    
    // =======================================================

    template<view Tpl>
    struct reverse_view : view_interface<reverse_view<Tpl>> {
        using types = decay_t<Tpl>::types::reverse;

        Tpl tpl;

        template<class T> requires constructible<Tpl, T&&>
        constexpr reverse_view(T&& v) : tpl(std::forward<T>(v)) {}

        template<std::size_t N, class Self>
            requires (N < types::size)
        constexpr decltype(auto) get(this Self&& self) {
            return tuples::get<types::size - N - 1>(std::forward<Self>(self).tpl);
        }
    };

    struct _reverse_fun {
        using tuple_pipe = int;

        template<class Tpl>
            requires (structured_binding<decay_t<Tpl>> || view<Tpl>)
        constexpr auto operator()(Tpl&& val) const {
            return reverse_view<all_t<Tpl>>{ std::forward<Tpl>(val) };
        }
    };

    constexpr auto reverse = _reverse_fun{};
    
    // =======================================================

    template<auto Filter, view Tpl>
    struct filter_view : view_interface<filter_view<Filter, Tpl>> {
        using types = decay_t<Tpl>::types::template filter<Filter>;

        Tpl tpl;

        template<class T> requires constructible<Tpl, T&&>
        constexpr filter_view(T&& v) : tpl(std::forward<T>(v)) {}

        template<std::size_t N, class Self>
            requires (N < types::size)
        constexpr decltype(auto) get(this Self&& self) {
            return tuples::get<types::size - N - 1>(std::forward<Self>(self).tpl);
        }
    };

    template<auto Filter>
    struct _filter_fun {
        using tuple_pipe = int;

        template<class Tpl>
            requires (structured_binding<decay_t<Tpl>> || view<Tpl>)
        constexpr auto operator()(Tpl&& val) const {
            if constexpr (all_t<Tpl>::types::template filter<Filter>::size == 0) {
                return empty_view{};
            } else {
                return filter_view<Filter, all_t<Tpl>>{ std::forward<Tpl>(val) };
            }
        }
    };

    template<auto Filter>
    constexpr auto filter = _filter_fun<Filter>{};
    
    // =======================================================

    template<view Tpl, class Arg>
    struct transform_view : view_interface<transform_view<Tpl, Arg>> {
        using types = decay_t<Tpl>::types
            ::template transform<typename partial<std::invoke_result_t, Arg>::type>;
    
        Arg arg;
        Tpl tpl;
    
        template<class T, class A> requires constructible<Tpl, T&&>
        constexpr transform_view(T&& v, A&& arg)
            : arg(std::forward<A>(arg)), tpl(std::forward<T>(v)) {}
    
        template<std::size_t N, class Self>
            requires (N < types::size)
        constexpr decltype(auto) get(this Self&& self) {
            return std::forward<Self>(self).arg(tuples::get<N>(std::forward<Self>(self).tpl));
        }
    };
    
    struct _transform_fun {
        using tuple_pipe = int;
    
        template<class Tpl, class Arg>
            requires ((structured_binding<decay_t<Tpl>> || view<Tpl>) 
                && all_t<Tpl>::types::template filter<!can_invoke<Arg>>::size == 0)
        constexpr auto operator()(Tpl&& val, Arg&& arg) const {
            return transform_view<all_t<Tpl>, Arg>{
                std::forward<Tpl>(val), std::forward<Arg>(arg)
            };
        }
    
        template<class Arg>
        constexpr auto operator()(Arg&& arg) const {
            return _tuple_closure<_transform_fun, Arg>(std::forward<Arg>(arg));
        }
    };
    
    constexpr auto transform = _transform_fun{};
    
    template<std::size_t I>
    constexpr auto elements = transform([]<class Ty>(Ty&& arg) -> decltype(auto) { return get<I>(std::forward<Ty>(arg)); });
    
    constexpr auto keys = elements<0>;
    constexpr auto values = elements<1>;

    // =======================================================

    struct _call_fun {
        template<class Functor, class Ty>
            requires (structured_binding<decay_t<Ty>> || view<Ty>)
        constexpr decltype(auto) operator()(Ty&& tuple, Functor&& functor) const {
            using types = all_t<Ty>::types;
            return sequence<0, types::size>([&]<std::size_t ...Is>() -> decltype(auto) {
                return std::forward<Functor>(functor)(get<Is>(std::forward<Ty>(tuple))...);
            });
        }
    };

    constexpr _call_fun call{};

    // =======================================================

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
                ::template as<std::tuple>;

            return tuple_type(get<I>(std::forward<Tys>(tuples))...);
        };

        using tuple_type = zipped
            ::template transform<typename move_tparams<std::tuple>::type>
            ::template as<std::tuple>;

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
            ::template as<std::tuple>;
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
                    ::template as<std::tuple>;

                return tuple_type(get<_indices[Is]>(get<Is>(_tuples))...);
            });
        };

        using tuple_type = cartesian_type
            ::template transform<typename move_tparams<std::tuple>::type>
            ::template as<std::tuple>;

        return sequence<(binding_size_v<decay_t<Tys>> * ... * 1)>([&]<std::size_t ...Is>{
            return tuple_type(eval_at(value_t<Is>{})...);
        });
    };
}