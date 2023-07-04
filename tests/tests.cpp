#include <iostream>
#include "kaixo/type_utils.hpp"
#include "concepts_tests.hpp"
#include "functions_tests.hpp"
#include "helpers_tests.hpp"
#include "info_tests.hpp"
#include "members_tests.hpp"
#include "pack_tests.hpp"
#include "qualifiers_tests.hpp"
#include "sequence_tests.hpp"
#include "string_literal_tests.hpp"
#include "struct_tests.hpp"
#include "switch_tests.hpp"
#include "tuple_tests.hpp"
#include "type_filter_tests.hpp"
#include "type_traits_tests.hpp"


int main() {
    concepts_tests();
    functions_tests();
    helpers_tests();
    info_tests();
    members_tests();
    pack_tests();
    qualifiers_tests();
    sequence_tests();
    string_literal_tests();
    struct_tests();
    switch_tests();
    tuple_tests();
    type_filter_tests();
    type_traits_tests();
}
