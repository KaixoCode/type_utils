#pragma once
#include <type_traits>
#include <typeinfo>
#include <concepts>
#include <array>
#include <utility>
#include <string_view>
#include <cmath>
#include <algorithm>
#include <tuple>
#include "string_literal.hpp"
#include "sequence.hpp"

/**
 * Some general helpers and pre-definitions.
 */
namespace kaixo {
    template<class ...Tys> struct info;
    template<class ...Args> struct template_pack;

    /**
     * Find the closes larger power of 2.
     * @param v value
     * @return closes larger power of 2 from value
     */
    template <std::unsigned_integral Ty>
    constexpr Ty closest_larger_power2(Ty v) {
        return v > 1ull ? 1ull << (sizeof(Ty) * CHAR_BIT - std::countl_zero(v - 1ull)) : v;
    }
    static_assert(closest_larger_power2(2ull) == 2);
    static_assert(closest_larger_power2(3ull) == 4);
    static_assert(closest_larger_power2(33ull) == 64);
    static_assert(closest_larger_power2(128ull) == 128);

    /**
     * Find the next multiple of a number.
     * @param num number
     * @param multiple multiple
     * @return next multiple of num
     */
    template<class A, class B>
    constexpr std::common_type_t<A, B> next_multiple(A num, B multiple) {
        using Ty = std::common_type_t<A, B>;
        if constexpr (std::is_floating_point_v<Ty>) {
            if (multiple == 0) return num;

            const auto remainder = num - static_cast<std::int64_t>(num / multiple) * multiple;
            if (remainder == 0) return num;

            return static_cast<Ty>(num + multiple - remainder);
        } else {
            if (multiple == 0) return num;

            const auto remainder = num % multiple;
            if (remainder == 0) return num;

            return static_cast<Ty>(num + multiple - remainder);
        }
    }
    static_assert(next_multiple(2, 0) == 2);
    static_assert(next_multiple(2, 5) == 5);
    static_assert(next_multiple(5, 5) == 5);
    static_assert(next_multiple(13, 12) == 24);

    /**
     * Dud type, used in places as a placeholder, or
     * when nothing else should match.
     */
    struct dud {
        constexpr bool operator==(const dud&) const { return true; }
    };

    /**
     * Overloaded Functor.
     * @tparam Functors... functor types
     */
    template<class ...Functors>
    struct overloaded : Functors... {
        using Functors::operator()...;
    };

    /**
     * Type for a template value.
     * @tparam V template value
     */
    template<auto V>
    struct value_t {
        constexpr static auto value = V;
    };

    /**
     * Wrapper for a templated type.
     * @tparam T templated type
     */
    template<template<class...> class T>
    struct templated_t {
        template<class ...Args>
        using type = T<Args...>;
    };

    /**
     * Change a type to Ty, useful in fold expressions.
     * @tparam Ty type to change to
     */
    template<class, class Ty> using change = Ty;

    /**
     * Class that's convertible to every type.
     */
    struct convertible_to_everything {
        // Complex requires clause for struct_size, when member
        // defines constructor that takes anything that's convertible
        // to something, it complains about ambiguous conversion operations.
        // This constraint on constructible_from prevents this.
        template<class Ty>
            requires (std::is_lvalue_reference_v<Ty&> && !std::is_rvalue_reference_v<Ty&> &&
            !std::constructible_from<Ty&&, convertible_to_everything> &&
            !std::constructible_from<Ty&, convertible_to_everything> &&
            !std::constructible_from<Ty, convertible_to_everything>)
        constexpr operator Ty& ();

        template<class Ty>
            requires (std::is_rvalue_reference_v<Ty&&> && !std::is_lvalue_reference_v<Ty&&> &&
            !std::constructible_from<Ty&&, convertible_to_everything> &&
            !std::constructible_from<Ty&, convertible_to_everything> &&
            !std::constructible_from<Ty, convertible_to_everything>)
        constexpr operator Ty&& ();
    };

    /**
     * Class that's only convertible to Tys...
     */
    template<class ...Tys>
    struct only_convertible_to {
        template<class Ty> 
            requires (((std::same_as<std::decay_t<Ty>, Tys>) || ...) &&
            std::is_lvalue_reference_v<Ty&&> && !std::is_rvalue_reference_v<Ty&&> &&
            !std::constructible_from<Ty&&, only_convertible_to> &&
            !std::constructible_from<Ty&, only_convertible_to> &&
            !std::constructible_from<Ty, only_convertible_to>)
        constexpr operator Ty && ();
        template<class Ty>
            requires (((std::same_as<std::decay_t<Ty>, Tys>) || ...) &&
            std::is_lvalue_reference_v<Ty&> && !std::is_rvalue_reference_v<Ty&> &&
            !std::constructible_from<Ty&&, only_convertible_to> &&
            !std::constructible_from<Ty&, only_convertible_to> &&
            !std::constructible_from<Ty, only_convertible_to>)
        constexpr operator Ty& ();
    };

    /**
     * Class that's not convertible to Tys...
     */
    template<class ...Tys>
    struct not_convertible_to {
        template<class Ty> 
            requires (((!std::same_as<std::decay_t<Ty>, Tys>) && ...)
            && ((!std::is_base_of_v<std::decay_t<Ty>, Tys>) && ...) &&
            std::is_lvalue_reference_v<Ty&> && !std::is_rvalue_reference_v<Ty&> &&
            !std::constructible_from<Ty&&, not_convertible_to> &&
            !std::constructible_from<Ty&, not_convertible_to> &&
            !std::constructible_from<Ty, not_convertible_to>)
        constexpr operator Ty& ();
        template<class Ty> 
            requires (((!std::same_as<std::decay_t<Ty>, Tys>) && ...)
            && ((!std::is_base_of_v<std::decay_t<Ty>, Tys>) && ...) &&
            std::is_lvalue_reference_v<Ty&&> && !std::is_rvalue_reference_v<Ty&&> &&
            !std::constructible_from<Ty&&, not_convertible_to> &&
            !std::constructible_from<Ty&, not_convertible_to> &&
            !std::constructible_from<Ty, not_convertible_to>)
        constexpr operator Ty && ();
    };

    /**
     * Extract enum name from function signature, used in
     * enum_name_impl.
     * @param name string containing enum name
     * @return extracted enum name
     */
    consteval std::string_view _enum_pretty_name(std::string_view name) noexcept {
        // Starting at end of string_view, only keep valid enum name characters
        for (std::size_t i = name.size(); i > 0; --i) {
            auto& c = name[i - 1];
            // Valid characters are [0-9a-zA-Z_]
            if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z')
                || (c >= 'A' && c <= 'Z') || (c == '_'))) {
                // Remove prefix once we've hit invalid character
                name.remove_prefix(i);
                break;
            }
        }

        // Make sure first character is valid as well.
        if (name.size() > 0 && ((name.front() >= 'a' && name.front() <= 'z') ||
            (name.front() >= 'A' && name.front() <= 'Z') || (name.front() == '_')))
            return name;

        return {}; // Invalid name.
    }

    /**
     * Get enum name using function signature macro.
     * @tparam Ty enum type
     * @tparam Value enum value
     * @return extracted enum name
     */
    template<class Ty, Ty Value>
    consteval std::string_view enum_name_impl() noexcept {
#if defined(__clang__) || defined(__GNUC__)
        return _enum_pretty_name({ __PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - 2 });
#elif defined(_MSC_VER)
        return _enum_pretty_name({ __FUNCSIG__, sizeof(__FUNCSIG__) - 17 });
#else
        return string_view{};
#endif
    }

    /**
     * Get enum name.
     * @tparam Ty enum type
     * @tparam V value that's convertible to Ty
     */
    template<class Ty, auto V>
    constexpr auto enum_name = [] {
        constexpr auto name = enum_name_impl<Ty, static_cast<Ty>(V)>();
        if constexpr (name.data() == nullptr) return string_literal<1>{ "\0" };
        else return string_literal<name.size() + 1>{ name };
    }();

    /**
     * Extract value name from function signature.
     * @tparam Value template value
     * @return extracted value name
     */
    template<auto Value>
    consteval auto value_name_impl() noexcept {
#if defined(__clang__) || defined(__GNUC__)
        constexpr auto name = std::string_view{ __PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - 2 };
        // Remove prefix, template argument starts at first '<'
        return name.substr(name.find_first_of('<') + 1);
#elif defined(_MSC_VER)
        constexpr auto name = std::string_view{ __FUNCSIG__, sizeof(__FUNCSIG__) - 17 };
        // Remove prefix, template argument starts at first '<'
        return name.substr(name.find_first_of('<') + 1);
#else
        return string_view{};
#endif
    }

    /**
     * Get value as string.
     * @tparam V template value
     */
    template<auto V>
    constexpr auto value_name = [] {
        constexpr auto name = value_name_impl<V>();
        if constexpr (name.data() == nullptr) return string_literal<1>{ "\0" };
        else return string_literal<name.size() + 1>{ name };
    }();

    /**
     * Get pretty function name from string_view containing
     * the function name.
     * @param name string containing function name
     * @return extracted function name
     */
    constexpr std::string_view _function_pretty_name(std::string_view name) noexcept {
        if (name.size() == 0) return {};

        // remove call '( ... )' part
        std::size_t count = 0;
        std::size_t suffix = 0;
        for (std::size_t i = name.size(); i > 0; --i) {
            auto& c = name[i - 1];
            if (c == ')') count++;
            if (c == '(') count--;
            suffix++;
            if (count == 0) {
                name.remove_suffix(suffix);
                break;
            }
        }

        if (name.size() == 0) return {};

        // Remove template part if it exists '< ... >'
        if (name[name.size() - 1] == '>') {
            count = 0;
            suffix = 0;
            for (std::size_t i = name.size(); i > 0; --i) {
                auto& c = name[i - 1];
                if (c == '>') count++;
                if (c == '<') count--;

                suffix++;
                if (count == 0) {
                    name.remove_suffix(suffix);
                    break;
                }
            }
        }

        if (name.size() == 0) return {};

        // Collect valid identifier characters
        for (std::size_t i = name.size(); i > 0; --i) {
            if (auto& c = name[i - 1]; !((c >= '0' && c <= '9') ||
                (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_'))) {
                name.remove_prefix(i); break;
            }
        }

        // Make sure it starts with a valid character
        if (name.size() > 0 && ((name.front() >= 'a' && name.front() <= 'z') ||
            (name.front() >= 'A' && name.front() <= 'Z') || (name.front() == '_')))
            return name;

        return {}; // Invalid name.
    }

    /**
     * Get function name from function signature.
     * @tparam Value function pointer
     * @return function name
     */
    template<auto Value>
    consteval auto function_name_impl() noexcept {
#if defined(__clang__) || defined(__GNUC__)
        constexpr auto name = std::string_view{ __PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - 2 };
        return _function_pretty_name(name.substr(name.find_first_of('<') + 1));
#elif defined(_MSC_VER)
        constexpr auto name = std::string_view{ __FUNCSIG__, sizeof(__FUNCSIG__) - 17 };
        return _function_pretty_name(name.substr(name.find_first_of('<') + 1));
#else
        return string_view{};
#endif
    }

    /**
     * Get function name.
     * @tparam Value (member) function pointer
     */
    template<auto V>
    constexpr auto function_name = [] {
        constexpr auto name = function_name_impl<V>();
        if constexpr (name.data() == nullptr) return string_literal<1>{ "\0" };
        else return string_literal<name.size() + 1>{ name };
    }();

    /**
     * Get type name from function signature.
     * @tparam Value type
     * @return type name
     */
    template<class Value>
    consteval auto type_name_impl() noexcept {
#if defined(__clang__) || defined(__GNUC__)
        auto name = std::string_view{ __PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - 2 };
        name = name.substr(name.find_first_of('<') + 1);
        if (name.starts_with("struct ")) name = name.substr(7);
        return name;
#elif defined(_MSC_VER)
        auto name = std::string_view{ __FUNCSIG__, sizeof(__FUNCSIG__) - 17 };
        name = name.substr(name.find_first_of('<') + 1);
        if (name.starts_with("struct ")) name = name.substr(7);
        return name;
#else
        return string_view{};
#endif
    }
    /**
     * Get type name.
     * @tparam V type
     */
    template<class V>
    constexpr auto type_name = [] {
        constexpr auto name = type_name_impl<V>();
        if constexpr (name.data() == nullptr) return string_literal<1>{ "\0" };
        else return string_literal<name.size() + 1>{ name };
    }();

    /**
     * Basically sizeof(Ty), but special case for
     * void and functions, as they normally give errors.
     */
    template<class Ty>
    constexpr std::size_t sizeof_v = [] {
        if constexpr (std::is_void_v<Ty>) return 0;
        else if constexpr (std::is_function_v<Ty>) return 0;
        else if constexpr (std::is_array_v<Ty> && std::extent_v<Ty> == 0) return 0;
        else return sizeof(Ty);
    }();

    /**
     * Basically alignof(Ty), but special case for
     * void and functions, as they normally give errors.
     */
    template<class Ty>
    constexpr std::size_t alignof_v = [] {
        if constexpr (std::is_void_v<Ty>) return 0;
        else if constexpr (std::is_function_v<Ty>) return 0;
        else return std::alignment_of_v<Ty>;
    }();

    template<template<class Ty, class ...Args> class Trait, class Ty, class ...Args>
    struct pack_trait_helper : Trait<Ty, Args...> {};
    template<template<class Ty, class ...Args> class Trait, class Ty, class ...Args>
    struct pack_trait_helper<Trait, Ty, info<Args...>> : Trait<Ty, Args...> {};
}