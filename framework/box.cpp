#include "box.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/intersect.hpp>

//definitions to simplify intersection testing
#define rayPointX r.origin.x+r.direction.x*scalar
#define rayPointY r.origin.y+r.direction.y*scalar
#define rayPointZ r.origin.z+r.direction.z*scalar

Box::Box(std::string name, Color color, float diffusive, float specular, glm::vec3 origin, float w, float h, float l) :
	Shape(name, color, diffusive, specular),
	origin_{ origin },
	height_{ h },
	length_{ l },
	width_{ w } {}

float Box::area() const
{
	return 2 * length_ * width_ + 2 * width_ * height_ + 2 * length_ * height_;
}

float Box::volume() const
{
	return length_ * width_ * height_;
}

void Box::intersectPlane(HitPoint& h, Ray const& r, Dim plane) const
{
	float const maxX = origin_.x + width_ / 2;
	float const minX = origin_.x - width_ / 2;
	float const maxY = origin_.y + height_ / 2;
	float const minY = origin_.y - height_ / 2;
	float const maxZ = origin_.z + length_ / 2;
	float const minZ = origin_.z - length_ / 2;

	float scalar;
	bool intersect = false; //assume intersection doesn't occur

	switch (plane)
	{
	case Dim::posX:
		scalar = (maxX - r.origin.x) / r.direction.x;
		if (0.f < scalar && scalar < h.dist && minY < rayPointY && rayPointY < maxY && minZ < rayPointZ && rayPointZ < maxZ) //if our scalar were negative our ray would be moving away from this plane
		{
			intersect = true;
			h.objNormal = { 1, 0, 0 };
		}
		break;

	case Dim::negX:
		scalar = (minX - r.origin.x) / r.direction.x;
		if (0.f < scalar && scalar < h.dist && minY < rayPointY && rayPointY < maxY && minZ < rayPointZ && rayPointZ < maxZ)
		{
			intersect = true;
			h.objNormal = { -1, 0, 0 };
		}
		break;

	case Dim::posY:
		scalar = (maxY - r.origin.y) / r.direction.y;
		if (0.f < scalar && scalar < h.dist && minX < rayPointX && rayPointX < maxX && minZ < rayPointZ && rayPointZ < maxZ)
		{
			intersect = true;
			h.objNormal = { 0, 1, 0 };
		}
		break;

	case Dim::negY:
		scalar = (minY - r.origin.y) / r.direction.y;
		if (0.f < scalar && scalar < h.dist && minX < rayPointX && rayPointX < maxX && minZ < rayPointZ && rayPointZ < maxZ)
		{
			intersect = true;
			h.objNormal = { 0, -1, 0 };
		}
		break;

	case Dim::posZ:
		scalar = (maxZ - r.origin.z) / r.direction.z;
		if (0.f < scalar && scalar < h.dist && minX < rayPointX && rayPointX < maxX && minY < rayPointY && rayPointY < maxY)
		{
			intersect = true;
			h.objNormal = { 0, 0, 1 };
		}
		break;

	case Dim::negZ:
		scalar = (minZ - r.origin.z) / r.direction.z;
		if (0.f < scalar && scalar < h.dist && minX < rayPointX && rayPointX < maxX && minY < rayPointY && rayPointY < maxY)
		{
			intersect = true;
			h.objNormal = { 0, 0, -1 };
		}
		break;

	default:
		break;
	}

	if (intersect) //handles general functionality when interseciton occurs
	{
		h.dist = scalar;
		h.intersect = true;
		h.intersectPoint = { rayPointX, rayPointY, rayPointZ };
		h.objColor = color_;
		h.objName = name_;
		h.rayDirection = r.direction; //still not sure as to what this helps with
	}
}

HitPoint Box::intersect(Ray const& r) const //assuming normalized input ray
{
	HitPoint h;
	h.dist = FLT_MAX; //necessary as many checks are skipped if the current chick returns a smaller distance

	intersectPlane(h, r, Dim::posX); //hand
	intersectPlane(h, r, Dim::negX);
	intersectPlane(h, r, Dim::posY);
	intersectPlane(h, r, Dim::negY);
	intersectPlane(h, r, Dim::posZ);
	intersectPlane(h, r, Dim::negZ);

	return h;
}

std::ostream& Box::print(std::ostream& os) const
{
	Shape::print(os);
	os << "Dimensions: " << height_ << ", " << length_ << ", " << width_ << std::endl;

	return os;
}