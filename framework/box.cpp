#include "box.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/intersect.hpp>

//definitions to simplify intersection testing
#define rayPointX r.origin.x+r.direction.x*scalar
#define rayPointY r.origin.y+r.direction.y*scalar
#define rayPointZ r.origin.z+r.direction.z*scalar

Box::Box(std::string const& name, Material const& m, glm::vec3 const& origin, float w, float h, float l) :
	Shape(name, m),
	origin_{ origin },
	width_{ w },
	height_{ h },
	length_{ l } {}

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
		h.objMat = m_;
		h.objName = name_;
		h.rayDirection = r.direction; //still not sure as to what this helps with


		glm::vec4 norm4 = glm::transpose(glm::inverse(getWorldTransformation())) * //take transformations into account for the normal calculation
			glm::vec4{ h.objNormal.x, h.objNormal.y, h.objNormal.z, 0.f };
		h.objNormal = glm::vec3{ norm4.x, norm4.y, norm4.z };
		h.objNormal = glm::normalize(h.objNormal);

		glm::vec4 intersect4 = getWorldTransformation() *
			glm::vec4{ h.intersectPoint.x, h.intersectPoint.y, h.intersectPoint.z, 1.f };
		h.intersectPoint = glm::vec3{ intersect4.x, intersect4.y, intersect4.z };
	}
}

HitPoint Box::intersect(Ray const& ray) const //assuming normalized input ray
{
	Ray r = transformRay(ray);
	r.direction = glm::normalize(r.direction);

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