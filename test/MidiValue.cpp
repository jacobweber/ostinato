#include <catch2/catch_test_macros.hpp>

#include "MidiValue.h"

TEST_CASE("MidiValue comparisons")
{
    MidiValue l1{10, 1};
    MidiValue r1{11, 1};
    REQUIRE(l1 < r1);
    REQUIRE(r1 > l1);
    REQUIRE(r1 >= l1);

    MidiValue l2{10, 1};
    MidiValue r2{10, 2};
    REQUIRE(l2 < r2);
    REQUIRE(r2 > l2);
    REQUIRE(r2 >= l2);

    MidiValue l3{10, 1};
    MidiValue r3{10, 1};
    REQUIRE(l3 == r3);
    REQUIRE(r3 >= l3);
}

TEST_CASE("MidiValue assignment")
{
    MidiValue r1{2, 1};
    MidiValue r2{1, 1};
    r1 = r2;
    REQUIRE(r1 == r2);
    r1.channel = 3;
    REQUIRE(r2.channel == 1);
}
