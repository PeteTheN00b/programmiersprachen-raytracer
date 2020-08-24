#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "color.hpp"

struct Material
{
	Color ambient;
	Color diffusive;
	Color specular;
	int specularExp;

	float reflectivity; //multiplier to reflect, between 0 and 1
	float refractivity; //multiplier to refract, between 0 and 1
	float refractiveIndex;
};

#endif