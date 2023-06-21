#pragma once
#include <concepts>
#include <utility>
#include <cstddef>

/**
 * Helpers for calling a lambda with a sequence of template values.
 */
namespace kaixo {
    constexpr std::size_t npos = static_cast<std::size_t>(-1);

    /**
     * Templated for, calls lambda with index sequence in pack,
     * requires S < E
     * @tparam S start value, or size when E is left at npos
     * @tparam E end value, or nothing when npos
     */
    template<std::integral auto S, std::integral auto E = npos> constexpr auto sequence = [](auto lambda) {
        if constexpr (E == npos)
            return[&] <std::size_t ...Is>(std::integer_sequence<decltype(S), Is...>) {
            return lambda.operator() < (Is)... > ();
        }(std::make_integer_sequence<decltype(S), S>{});
        else return[&] <auto ...Is>(std::integer_sequence<decltype(E - S), Is...>) {
            return lambda.operator() < (Is + S)... > ();
        }(std::make_integer_sequence<decltype(E - S), E - S>{});
    };

    /**
     * Templated for, calls lambda with index sequence in reverse in pack,
     * requires S < E
     * @tparam S start value, or size when E is left at npos
     * @tparam E end value, or nothing when npos
     */
    template<std::integral auto S, std::integral auto E = npos> constexpr auto reverse_sequence = [](auto lambda) {
        if constexpr (E == npos)
            return[&] <std::size_t ...Is>(std::integer_sequence<decltype(S), Is...>) {
            return lambda.operator() < (S - Is - 1)... > ();
        }(std::make_integer_sequence<decltype(S), S>{});
        else return[&] <auto ...Is>(std::integer_sequence<decltype(E - S), Is...>) {
            return lambda.operator() < ((E + S) - (Is + S) - 1)... > ();
        }(std::make_integer_sequence<decltype(E - S), E - S>{});
    };

    /**
     * Templated for, calls lambda with all indices separately,
     * requires S < E
     * @tparam S start value, or size when E is left at npos
     * @tparam E end value, or nothing when npos
     */
    template<std::integral auto S, std::integral auto E = npos> constexpr auto indexed_for =
        []<class Ty>(Ty && lambda) {
        if constexpr (E == npos)
            [&] <auto ...Is>(std::integer_sequence<decltype(S), Is...>) {
            (lambda.operator() < Is > (), ...);
        }(std::make_integer_sequence<decltype(S), S>{});
        else[&] <auto ...Is>(std::integer_sequence<decltype(E - S), Is...>) {
            (lambda.operator() < Is + S > (), ...);
        }(std::make_integer_sequence<decltype(E - S), E - S>{});
    };

    /**
     * Call lambda with array values as template arguments, like
     * Lambda.operator()<Array[Is]...>();
     */
    template<auto Array>
    constexpr auto iterate = [](auto Lambda) {
        return[Lambda]<std::size_t ...Is>(std::index_sequence<Is...>) {
            return Lambda.operator() < Array[Is]... > ();
        }(std::make_index_sequence<Array.size()>{});
    };
}