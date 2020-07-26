#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "color.hpp"

struct Material
{
	Color color = {};
	float ambient = 0.f;
	float diffusive = 0.f;
	float specular = 0.f;
};

#endif