#include "type_utils.hpp"

struct Struct {
    int a;
    double b;
    float c;
};

int my_fun(double, long, int, float) { return 1; }

int main() {
    using namespace kaixo;

    // Simple type traits
    static_assert(info<int>::is_integral); 
    
    // Query function information
    static_assert(info_v<my_fun>::arguments::size == 4); 
    static_assert(same_as<info_v<my_fun>::result::type, int>); 

    // Query struct member types
    static_assert(info<Struct>::members::size == 3);
    static_assert(same_as<info<Struct>::members::element<0>::type, int>);

    return 0;
}