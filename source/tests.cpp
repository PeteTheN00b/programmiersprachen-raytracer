#define CATCH_CONFIG_RUNNER
#include <catch.hpp>
#define _USE_MATH_DEFINES //for pi
#include <math.h>

#include "sphere.hpp"
#include "box.hpp"

TEST_CASE("Sphere area and volume functions")
{
    Sphere s{ "MySphere", {1, 0, 0}, {0, 0, 0}, 5 }; //sphere with centre at origin and radius of 5
    REQUIRE(s.area() == Approx(25. * 4. * M_PI));
    REQUIRE(s.volume() == Approx(125. * 4. / 3. * M_PI));

    s.print(std::cout);
}

TEST_CASE("Box area and volume functions")
{
    Box b{ "MyBox", {0, 1, 0.5}, 2, 4, 8 };
    REQUIRE(b.area() == Approx(112.f));
    REQUIRE(b.volume() == Approx(64.f));

    b.print(std::cout);
}

int main(int argc, char *argv[])
{
  return Catch::Session().run(argc, argv);
}
