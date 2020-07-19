#define CATCH_CONFIG_RUNNER
#include <catch.hpp>
#define _USE_MATH_DEFINES //for pi
#include <math.h>

#include "sphere.hpp"
#include "box.hpp"

# include <glm/glm.hpp>
# include <glm/gtx/intersect.hpp>
TEST_CASE("intersect_ray_sphere", "[intersect]")
{
    // Ray
    glm::vec3 ray_origin{ 0.0f, 0.0f, 0.0f };
    // ray direction has to be normalized !
    // you can use :
    // v = glm :: normalize ( some_vector )
    glm::vec3 ray_direction{ 0.0f, 0.0f, 1.0f };
    // Sphere
    glm::vec3 sphere_center{ 0.0f ,0.0f, 5.0f };
    float sphere_radius{ 1.0f };
    float distance = 0.0f;
    auto result = glm::intersectRaySphere(
        ray_origin, ray_direction,
        sphere_center,
        sphere_radius * sphere_radius, // squared radius !!!
        distance);
    REQUIRE(distance == Approx(4.0f));
}

TEST_CASE("intersect function")
{
    SECTION("Ray moves in the x direction, intersects sphere at the centre")
    {
        Ray r{ {0, 0, 0}, {1, 0, 0} };
        Sphere s{ "TestSphere", {1, 0, 0}, {50, 0, 0}, 1.f };

        HitPoint h = s.intersect(r);

        REQUIRE(h.intersect);
        REQUIRE(Approx(49.f) == h.dist);
        REQUIRE("TestSphere" == h.objName);
        REQUIRE(Approx(1.f) == h.objColor.r);
        REQUIRE((Approx(49.f) == h.intersectPoint.x && Approx(0.f) == h.intersectPoint.y && Approx(0.f) == h.intersectPoint.z));
        REQUIRE(h.rayDirection == r.direction);
    }

    SECTION("Ray moves away from sphere, no intersection occurs")
    {
        Ray r{ {0, 0, 0}, {-1, 0, 0} };
        Sphere s{ "TestSphere", {1, 0, 0}, {50, 0, 0}, 1.f };

        HitPoint h = s.intersect(r);

        REQUIRE(!h.intersect);
    }

    SECTION("Ray intersects at tangent of sphere")
    {
        Ray r{ {0, 1, 0}, {1, 0, 0} };
        Sphere s{ "TestSphere", {1, 0, 0}, {50, 0, 0}, 1.f };

        HitPoint h = s.intersect(r);

        REQUIRE(h.intersect);
        REQUIRE(Approx(50.f) == h.dist);
        REQUIRE((Approx(50.f) == h.intersectPoint.x && Approx(1.f) == h.intersectPoint.y && Approx(0.f) == h.intersectPoint.z));
    }

    SECTION("Ray starts inside of sphere, appears to intersect at the circle's surface")
        //testing confirms peculiar interaction, an intersection is only detected on the outside of the circle, not on the inside
    {

        Ray r{ {0, 0, 0}, {1, 0, 0} };
        Sphere s{ "TestSphere", {1, 0, 0}, {0, 0, 0}, 5.f };

        HitPoint h = s.intersect(r);

        REQUIRE(h.intersect);
        REQUIRE(Approx(5.f) == h.dist);
    }

    SECTION("Ray intersects with diagonal movement")
    {
        Ray r{ {0, 0, 0}, {1, 2, 3} };
        Sphere s{ "TestSphere", {1, 0, 0}, {3, 6, 9}, 2.0f };

        HitPoint h = s.intersect(r);

        REQUIRE(h.intersect);
    }
}

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
