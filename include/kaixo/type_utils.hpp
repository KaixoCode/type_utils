#pragma once
#include "impl/info.hpp"
#include "impl/tuple.hpp"
#include "impl/switch.hpp"

namespace kaixo {
    namespace _tests {
        using _big_info = info<int, double, float, char, long, unsigned>;
        using _big_info_d = info<int, double, float, int, double, float>;
        using _small_info = info<int, double, float>;
        static_assert(_big_info::size == 6);
        static_assert(_big_info_d::unique_size == 3);
        static_assert(_small_info::bytes::value<0> == sizeof(int));
        static_assert(_big_info_d::index<double> == 1);
        static_assert(_big_info_d::last_index<double> == 4);
        static_assert(_big_info_d::count<float> == 2);
        static_assert(_big_info_d::occurs<float>);
        static_assert(!_big_info_d::occurs<long>);
        static_assert(_big_info_d::indices<int>.size() == 2);
        static_assert(_big_info_d::indices<int>[0] == 0 && _big_info_d::indices<int>[1] == 3);
        static_assert(_big_info_d::indices_except<int>.size() == 4);
        static_assert(_big_info_d::indices_except<int>[0] == 1);
        static_assert(_big_info_d::indices_filter<is_integral>.size() == 2);
        static_assert(_big_info_d::indices_filter<is_integral>[0] == 0);
        static_assert(same_as<_big_info::element<3>::type, char>);

        static_assert(_big_info::take<_big_info::size>::size == 6);
        static_assert(_big_info::take<0>::size == 0);
        static_assert(_big_info::take<2>::size == 2);
        static_assert(same_as<_big_info::take<2>, info<int, double>>);

        static_assert(_big_info::drop<_big_info::size>::size == 0);
        static_assert(_big_info::drop<0>::size == 6);
        static_assert(_big_info::drop<4>::size == 2);
        static_assert(same_as<_big_info::drop<4>, info<long, unsigned>>);

        static_assert(_big_info::erase<0>::size == 5);
        static_assert(_big_info::erase<5>::size == 5);
        static_assert(same_as<_big_info::erase<5>::last<1>::type, long>);

        static_assert(_big_info::insert<3, int>::size == 7);
        static_assert(_big_info::insert<6, int>::size == 7);
        static_assert(same_as<_big_info::insert<6, int>::element<6>::type, int>);
        static_assert(same_as<_big_info::insert<4, int>::element<6>::type, unsigned>);

        static_assert(_big_info::swap<3, int>::size == 6);
        static_assert(_big_info::swap<5, int>::size == 6);
        static_assert(same_as<_big_info::swap<5, int>::element<5>::type, int>);
        static_assert(same_as<_big_info::swap<4, int>::element<5>::type, unsigned>);

        static_assert(_big_info::sub<2, 4>::size == 2);
        static_assert(same_as<_big_info::sub<2, 4>::element<0>, _big_info::element<2>>);
        static_assert(same_as<_big_info::sub<2, 6>::element<3>, _big_info::element<5>>);

        static_assert(_big_info_d::remove<int>::size == 4);
        static_assert(_big_info_d::keep<int>::size == 2);

        static_assert(_big_info_d::append<int>::size == 7);
        static_assert(same_as<_big_info_d::append<int>::element<6>::type, int>);
        static_assert(_big_info_d::prepend<int>::size == 7);
        static_assert(same_as<_big_info_d::prepend<int>::element<0>::type, int>);

        static_assert(_big_info_d::unique::size == 3);
        static_assert(_big_info_d::reverse::size == 6);
        static_assert(same_as<_big_info::reverse::element<0>, _big_info::element<5>>);
        static_assert(same_as<_big_info::reverse::element<1>, _big_info::element<4>>);
        static_assert(same_as<_big_info::reverse::element<2>, _big_info::element<3>>);

        static_assert(_big_info::filter<is_integral>::size == 4);
        static_assert(_big_info_d::filter<is_integral>::size == 2);
    }
}