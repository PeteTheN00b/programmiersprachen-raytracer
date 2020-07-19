#ifndef SPHERE_HPP
#define SPHERE_HPP

#include <glm/vec3.hpp>

#include "shape.hpp"
#include "ray.hpp"
#include "hitpoint.hpp"

class Sphere : public Shape
{
public:
	Sphere(std::string name, Color color, glm::vec3 centre, float radius);
	~Sphere();

	float area() const override;
	float volume() const override;

	std::ostream& print(std::ostream& os) const override;

	HitPoint intersect(Ray r) const;

protected:
	glm::vec3 centre_;
	float radius_;
};

#endif