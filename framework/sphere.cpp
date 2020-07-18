#include "sphere.hpp"

#define _USE_MATH_DEFINES
#include <math.h>


Sphere::Sphere(std::string name, Color color, glm::vec3 centre, float radius) :
	Shape(name, color),
	centre_{centre},
	radius_{radius} {}

float Sphere::area() const
{
	return 4.f * M_PI * pow(radius_, 2);
}

float Sphere::volume() const
{
	return 4.f / 3.f * M_PI * pow(radius_, 3);
}