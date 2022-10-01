#pragma once
#include "info.hpp"

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
        constexpr static std::size_t value = reverse_sequence<0, sizeof_v<Ty> +1>([]<std::size_t ...Ns>{
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

    /**
     * Find member types of a struct, uses a macro to define
     * overloads up to 99 members using structured bindings.
     */
    template<aggregate Ty, std::size_t N>
    struct struct_members {
        using type = info<Ty>;
    };

    template<aggregate Ty>
    struct struct_members<Ty, 0> {
        using type = info<Ty>;
    };

#define KAIXO_STRUCT_MEMBERS_M(c, ...)                                         \
        template<aggregate Ty>                                                 \
            requires (struct_size_v<Ty> == c)                                  \
        struct struct_members<Ty, c> {                                         \
            using type = typename decltype([](Ty& ty) {                        \
                auto& [__VA_ARGS__] = ty;                                      \
                using tuple_t = decltype(std::tuple{ __VA_ARGS__ });           \
                return move_tparams_t<tuple_t, info>{};                        \
            }(std::declval<Ty&>()));                                           \
        };

#define KAIXO_MERGE(a, b) KAIXO_MERGE1(a, b)
#define KAIXO_MERGE1(a, b) a##b
#define KAIXO_LABEL(x) KAIXO_MERGE(val, x)
#define KAIXO_UNIQUE_NAME KAIXO_LABEL(__COUNTER__)
#define KAIXO_SIZE(...) (std::tuple_size_v<decltype(std::make_tuple(__VA_ARGS__))>)
#define    KAIXO_UNIQUE(m, c, ...) m(c, __VA_ARGS__)
#define  KAIXO_UNIQUE_1(m, c, ...)    KAIXO_UNIQUE(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__)
#define  KAIXO_UNIQUE_2(m, c, ...)  KAIXO_UNIQUE_1(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c -  1), __VA_ARGS__)
#define  KAIXO_UNIQUE_3(m, c, ...)  KAIXO_UNIQUE_2(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c -  2), __VA_ARGS__)
#define  KAIXO_UNIQUE_4(m, c, ...)  KAIXO_UNIQUE_3(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c -  3), __VA_ARGS__)
#define  KAIXO_UNIQUE_5(m, c, ...)  KAIXO_UNIQUE_4(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c -  4), __VA_ARGS__)
#define  KAIXO_UNIQUE_6(m, c, ...)  KAIXO_UNIQUE_5(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c -  5), __VA_ARGS__)
#define  KAIXO_UNIQUE_7(m, c, ...)  KAIXO_UNIQUE_6(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c -  6), __VA_ARGS__)
#define  KAIXO_UNIQUE_8(m, c, ...)  KAIXO_UNIQUE_7(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c -  7), __VA_ARGS__)
#define  KAIXO_UNIQUE_9(m, c, ...)  KAIXO_UNIQUE_8(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c -  8), __VA_ARGS__)
#define KAIXO_UNIQUE_10(m, c, ...)  KAIXO_UNIQUE_9(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c -  9), __VA_ARGS__)
#define KAIXO_UNIQUE_11(m, c, ...) KAIXO_UNIQUE_10(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 10), __VA_ARGS__)
#define KAIXO_UNIQUE_12(m, c, ...) KAIXO_UNIQUE_11(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 11), __VA_ARGS__)
#define KAIXO_UNIQUE_13(m, c, ...) KAIXO_UNIQUE_12(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 12), __VA_ARGS__)
#define KAIXO_UNIQUE_14(m, c, ...) KAIXO_UNIQUE_13(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 13), __VA_ARGS__)
#define KAIXO_UNIQUE_15(m, c, ...) KAIXO_UNIQUE_14(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 14), __VA_ARGS__)
#define KAIXO_UNIQUE_16(m, c, ...) KAIXO_UNIQUE_15(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 15), __VA_ARGS__)
#define KAIXO_UNIQUE_17(m, c, ...) KAIXO_UNIQUE_16(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 16), __VA_ARGS__)
#define KAIXO_UNIQUE_18(m, c, ...) KAIXO_UNIQUE_17(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 17), __VA_ARGS__)
#define KAIXO_UNIQUE_19(m, c, ...) KAIXO_UNIQUE_18(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 18), __VA_ARGS__)
#define KAIXO_UNIQUE_20(m, c, ...) KAIXO_UNIQUE_19(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 19), __VA_ARGS__)
#define KAIXO_UNIQUE_21(m, c, ...) KAIXO_UNIQUE_20(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 20), __VA_ARGS__)
#define KAIXO_UNIQUE_22(m, c, ...) KAIXO_UNIQUE_21(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 21), __VA_ARGS__)
#define KAIXO_UNIQUE_23(m, c, ...) KAIXO_UNIQUE_22(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 22), __VA_ARGS__)
#define KAIXO_UNIQUE_24(m, c, ...) KAIXO_UNIQUE_23(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 23), __VA_ARGS__)
#define KAIXO_UNIQUE_25(m, c, ...) KAIXO_UNIQUE_24(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 24), __VA_ARGS__)
#define KAIXO_UNIQUE_26(m, c, ...) KAIXO_UNIQUE_25(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 25), __VA_ARGS__)
#define KAIXO_UNIQUE_27(m, c, ...) KAIXO_UNIQUE_26(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 26), __VA_ARGS__)
#define KAIXO_UNIQUE_28(m, c, ...) KAIXO_UNIQUE_27(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 27), __VA_ARGS__)
#define KAIXO_UNIQUE_29(m, c, ...) KAIXO_UNIQUE_28(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 28), __VA_ARGS__)
#define KAIXO_UNIQUE_30(m, c, ...) KAIXO_UNIQUE_29(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 29), __VA_ARGS__)
#define KAIXO_UNIQUE_31(m, c, ...) KAIXO_UNIQUE_30(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 30), __VA_ARGS__)
#define KAIXO_UNIQUE_32(m, c, ...) KAIXO_UNIQUE_31(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 31), __VA_ARGS__)
#define KAIXO_UNIQUE_33(m, c, ...) KAIXO_UNIQUE_32(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 32), __VA_ARGS__)
#define KAIXO_UNIQUE_34(m, c, ...) KAIXO_UNIQUE_33(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 33), __VA_ARGS__)
#define KAIXO_UNIQUE_35(m, c, ...) KAIXO_UNIQUE_34(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 34), __VA_ARGS__)
#define KAIXO_UNIQUE_36(m, c, ...) KAIXO_UNIQUE_35(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 35), __VA_ARGS__)
#define KAIXO_UNIQUE_37(m, c, ...) KAIXO_UNIQUE_36(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 36), __VA_ARGS__)
#define KAIXO_UNIQUE_38(m, c, ...) KAIXO_UNIQUE_37(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 37), __VA_ARGS__)
#define KAIXO_UNIQUE_39(m, c, ...) KAIXO_UNIQUE_38(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 38), __VA_ARGS__)
#define KAIXO_UNIQUE_40(m, c, ...) KAIXO_UNIQUE_39(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 39), __VA_ARGS__)
#define KAIXO_UNIQUE_41(m, c, ...) KAIXO_UNIQUE_40(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 40), __VA_ARGS__)
#define KAIXO_UNIQUE_42(m, c, ...) KAIXO_UNIQUE_41(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 41), __VA_ARGS__)
#define KAIXO_UNIQUE_43(m, c, ...) KAIXO_UNIQUE_42(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 42), __VA_ARGS__)
#define KAIXO_UNIQUE_44(m, c, ...) KAIXO_UNIQUE_43(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 43), __VA_ARGS__)
#define KAIXO_UNIQUE_45(m, c, ...) KAIXO_UNIQUE_44(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 44), __VA_ARGS__)
#define KAIXO_UNIQUE_46(m, c, ...) KAIXO_UNIQUE_45(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 45), __VA_ARGS__)
#define KAIXO_UNIQUE_47(m, c, ...) KAIXO_UNIQUE_46(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 46), __VA_ARGS__)
#define KAIXO_UNIQUE_48(m, c, ...) KAIXO_UNIQUE_47(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 47), __VA_ARGS__)
#define KAIXO_UNIQUE_49(m, c, ...) KAIXO_UNIQUE_48(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 48), __VA_ARGS__)
#define KAIXO_UNIQUE_50(m, c, ...) KAIXO_UNIQUE_49(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 49), __VA_ARGS__)
#define KAIXO_UNIQUE_51(m, c, ...) KAIXO_UNIQUE_50(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 50), __VA_ARGS__)
#define KAIXO_UNIQUE_52(m, c, ...) KAIXO_UNIQUE_51(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 51), __VA_ARGS__)
#define KAIXO_UNIQUE_53(m, c, ...) KAIXO_UNIQUE_52(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 52), __VA_ARGS__)
#define KAIXO_UNIQUE_54(m, c, ...) KAIXO_UNIQUE_53(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 53), __VA_ARGS__)
#define KAIXO_UNIQUE_55(m, c, ...) KAIXO_UNIQUE_54(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 54), __VA_ARGS__)
#define KAIXO_UNIQUE_56(m, c, ...) KAIXO_UNIQUE_55(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 55), __VA_ARGS__)
#define KAIXO_UNIQUE_57(m, c, ...) KAIXO_UNIQUE_56(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 56), __VA_ARGS__)
#define KAIXO_UNIQUE_58(m, c, ...) KAIXO_UNIQUE_57(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 57), __VA_ARGS__)
#define KAIXO_UNIQUE_59(m, c, ...) KAIXO_UNIQUE_58(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 58), __VA_ARGS__)
#define KAIXO_UNIQUE_60(m, c, ...) KAIXO_UNIQUE_59(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 59), __VA_ARGS__)
#define KAIXO_UNIQUE_61(m, c, ...) KAIXO_UNIQUE_60(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 60), __VA_ARGS__)
#define KAIXO_UNIQUE_62(m, c, ...) KAIXO_UNIQUE_61(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 61), __VA_ARGS__)
#define KAIXO_UNIQUE_63(m, c, ...) KAIXO_UNIQUE_62(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 62), __VA_ARGS__)
#define KAIXO_UNIQUE_64(m, c, ...) KAIXO_UNIQUE_63(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 63), __VA_ARGS__)
#define KAIXO_UNIQUE_65(m, c, ...) KAIXO_UNIQUE_64(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 64), __VA_ARGS__)
#define KAIXO_UNIQUE_66(m, c, ...) KAIXO_UNIQUE_65(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 65), __VA_ARGS__)
#define KAIXO_UNIQUE_67(m, c, ...) KAIXO_UNIQUE_66(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 66), __VA_ARGS__)
#define KAIXO_UNIQUE_68(m, c, ...) KAIXO_UNIQUE_67(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 67), __VA_ARGS__)
#define KAIXO_UNIQUE_69(m, c, ...) KAIXO_UNIQUE_68(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 68), __VA_ARGS__)
#define KAIXO_UNIQUE_70(m, c, ...) KAIXO_UNIQUE_69(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 69), __VA_ARGS__)
#define KAIXO_UNIQUE_71(m, c, ...) KAIXO_UNIQUE_70(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 70), __VA_ARGS__)
#define KAIXO_UNIQUE_72(m, c, ...) KAIXO_UNIQUE_71(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 71), __VA_ARGS__)
#define KAIXO_UNIQUE_73(m, c, ...) KAIXO_UNIQUE_72(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 72), __VA_ARGS__)
#define KAIXO_UNIQUE_74(m, c, ...) KAIXO_UNIQUE_73(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 73), __VA_ARGS__)
#define KAIXO_UNIQUE_75(m, c, ...) KAIXO_UNIQUE_74(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 74), __VA_ARGS__)
#define KAIXO_UNIQUE_76(m, c, ...) KAIXO_UNIQUE_75(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 75), __VA_ARGS__)
#define KAIXO_UNIQUE_77(m, c, ...) KAIXO_UNIQUE_76(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 76), __VA_ARGS__)
#define KAIXO_UNIQUE_78(m, c, ...) KAIXO_UNIQUE_77(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 77), __VA_ARGS__)
#define KAIXO_UNIQUE_79(m, c, ...) KAIXO_UNIQUE_78(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 78), __VA_ARGS__)
#define KAIXO_UNIQUE_80(m, c, ...) KAIXO_UNIQUE_79(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 79), __VA_ARGS__)
#define KAIXO_UNIQUE_81(m, c, ...) KAIXO_UNIQUE_80(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 80), __VA_ARGS__)
#define KAIXO_UNIQUE_82(m, c, ...) KAIXO_UNIQUE_81(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 81), __VA_ARGS__)
#define KAIXO_UNIQUE_83(m, c, ...) KAIXO_UNIQUE_82(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 82), __VA_ARGS__)
#define KAIXO_UNIQUE_84(m, c, ...) KAIXO_UNIQUE_83(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 83), __VA_ARGS__)
#define KAIXO_UNIQUE_85(m, c, ...) KAIXO_UNIQUE_84(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 84), __VA_ARGS__)
#define KAIXO_UNIQUE_86(m, c, ...) KAIXO_UNIQUE_85(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 85), __VA_ARGS__)
#define KAIXO_UNIQUE_87(m, c, ...) KAIXO_UNIQUE_86(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 86), __VA_ARGS__)
#define KAIXO_UNIQUE_88(m, c, ...) KAIXO_UNIQUE_87(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 87), __VA_ARGS__)
#define KAIXO_UNIQUE_89(m, c, ...) KAIXO_UNIQUE_88(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 88), __VA_ARGS__)
#define KAIXO_UNIQUE_90(m, c, ...) KAIXO_UNIQUE_89(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 89), __VA_ARGS__)
#define KAIXO_UNIQUE_91(m, c, ...) KAIXO_UNIQUE_90(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 90), __VA_ARGS__)
#define KAIXO_UNIQUE_92(m, c, ...) KAIXO_UNIQUE_91(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 91), __VA_ARGS__)
#define KAIXO_UNIQUE_93(m, c, ...) KAIXO_UNIQUE_92(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 92), __VA_ARGS__)
#define KAIXO_UNIQUE_94(m, c, ...) KAIXO_UNIQUE_93(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 93), __VA_ARGS__)
#define KAIXO_UNIQUE_95(m, c, ...) KAIXO_UNIQUE_94(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 94), __VA_ARGS__)
#define KAIXO_UNIQUE_96(m, c, ...) KAIXO_UNIQUE_95(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 95), __VA_ARGS__)
#define KAIXO_UNIQUE_97(m, c, ...) KAIXO_UNIQUE_96(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 96), __VA_ARGS__)
#define KAIXO_UNIQUE_98(m, c, ...) KAIXO_UNIQUE_97(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 97), __VA_ARGS__)
#define KAIXO_UNIQUE_99(m, c, ...) KAIXO_UNIQUE_98(m, c, KAIXO_UNIQUE_NAME, __VA_ARGS__) KAIXO_UNIQUE_1(m, (c - 98), __VA_ARGS__)

    KAIXO_UNIQUE_99(KAIXO_STRUCT_MEMBERS_M, 99);

    /**
     * Find the member types of a struct.
     * @tparam Ty struct
     */
    template<aggregate Ty>
    using struct_members_t = typename struct_members<Ty, struct_size_v<Ty>>::type;
}