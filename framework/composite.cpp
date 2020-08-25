#include "composite.hpp"

Composite::Composite(std::string const& name, Material const& m) :
	Shape(name, m)
{}

void Composite::addChild(std::shared_ptr<Shape> shape)
{
	children.push_back(shape);
}

float Composite::area() const
{
	float sum = 0;
	for (std::shared_ptr<Shape> child : children)
	{
		sum += child.get()->area();
	}

	return sum;
}

float Composite::volume() const
{
	float sum = 0;
	for (std::shared_ptr<Shape> child : children)
	{
		sum += child.get()->volume();
	}

	return sum;
}

HitPoint Composite::intersect(Ray const& r) const
{
	HitPoint closestHit;

	for (std::shared_ptr<Shape> child : children)
	{
		HitPoint h = child.get()->intersect(r);

		if (h.intersect && (h.dist < closestHit.dist || !closestHit.intersect)) //if the ray intersects, (and is closer than our previous intersection, or we don't have a previous intersection)
			closestHit = h; //make this our new closest intersection
	}

	return closestHit;
}