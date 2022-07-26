# Type Utils

Single header type utilities library for things like template packs, structs, functions, enums. 

```cpp
using namespace kaixo;

struct Struct {
    int a;
    double b;
    float c;
};

int my_fun(double, long, int, float) { return 1; }

enum my_enum { Value1, Value2 };

// boolean operations on type traits
template<require<is_integral || is_floating_point> Ty> 
struct my_type {};

int main() {

    // Simple type traits
    static_assert(info<int>::is_integral); 
    static_assert(info<double[3][5]>::extent<1> == 5);
    static_assert(info<double, long, int, float>::can_invoke<decltype(my_fun)>);
    
    // Query function information
    static_assert(info_v<my_fun>::arguments::size == 4); 
    static_assert(same_as<info_v<my_fun>::result::type, int>); 
    static_assert(same_as<info_v<my_fun>::pointer::type, int(*)(double, long, int, float)>);

    // Query struct member types
    static_assert(info<Struct>::members::size == 3);
    static_assert(same_as<info<Struct>::members::element<0>::type, int>);

    // Complex template pack manipulation
    static_assert(info<int, double, float>::filter<is_integral>::size == 1);
    static_assert(info<int, double, int>::indices<int>.size() == 2); // result: std::array{ 0ull, 2ull }
    static_assert(same_as<info<int, double, int, float>::unique, info<int, double, float>>);
    static_assert(info<int, unsigned, float, double>::count_filter<is_integral> == 2);

    // Enum value names
    static_assert(info<my_enum>::name<Value1> == "Value1");
    static_assert(info<my_enum>::name<1> == "Value2");
    static_assert(info<my_enum>::defined<0> == true);
    static_assert(info<my_enum>::defined<3> == false);

    // Type manipulation
    static_assert(same_as<info<int&>::add_cvref_from<const int>::type, const int&>);
    static_assert(same_as<info<const int&>::copy_ref_to<int>::type, int&>);

    return 0;
}
```
