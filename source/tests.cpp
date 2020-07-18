#define CATCH_CONFIG_RUNNER
#include <catch.hpp>
#define _USE_MATH_DEFINES //for pi
#include <math.h>

#include "sphere.hpp"
#include "box.hpp"

TEST_CASE("test")
{
    Sphere s{ {0, 0, 0}, 5 }; //sphere with centre at origin and radius of 5
    REQUIRE(s.area() == Approx(25.f * M_PI));
    REQUIRE(s.volume() == Approx(125.f * 4.f / 3.f * M_PI));
}

int main(int argc, char *argv[])
{
  return Catch::Session().run(argc, argv);
}
