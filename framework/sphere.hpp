#ifndef SPHERE_HPP
#define SPHERE_HPP

#include <glm/vec3.hpp>

#include "shape.hpp"

class Sphere : public Shape
{
public:
	Sphere(std::string name, Color color, glm::vec3 centre, float radius);

	float area() const override;
	float volume() const override;

protected:
	glm::vec3 centre_;
	float radius_;
};

#endif