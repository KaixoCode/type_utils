﻿#include "kaixo/type_utils.hpp"
#include <vector>

using namespace kaixo;

struct Struct {
    int a;
    float b;
    double c;
};

int my_fun(double, long, int, float) { return 1; }

enum my_enum { Value1, Value2 };

// boolean operations on type traits
template<require<is_integral || is_floating_point> Ty> 
struct my_type {};

int main() {


    // Simple type traits
    static_assert(info<int>::is_integral::value); 
    static_assert(info<double[3][5]>::extent<1>::value == 5);
    static_assert(info<double, long, int, float>::can_invoke<decltype(my_fun)>::value);
    
    // Query function information
    static_assert(info_v<my_fun>::arguments::size == 4); 
    static_assert(same_as<info_v<my_fun>::result::type, int>); 
    static_assert(same_as<info_v<my_fun>::pointer::type, int(*)(double, long, int, float)>);

    // Query struct member types (BROKEN RIGHT NOW)
    //static_assert(info<Struct>::members::type::size == 3);
    //static_assert(same_as<info<Struct>::members::element<0>::type, int>);

    // Complex template pack manipulation
    static_assert(info<int, double, float>::filter<is_integral>::size == 1);
    static_assert(info<int, double, int>::indices<int>.size() == 2); // result: std::array{ 0ull, 2ull }
    static_assert(same_as<info<int, double, int, float>::unique, info<int, double, float>>);
    static_assert(info<int, unsigned, float, double>::count_filter<is_integral> == 2);

    // Enum value names
    static_assert(info<my_enum>::name<Value1>::value == "Value1");
    static_assert(info<my_enum>::name<1>::value == "Value2");
    static_assert(info<my_enum>::defined<0>::value == true);
    static_assert(info<my_enum>::defined<3>::value == false);

    // Type manipulation
    static_assert(same_as<info<int&>::add_cvref_from<const int>::type, const int&>);
    static_assert(same_as<info<const int&>::copy_ref_to<int>::type, int&>);

    // Sort types
    static_assert(same_as<
        info<uint16_t, uint64_t, uint8_t, uint32_t>::sort<type_sorters::rsize>,
        info<uint64_t, uint32_t, uint16_t, uint8_t>
    >);

    // Transform pack of types
    static_assert(same_as<
        info<std::vector<int>, std::vector<double>>::transform<grab::reference>,
        info<int&, double&>
    >);

    return 0;
}