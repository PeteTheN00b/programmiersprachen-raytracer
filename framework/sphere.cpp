#include "sphere.hpp"

#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtx/intersect.hpp>


Sphere::Sphere(std::string name, Color color, float diffusive, float specular, glm::vec3 centre, float radius) :
	Shape(name, color, diffusive, specular),
	centre_{centre},
	radius_{radius} {std::cout << "Sphere ctor" << std::endl; }

Sphere::~Sphere()
{
	std::cout << "Sphere dtor" << std::endl;
}

float Sphere::area() const
{
	return 4.f * M_PI * pow(radius_, 2);
}

float Sphere::volume() const
{
	return 4.f / 3.f * M_PI * pow(radius_, 3);
}

HitPoint Sphere::intersect(Ray const& r) const
{
	HitPoint h;
	glm::vec3 iNormal;
	h.intersect = glm::intersectRaySphere<glm::vec3>(r.origin, r.direction, centre_, radius_, h.intersectPoint, iNormal);

	if (h.intersect)
	{
		glm::vec3 distVec = h.intersectPoint - r.origin;

		h.dist = sqrt(pow(distVec.x, 2) + pow(distVec.y, 2) + pow(distVec.z, 2));
		h.objName = name_;
		h.objColor = color_;
		h.rayDirection = r.direction;

		h.objDiffusion = diffusive_;
		h.objSpecular = specular_;

		h.objNormal = h.intersectPoint - centre_;
		h.objNormal = glm::normalize(h.objNormal);
	}

	return h;
}

std::ostream& Sphere::print(std::ostream& os) const
{
	Shape::print(os);
	os << "Centre: " << centre_.x << ", " << centre_.y << ", " << centre_.z << std::endl
		<< "Radius: " << radius_ << std::endl;

	return os;
}