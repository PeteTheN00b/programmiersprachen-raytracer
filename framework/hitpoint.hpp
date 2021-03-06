#ifndef HITPOINT_HPP
#define HITPOINT_HPP

#include <string>

#include "material.hpp"
#include "color.hpp"

struct HitPoint
{
	bool intersect = false;
	float dist = 0.f; //distance ray travelled from origin point
	std::string objName = "None"; //name of the object that was hit
	Material objMat; //material of the object that was hit
	glm::vec3 intersectPoint = { 0.f, 0.f, 0.f }; //point of intersection between object and ray
	glm::vec3 rayDirection = { 0.f, 0.f, 1.f }; //direction the colliding ray was travelling in
	glm::vec3 objNormal = { 0.f, 0.f, 1.f };
};

#endif