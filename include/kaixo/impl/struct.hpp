#pragma once
#include "info.hpp"

/**
 * Helpers for getting information about a struct and structured bindings.
 */
namespace kaixo {
    template<class Ty, class ...Args>
    concept struct_constructible_with = requires (Args...args) {
        { Ty{ args... } };
    };

    /**
     * Find amount of members in a struct.
     * @tparam Ty struct
     */
    template<class Ty>
    struct struct_size {
        /**
         * Finds struct size by trying to construct the struct using
         * a type that's convertible to anything but the struct itself
         * (to prevent copy or move constructor from being called and
         *  making it result in 1 when it should be 0), starting at
         * sizeof(Ty) parameters, and trying 1 less each time until
         * it is constructible.
         */
        constexpr static std::size_t value = reverse_sequence<0, sizeof_v<Ty> + 1>([]<std::size_t ...Ns>{
            using convertible_type = not_convertible_to<Ty>;

            std::size_t res = 0;
            constexpr auto try_one = []<std::size_t ...Is> {
                return struct_constructible_with<Ty, change<value_t<Is>, convertible_type>...>;
            };

            ((sequence<Ns>(try_one) ? (res = Ns, true) : false) || ...);
            return res;
        });
    };

    template<class Ty, std::size_t N>
    struct struct_size<std::array<Ty, N>> {
        constexpr static std::size_t value = N;
    };

    /**
     * Find amount of members in a struct.
     * @tparam Ty struct
     */
    template<aggregate Ty>
    constexpr std::size_t struct_size_v = struct_size<Ty>::value;

    template<class Ty> struct binding_size;
    template<aggregate Ty> 
    struct binding_size<Ty> : std::integral_constant<std::size_t, struct_size_v<Ty>> {};
    template<structured_binding Ty>
    struct binding_size<Ty> : std::integral_constant<std::size_t, std::tuple_size_v<Ty>> {};

    /**
     * Get the size of the structured binding of a type.
     * @tparam Ty type
     */
    template<class Ty>
    constexpr std::size_t binding_size_v = binding_size<Ty>::value;

    /**
     * Find types in structured binding.
     * @tparam Ty type
     * @tparam N number of elements in binding
     */
    template<structured_binding Ty, std::size_t N>
    struct binding_types_impl {
        using type = info<Ty>;
    };

    template<structured_binding Ty>
    struct binding_types_impl<Ty, 0> {
        using type = info<Ty>;
    };

#define KAIXO_STRUCT_MEMBERS_M(c, V, P)            \
    template<structured_binding Ty>                    \
        requires (binding_size_v<Ty> == c)         \
    struct binding_types_impl<Ty, c> {             \
        using type = typename decltype([](Ty& ty) {\
            auto& [V] = ty;                        \
            return info<P>{};                      \
        }(std::declval<Ty&>()));                   \
    };

    template<structured_binding Ty, std::size_t N>
    struct binding_get_member {};

    template<structured_binding Ty>
    struct binding_get_member<Ty, 0> {};

#define KAIXO_STRUCT_GET_MEMBERS_M(c, V, P)                                       \
    template<structured_binding Ty>                                               \
        requires (binding_size_v<Ty> == c)                                        \
    struct binding_get_member<Ty, c> {                                            \
        template<std::size_t I, class Arg>                                        \
            requires (I < c && std::same_as<std::decay_t<Arg>, Ty>                \
                 && !std::is_volatile_v<std::remove_reference_t<Arg>>)            \
        constexpr static decltype(auto) get(Arg&& arg) {                          \
            using type = binding_types<Ty, c>::type::template element<I>;         \
            auto& [V] = arg;                                                      \
            if constexpr (type::is_lvalue_reference::value) {                     \
                return std::get<I>(std::forward_as_tuple(V));                     \
            } else {                                                              \
                return std::move(std::get<I>(std::forward_as_tuple(V)));          \
            }                                                                     \
        }                                                                         \
        template<std::size_t I, class Arg>                                        \
            requires (I < c && std::same_as<std::decay_t<Arg>, Ty>                \
                 && !std::is_volatile_v<std::remove_reference_t<Arg>>)            \
        constexpr static decltype(auto) get(Arg& arg) {                           \
            auto& [V] = arg;                                                      \
            return std::get<I>(std::forward_as_tuple(V));                         \
        }                                                                         \
    };

#define KAIXO_EMPTY
#define KAIXO_COMMA KAIXO_COMMA1
#define KAIXO_COMMA1 ,
#define KAIXO_MERGE(a, b) KAIXO_MERGE1(a, b)
#define KAIXO_MERGE1(a, b) a##b
#define KAIXO_COMMA_MERGE(a, ...) a KAIXO_COMMA __VA_ARGS__
#define KAIXO_LABEL(x) KAIXO_MERGE(val, x)
#define KAIXO_UNIQUE_NAME KAIXO_LABEL(__COUNTER__)
#define KAIXO_MAKE(f, n, V, P) n KAIXO_COMMA V, f(n) KAIXO_COMMA P
#define KAIXO_SIZE(...) (std::tuple_size_v<decltype(std::make_tuple(__VA_ARGS__))>)
#define    KAIXO_UNIQUE(m, c, V, P) m(c, V, P)
#define  KAIXO_UNIQUE_1(m, c, f, n, V, P)  KAIXO_UNIQUE(m, c, V, P)
#define  KAIXO_UNIQUE_2(m, c, f, n, V, P)  KAIXO_UNIQUE_1(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c -  1), f, KAIXO_UNIQUE_NAME, V, P)
#define  KAIXO_UNIQUE_3(m, c, f, n, V, P)  KAIXO_UNIQUE_2(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c -  2), f, KAIXO_UNIQUE_NAME, V, P)
#define  KAIXO_UNIQUE_4(m, c, f, n, V, P)  KAIXO_UNIQUE_3(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c -  3), f, KAIXO_UNIQUE_NAME, V, P)
#define  KAIXO_UNIQUE_5(m, c, f, n, V, P)  KAIXO_UNIQUE_4(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c -  4), f, KAIXO_UNIQUE_NAME, V, P)
#define  KAIXO_UNIQUE_6(m, c, f, n, V, P)  KAIXO_UNIQUE_5(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c -  5), f, KAIXO_UNIQUE_NAME, V, P)
#define  KAIXO_UNIQUE_7(m, c, f, n, V, P)  KAIXO_UNIQUE_6(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c -  6), f, KAIXO_UNIQUE_NAME, V, P)
#define  KAIXO_UNIQUE_8(m, c, f, n, V, P)  KAIXO_UNIQUE_7(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c -  7), f, KAIXO_UNIQUE_NAME, V, P)
#define  KAIXO_UNIQUE_9(m, c, f, n, V, P)  KAIXO_UNIQUE_8(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c -  8), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_10(m, c, f, n, V, P)  KAIXO_UNIQUE_9(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c -  9), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_11(m, c, f, n, V, P) KAIXO_UNIQUE_10(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 10), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_12(m, c, f, n, V, P) KAIXO_UNIQUE_11(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 11), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_13(m, c, f, n, V, P) KAIXO_UNIQUE_12(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 12), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_14(m, c, f, n, V, P) KAIXO_UNIQUE_13(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 13), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_15(m, c, f, n, V, P) KAIXO_UNIQUE_14(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 14), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_16(m, c, f, n, V, P) KAIXO_UNIQUE_15(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 15), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_17(m, c, f, n, V, P) KAIXO_UNIQUE_16(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 16), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_18(m, c, f, n, V, P) KAIXO_UNIQUE_17(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 17), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_19(m, c, f, n, V, P) KAIXO_UNIQUE_18(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 18), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_20(m, c, f, n, V, P) KAIXO_UNIQUE_19(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 19), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_21(m, c, f, n, V, P) KAIXO_UNIQUE_20(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 20), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_22(m, c, f, n, V, P) KAIXO_UNIQUE_21(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 21), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_23(m, c, f, n, V, P) KAIXO_UNIQUE_22(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 22), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_24(m, c, f, n, V, P) KAIXO_UNIQUE_23(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 23), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_25(m, c, f, n, V, P) KAIXO_UNIQUE_24(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 24), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_26(m, c, f, n, V, P) KAIXO_UNIQUE_25(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 25), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_27(m, c, f, n, V, P) KAIXO_UNIQUE_26(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 26), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_28(m, c, f, n, V, P) KAIXO_UNIQUE_27(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 27), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_29(m, c, f, n, V, P) KAIXO_UNIQUE_28(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 28), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_30(m, c, f, n, V, P) KAIXO_UNIQUE_29(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 29), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_31(m, c, f, n, V, P) KAIXO_UNIQUE_30(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 30), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_32(m, c, f, n, V, P) KAIXO_UNIQUE_31(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 31), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_33(m, c, f, n, V, P) KAIXO_UNIQUE_32(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 32), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_34(m, c, f, n, V, P) KAIXO_UNIQUE_33(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 33), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_35(m, c, f, n, V, P) KAIXO_UNIQUE_34(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 34), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_36(m, c, f, n, V, P) KAIXO_UNIQUE_35(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 35), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_37(m, c, f, n, V, P) KAIXO_UNIQUE_36(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 36), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_38(m, c, f, n, V, P) KAIXO_UNIQUE_37(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 37), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_39(m, c, f, n, V, P) KAIXO_UNIQUE_38(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 38), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_40(m, c, f, n, V, P) KAIXO_UNIQUE_39(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 39), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_41(m, c, f, n, V, P) KAIXO_UNIQUE_40(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 40), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_42(m, c, f, n, V, P) KAIXO_UNIQUE_41(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 41), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_43(m, c, f, n, V, P) KAIXO_UNIQUE_42(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 42), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_44(m, c, f, n, V, P) KAIXO_UNIQUE_43(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 43), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_45(m, c, f, n, V, P) KAIXO_UNIQUE_44(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 44), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_46(m, c, f, n, V, P) KAIXO_UNIQUE_45(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 45), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_47(m, c, f, n, V, P) KAIXO_UNIQUE_46(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 46), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_48(m, c, f, n, V, P) KAIXO_UNIQUE_47(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 47), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_49(m, c, f, n, V, P) KAIXO_UNIQUE_48(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 48), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_50(m, c, f, n, V, P) KAIXO_UNIQUE_49(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 49), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_51(m, c, f, n, V, P) KAIXO_UNIQUE_50(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 50), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_52(m, c, f, n, V, P) KAIXO_UNIQUE_51(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 51), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_53(m, c, f, n, V, P) KAIXO_UNIQUE_52(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 52), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_54(m, c, f, n, V, P) KAIXO_UNIQUE_53(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 53), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_55(m, c, f, n, V, P) KAIXO_UNIQUE_54(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 54), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_56(m, c, f, n, V, P) KAIXO_UNIQUE_55(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 55), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_57(m, c, f, n, V, P) KAIXO_UNIQUE_56(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 56), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_58(m, c, f, n, V, P) KAIXO_UNIQUE_57(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 57), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_59(m, c, f, n, V, P) KAIXO_UNIQUE_58(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 58), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_60(m, c, f, n, V, P) KAIXO_UNIQUE_59(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 59), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_61(m, c, f, n, V, P) KAIXO_UNIQUE_60(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 60), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_62(m, c, f, n, V, P) KAIXO_UNIQUE_61(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 61), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_63(m, c, f, n, V, P) KAIXO_UNIQUE_62(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 62), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_64(m, c, f, n, V, P) KAIXO_UNIQUE_63(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 63), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_65(m, c, f, n, V, P) KAIXO_UNIQUE_64(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 64), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_66(m, c, f, n, V, P) KAIXO_UNIQUE_65(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 65), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_67(m, c, f, n, V, P) KAIXO_UNIQUE_66(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 66), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_68(m, c, f, n, V, P) KAIXO_UNIQUE_67(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 67), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_69(m, c, f, n, V, P) KAIXO_UNIQUE_68(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 68), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_70(m, c, f, n, V, P) KAIXO_UNIQUE_69(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 69), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_71(m, c, f, n, V, P) KAIXO_UNIQUE_70(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 70), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_72(m, c, f, n, V, P) KAIXO_UNIQUE_71(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 71), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_73(m, c, f, n, V, P) KAIXO_UNIQUE_72(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 72), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_74(m, c, f, n, V, P) KAIXO_UNIQUE_73(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 73), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_75(m, c, f, n, V, P) KAIXO_UNIQUE_74(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 74), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_76(m, c, f, n, V, P) KAIXO_UNIQUE_75(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 75), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_77(m, c, f, n, V, P) KAIXO_UNIQUE_76(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 76), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_78(m, c, f, n, V, P) KAIXO_UNIQUE_77(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 77), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_79(m, c, f, n, V, P) KAIXO_UNIQUE_78(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 78), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_80(m, c, f, n, V, P) KAIXO_UNIQUE_79(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 79), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_81(m, c, f, n, V, P) KAIXO_UNIQUE_80(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 80), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_82(m, c, f, n, V, P) KAIXO_UNIQUE_81(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 81), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_83(m, c, f, n, V, P) KAIXO_UNIQUE_82(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 82), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_84(m, c, f, n, V, P) KAIXO_UNIQUE_83(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 83), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_85(m, c, f, n, V, P) KAIXO_UNIQUE_84(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 84), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_86(m, c, f, n, V, P) KAIXO_UNIQUE_85(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 85), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_87(m, c, f, n, V, P) KAIXO_UNIQUE_86(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 86), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_88(m, c, f, n, V, P) KAIXO_UNIQUE_87(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 87), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_89(m, c, f, n, V, P) KAIXO_UNIQUE_88(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 88), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_90(m, c, f, n, V, P) KAIXO_UNIQUE_89(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 89), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_91(m, c, f, n, V, P) KAIXO_UNIQUE_90(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 90), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_92(m, c, f, n, V, P) KAIXO_UNIQUE_91(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 91), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_93(m, c, f, n, V, P) KAIXO_UNIQUE_92(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 92), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_94(m, c, f, n, V, P) KAIXO_UNIQUE_93(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 93), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_95(m, c, f, n, V, P) KAIXO_UNIQUE_94(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 94), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_96(m, c, f, n, V, P) KAIXO_UNIQUE_95(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 95), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_97(m, c, f, n, V, P) KAIXO_UNIQUE_96(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 96), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_98(m, c, f, n, V, P) KAIXO_UNIQUE_97(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 97), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_UNIQUE_99(m, c, f, n, V, P) KAIXO_UNIQUE_98(m, c, f, KAIXO_UNIQUE_NAME, KAIXO_COMMA_MERGE(n, V), KAIXO_COMMA_MERGE(f(n), P)) KAIXO_UNIQUE_1(m, (c - 98), f, KAIXO_UNIQUE_NAME, V, P)
#define KAIXO_MAKE_UNIQUE(m, c, f) KAIXO_UNIQUE_##c(m, c, f, KAIXO_UNIQUE_NAME, val, f(val))

#define KAIXO_DECLTYPE(x) decltype(x)
    KAIXO_MAKE_UNIQUE(KAIXO_STRUCT_MEMBERS_M, 99, KAIXO_DECLTYPE);

    /**
     * Find member types of a struct, uses a macro to define
     * overloads up to 99 members using structured bindings.
     */
    template<aggregate Ty>
    struct struct_members : binding_types_impl<Ty, struct_size_v<Ty>> {};

    /**
     * Find the member types of a struct.
     * @tparam Ty struct
     */
    template<aggregate Ty>
    using struct_members_t = typename struct_members<Ty>::type;

    /**
     * Find the types of structured bindings.
     * @tparam Ty type
     */
    template<structured_binding Ty>
    struct binding_types : binding_types_impl<Ty, binding_size_v<Ty>> {};

    /**
     * Get the types of the structured binding elements of Ty.
     * @tparam Ty type
     */
    template<structured_binding Ty>
    using binding_types_t = typename binding_types<Ty>::type;

#define KAIXO_DECLTYPE2(x) decltype(x)
    KAIXO_MAKE_UNIQUE(KAIXO_STRUCT_GET_MEMBERS_M, 99, KAIXO_DECLTYPE2);
}

namespace std {
    template<std::size_t I, class Ty>
        requires (kaixo::structured_binding<decay_t<Ty>> && !is_volatile_v<remove_reference_t<Ty>>)
    constexpr decltype(auto) get(Ty&& value) {
        using type = decay_t<Ty>;
        return kaixo::binding_get_member<type, kaixo::binding_size_v<type>>::get<I>(std::forward<Ty>(value));
    }
}