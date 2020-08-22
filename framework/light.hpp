#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <glm/glm.hpp>

struct PointLight
{
	glm::vec3 origin = { 0.f, 0.f, 0.f };
	Color color = { 1.f, 1.f, 1.f };
	float intensity = 1;
};

#endif