#include <catch2/catch_all.hpp>

#include "Engine.hpp"

TEST_CASE("Test returns 420")
{
    int Result = Test();
    REQUIRE(Result == 420);
}
