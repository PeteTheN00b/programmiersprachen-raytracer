#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "color.hpp"

struct Material
{
	Color ambient;
	Color diffusive;
	Color specular;
	int specularExp;
};

#endif