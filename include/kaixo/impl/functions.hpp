#pragma once 
#include "helpers.hpp"
#include "concepts.hpp"

/**
 * Information regarding function, member functions, and function pointers.
 */
namespace kaixo {

    namespace functions {
        constexpr auto unit = []<class Ty>(Ty && i) -> Ty&& { return std::forward<Ty>(i); };
        template<class To>
        constexpr auto cast = []<class Ty>(Ty && i) -> To { return static_cast<To>(std::forward<Ty>(i)); };
    }

#define NO_ARG
#define KAIXO_MEMBER_CALL_C(MAC, V, REF, NOEXCEPT) \
MAC(NO_ARG, V, REF, NOEXCEPT)                       \
MAC(const , V, REF, NOEXCEPT)

#define KAIXO_MEMBER_CALL_V(MAC, REF, NOEXCEPT)   \
KAIXO_MEMBER_CALL_C(MAC, NO_ARG  , REF, NOEXCEPT) \
KAIXO_MEMBER_CALL_C(MAC, volatile, REF, NOEXCEPT)

#define KAIXO_MEMBER_CALL_NOEXCEPT(MAC, NOEXCEPT) \
KAIXO_MEMBER_CALL_V(MAC, NO_ARG, NOEXCEPT)            \
KAIXO_MEMBER_CALL_V(MAC,      &, NOEXCEPT)            \
KAIXO_MEMBER_CALL_V(MAC,     &&, NOEXCEPT)

#define KAIXO_MEMBER_CALL(MAC)            \
KAIXO_MEMBER_CALL_NOEXCEPT(MAC, NO_ARG  ) \
KAIXO_MEMBER_CALL_NOEXCEPT(MAC, noexcept) 

    namespace detail {
        template<class> struct function_info_impl;
        template<concepts::functor Ty> struct function_info_impl<Ty>
        : function_info_impl<decltype(&Ty::operator())> {};
    

#define KAIXO_MEMBER_FUNCTION_INFO_MOD(CONST, VOLATILE, REF, NOEXCEPT)                   \
template<class Ty, class R, class ...Args>                                               \
struct function_info_impl<R(Ty::*)(Args...) CONST VOLATILE REF NOEXCEPT> {               \
    using pointer = R(*)(Args...) NOEXCEPT;                                              \
    using signature = R(Args...) CONST VOLATILE REF NOEXCEPT;                            \
    using object = CONST VOLATILE Ty REF;                                                \
    using result = R;                                                                    \
    using arguments = info<Args...>;                                                     \
    constexpr static bool is_fun_const = std::same_as<const int, CONST int>;             \
    constexpr static bool is_fun_mutable = !is_fun_const;                                \
    constexpr static bool is_fun_volatile = std::same_as<volatile int, VOLATILE int>;    \
    constexpr static bool is_fun_lvalue_reference = std::same_as<int&, int REF>;         \
    constexpr static bool is_fun_rvalue_reference = std::same_as<int&&, int REF>;        \
    constexpr static bool is_fun_reference = std::is_reference_v<int REF>;               \
    constexpr static bool is_noexcept = std::same_as<void() noexcept, void() NOEXCEPT>;  \
    using add_fun_const = R(Ty::*)(Args...) const VOLATILE REF NOEXCEPT;                 \
    using remove_fun_const = R(Ty::*)(Args...) VOLATILE REF NOEXCEPT;                    \
    using add_fun_volatile = R(Ty::*)(Args...) CONST volatile REF NOEXCEPT;              \
    using remove_fun_volatile = R(Ty::*)(Args...) CONST REF NOEXCEPT;                    \
    using add_fun_cv = R(Ty::*)(Args...) const volatile REF NOEXCEPT;                    \
    using remove_fun_cv = R(Ty::*)(Args...) REF NOEXCEPT;                                \
    using add_fun_lvalue_reference = R(Ty::*)(Args...) CONST VOLATILE & NOEXCEPT;        \
    using add_fun_rvalue_reference = R(Ty::*)(Args...) CONST VOLATILE && NOEXCEPT;       \
    using remove_fun_reference = R(Ty::*)(Args...) CONST VOLATILE NOEXCEPT;              \
    using remove_fun_cvref = R(Ty::*)(Args...) NOEXCEPT;                                 \
    using add_noexcept = R(Ty::*)(Args...) CONST VOLATILE REF noexcept;                  \
    using remove_noexcept = R(Ty::*)(Args...) CONST VOLATILE REF;                        \
    using fun_decay = R(Ty::*)(Args...);                                                 \
};

    KAIXO_MEMBER_CALL(KAIXO_MEMBER_FUNCTION_INFO_MOD);
#undef KAIXO_MEMBER_FUNCTION_INFO_MOD

#define KAIXO_FUNCTION_INFO_MOD(CONST, VOLATILE, REF, NOEXCEPT)                         \
template<class R, class ...Args>                                                        \
struct function_info_impl<R(Args...) CONST VOLATILE REF NOEXCEPT> {                     \
    using pointer = R(*)(Args...) NOEXCEPT;                                             \
    using signature = R(Args...) CONST VOLATILE REF NOEXCEPT;                           \
    using result = R;                                                                   \
    using arguments = info<Args...>;                                                    \
    constexpr static bool is_fun_const = std::same_as<const int, CONST int>;            \
    constexpr static bool is_fun_mutable = !is_fun_const;                               \
    constexpr static bool is_fun_volatile = std::same_as<volatile int, VOLATILE int>;   \
    constexpr static bool is_fun_lvalue_reference = std::same_as<int&, int REF>;        \
    constexpr static bool is_fun_rvalue_reference = std::same_as<int&&, int REF>;       \
    constexpr static bool is_fun_reference = std::is_reference_v<int REF>;              \
    constexpr static bool is_noexcept = std::same_as<void() noexcept, void() NOEXCEPT>; \
    using add_fun_const = R(Args...) const VOLATILE REF NOEXCEPT;                       \
    using remove_fun_const = R(Args...) VOLATILE REF NOEXCEPT;                          \
    using add_fun_volatile = R(Args...) CONST volatile REF NOEXCEPT;                    \
    using remove_fun_volatile = R(Args...) CONST REF NOEXCEPT;                          \
    using add_fun_cv = R(Args...) const volatile REF NOEXCEPT;                          \
    using remove_fun_cv = R(Args...) REF NOEXCEPT;                                      \
    using add_fun_lvalue_reference = R(Args...) CONST VOLATILE & NOEXCEPT;              \
    using add_fun_rvalue_reference = R(Args...) CONST VOLATILE && NOEXCEPT;             \
    using remove_fun_reference = R(Args...) CONST VOLATILE NOEXCEPT;                    \
    using remove_fun_cvref = R(Args...) NOEXCEPT;                                       \
    using add_noexcept = R(Args...) CONST VOLATILE REF noexcept;                        \
    using remove_noexcept = R(Args...) CONST VOLATILE REF;                              \
    using fun_decay = R(Args...);                                                       \
};

    KAIXO_MEMBER_CALL(KAIXO_FUNCTION_INFO_MOD);
#undef KAIXO_FUNCTION_INFO_MOD

#define KAIXO_FUNCTION_PTR_INFO_MOD(NOEXCEPT)                                               \
template<class R, class ...Args>                                                            \
struct function_info_impl<R(*)(Args...) NOEXCEPT> {                                         \
    using pointer = R(*)(Args...) NOEXCEPT;                                                 \
    using signature = R(Args...) NOEXCEPT;                                                  \
    using result = R;                                                                       \
    using arguments = info<Args...>;                                                        \
    constexpr static bool is_noexcept = std::same_as<void() noexcept, void() NOEXCEPT>;     \
    using add_noexcept = R(Args...) noexcept;                                               \
    using remove_noexcept = R(Args...);                                                     \
    using fun_decay = R(*)(Args...);                                                        \
};

    KAIXO_FUNCTION_PTR_INFO_MOD(NO_ARG);
    KAIXO_FUNCTION_PTR_INFO_MOD(noexcept);
#undef KAIXO_FUNCTION_PTR_INFO_MOD

        template<class Ty> using function_info = function_info_impl<std::remove_cv_t<std::remove_reference_t<Ty>>>;
        template<class Ty> concept callable_type = requires(Ty) { typename function_info<Ty>::result; };
    }

    // =======================================================

    template<class ...Ty> requires (sizeof...(Ty) <= 1)
    struct arguments {
        using type = info<>;
        using _type = type;
    };

    template<detail::callable_type Ty>
    struct arguments<Ty> {
        using type = detail::function_info<Ty>::arguments;
        using _type = type;
    };

    template<>
    struct arguments<> {
        template<class Ty>
        using type = arguments<Ty>::type;
        using _type = templated_t<type>;
    };

    template<class ...Ty> requires (sizeof...(Ty) <= 1)
    using arguments_t = arguments<Ty...>::_type;

    // =======================================================
    
    template<class ...Ty> requires (sizeof...(Ty) <= 1)
    struct return_type {
        using type = info<>;
        using _type = type;
    };

    template<detail::callable_type Ty>
    struct return_type<Ty> {
        using type = detail::function_info<Ty>::result;
        using _type = type;
    };

    template<>
    struct return_type<> {
        template<class Ty>
        using type = return_type<Ty>::type;
        using _type = templated_t<type>;
    };

    template<class ...Ty> requires (sizeof...(Ty) <= 1)
    using return_type_t = return_type<Ty...>::_type;

    // =======================================================

}