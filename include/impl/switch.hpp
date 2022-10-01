#pragma once
#include "helpers.hpp"
#include "functions.hpp"

/**
 * Helper for generating a custom switch statement.
 */
namespace kaixo {

#define   KAIXO_SC1(sc, a)         sc (    0ull + a) 
#define   KAIXO_SC2(sc, a)         sc (    0ull + a)         sc (      1ull + a)
#define   KAIXO_SC4(sc, a)   KAIXO_SC2(sc, 0ull + a)   KAIXO_SC2(sc,   2ull + a)
#define   KAIXO_SC8(sc, a)   KAIXO_SC4(sc, 0ull + a)   KAIXO_SC4(sc,   4ull + a)
#define  KAIXO_SC16(sc, a)   KAIXO_SC8(sc, 0ull + a)   KAIXO_SC8(sc,   8ull + a)
#define  KAIXO_SC32(sc, a)  KAIXO_SC16(sc, 0ull + a)  KAIXO_SC16(sc,  16ull + a)
#define  KAIXO_SC64(sc, a)  KAIXO_SC32(sc, 0ull + a)  KAIXO_SC32(sc,  32ull + a)
#define KAIXO_SC128(sc, a)  KAIXO_SC64(sc, 0ull + a)  KAIXO_SC64(sc,  64ull + a)
#define KAIXO_SC256(sc, a) KAIXO_SC128(sc, 0ull + a) KAIXO_SC128(sc, 128ull + a)
#define KAIXO_SC512(sc, a) KAIXO_SC256(sc, 0ull + a) KAIXO_SC256(sc, 256ull + a)

#define KAIXO_SWITCH_IMPL(TYPE, CASE) \
TYPE(  1ull,   KAIXO_SC1(CASE, 0ull)) \
TYPE(  2ull,   KAIXO_SC2(CASE, 0ull)) \
TYPE(  4ull,   KAIXO_SC4(CASE, 0ull)) \
TYPE(  8ull,   KAIXO_SC8(CASE, 0ull)) \
TYPE( 16ull,  KAIXO_SC16(CASE, 0ull)) \
TYPE( 32ull,  KAIXO_SC32(CASE, 0ull)) \
TYPE( 64ull,  KAIXO_SC64(CASE, 0ull)) \
TYPE(128ull, KAIXO_SC128(CASE, 0ull)) \
TYPE(256ull, KAIXO_SC256(CASE, 0ull)) \
TYPE(512ull, KAIXO_SC512(CASE, 0ull))

    /**
     * Cases switch, has a unique lambda for all
     * the cases.
     */
    template<std::size_t I>
    struct cases_switch_impl;

#define KAIXO_CASES_SWITCH_C(i) case transform(i):      \
if constexpr (i < sizeof...(Args)) {                    \
    if constexpr (std::invocable<                       \
        decltype(std::get<i>(cases)), decltype(index)>) \
        return std::get<i>(cases)(index);               \
    else return std::get<i>(cases)();                   \
} else break; 

#define KAIXO_CASES_SWITCH_S(n, cs)                              \
template<>                                                       \
struct cases_switch_impl<n> {                                    \
template<auto transform, class ...Args>                          \
constexpr static auto handle(Args&& ...cases) {                  \
    return [cases = std::tuple(                                  \
                    std::forward<Args>(cases)...)](auto index) { \
        switch (index) { cs }                                    \
    };                                                           \
}                                                                \
};

    KAIXO_SWITCH_IMPL(KAIXO_CASES_SWITCH_S, KAIXO_CASES_SWITCH_C)
#undef KAIXO_CASES_SWITCH_S
#undef KAIXO_CASES_SWITCH_C

    /**
     * Generate a switch statement with lambdas as cases.
     * @tparam transform transform case index to any other value
     * @param cases... functors, either invocable with case value, or nothing
     * @return generated switch
     */
    template<auto transform = unit>
    constexpr auto generate_switch = []<class ...Functors>(Functors&& ...cases) {
        constexpr auto p2 = closest_larger_power2(sizeof...(Functors));
        return tuple_switch_impl<p2>::template handle<transform>(std::forward<Functors>(cases)...);
    };

    /**
     * Templated switch statement, where the
     * argument will be converted into a template parameter value.
     */
    template<std::size_t I>
    struct template_switch_impl;

#define KAIXO_TEMPLATE_SWITCH_C(i) case transform(i):  \
if constexpr (i < cases) {                             \
    return functor.operator()<transform(i)>();         \
} else break; 

#define KAIXO_TEMPLATE_SWITCH_S(n, cs)                           \
template<>                                                       \
struct template_switch_impl<n> {                                 \
template<auto cases, auto transform, class Arg>                  \
constexpr static auto handle(Arg&& functor) {                    \
    return [functor = std::forward<Arg>(functor)](auto index) {  \
        switch (index) { cs }                                    \
    };                                                           \
}                                                                \
};

    KAIXO_SWITCH_IMPL(KAIXO_TEMPLATE_SWITCH_S, KAIXO_TEMPLATE_SWITCH_C)
#undef KAIXO_TEMPLATE_SWITCH_S
#undef KAIXO_TEMPLATE_SWITCH_C

    /**
     * Generate a template switch statement, takes a single
     * functor which has a template argument value.
     * @tparam cases how many cases to generate
     * @tparam transform transform the case index
     * @return generated template switch
     */
    template<std::unsigned_integral auto cases, auto transform = unit>
    constexpr auto generate_template_switch = []<class Arg>(Arg && functor) {
        constexpr auto p2 = closest_larger_power2(cases);
        return template_switch_impl<p2>::template handle<cases, transform>(std::forward<Arg>(functor));
    };
}