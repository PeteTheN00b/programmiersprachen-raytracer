#ifndef SPHERE_HPP
#define SPHERE_HPP

#include <glm/vec3.hpp>

#include "shape.hpp"

class Sphere : public Shape
{
public:
	Sphere(std::string const& name, Material const& m, glm::vec3 const& centre, float radius);
	~Sphere();

	float area() const override;
	float volume() const override;

	std::ostream& print(std::ostream& os) const override;

	HitPoint intersect(Ray const& r) const override;

protected:
	glm::vec3 centre_;
	float radius_;
};

#endif