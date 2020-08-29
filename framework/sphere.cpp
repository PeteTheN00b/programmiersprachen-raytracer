#include "sphere.hpp"

#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtx/intersect.hpp>


Sphere::Sphere(std::string const& name, Material const& m, glm::vec3 const& centre, float radius) :
	Shape(name, m),
	centre_{centre},
	radius_{radius} {}

Sphere::~Sphere()
{
}

float Sphere::area() const
{
	return 4.f * M_PI * pow(radius_, 2);
}

float Sphere::volume() const
{
	return 4.f / 3.f * M_PI * pow(radius_, 3);
}

HitPoint Sphere::intersect(Ray const& ray) const
{
	Ray r = transformRay(ray);
	r.direction = glm::normalize(r.direction);

	HitPoint h;
	glm::vec3 iNormal;
	h.intersect = glm::intersectRaySphere<glm::vec3>(r.origin, r.direction, centre_, radius_, h.intersectPoint, iNormal);

	if (h.intersect)
	{
		glm::vec3 distVec = h.intersectPoint - r.origin;

		h.dist = sqrt(pow(distVec.x, 2) + pow(distVec.y, 2) + pow(distVec.z, 2));
		h.objName = name_;
		h.objMat = m_;
		h.rayDirection = r.direction;

		h.objNormal = h.intersectPoint - centre_;
		glm::vec4 norm4 = glm::transpose(glm::inverse(getWorldTransformation())) * //take transformations into account for the normal calculation
			glm::vec4{ h.objNormal.x, h.objNormal.y, h.objNormal.z, 0.f };
		h.objNormal = glm::vec3{ norm4.x, norm4.y, norm4.z };
		h.objNormal = glm::normalize(h.objNormal);

		glm::vec4 intersect4 = getWorldTransformation() *
			glm::vec4{ h.intersectPoint.x, h.intersectPoint.y, h.intersectPoint.z, 1.f };
		h.intersectPoint = glm::vec3{ intersect4.x, intersect4.y, intersect4.z };
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