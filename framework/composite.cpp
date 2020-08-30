#include "composite.hpp"

Composite::Composite(std::string const& name) :
	Shape(name, Material{})
{}

std::vector<std::shared_ptr<Shape>> Composite::getChildren()
{
	return children_;
}

std::vector<std::shared_ptr<Shape>> Composite::getAllChildren()
{
	std::vector<std::shared_ptr<Shape>> allChildren = getChildren();

	//for (auto child = children_.begin(); child != children_.end(); ++child)
	for (std::shared_ptr<Shape> child : children_)
	{
		if (typeid(*(child.get())) == typeid(Composite))
		{
			std::shared_ptr<Composite> cChild = std::static_pointer_cast<Composite>(child);
			std::vector<std::shared_ptr<Shape>> cChildren = cChild.get()->getAllChildren();

			allChildren.insert(
				allChildren.end(),
				cChildren.cbegin(),
				cChildren.cend()
			);
		}
	}

	return allChildren;
}

void Composite::addChild(std::shared_ptr<Shape> shape)
{
	if (nullptr == shape) return;

	children_.push_back(shape);
}

std::shared_ptr<Shape> Composite::removeChild(std::string const& name)
{
	for (auto child = children_.begin(); child != children_.end(); ++child)
	{
		if (child->get()->getName() == name)
		{
			std::shared_ptr<Shape> retChild = *child;

			children_.erase(child);

			return retChild;
		}
	}

	return nullptr;
}

float Composite::area() const
{
	float sum = 0;
	for (std::shared_ptr<Shape> child : children_)
	{
		sum += child.get()->area();
	}

	return sum;
}

float Composite::volume() const
{
	float sum = 0;
	for (std::shared_ptr<Shape> child : children_)
	{
		sum += child.get()->volume();
	}

	return sum;
}

HitPoint Composite::intersect(Ray const& r) const
{
	HitPoint closestHit;

	for (std::shared_ptr<Shape> child : children_)
	{
		HitPoint h = child.get()->intersect(r);

		if (h.intersect && (h.dist < closestHit.dist || !closestHit.intersect)) //if the ray intersects, (and is closer than our previous intersection, or we don't have a previous intersection)
			closestHit = h; //make this our new closest intersection
	}

	return closestHit;
}

void Composite::translate(glm::vec3 v)
{
	for (std::shared_ptr<Shape> child : children_)
	{
		child.get()->translate(v);
	}
}

void Composite::rotate(float f, glm::vec3 v)
{
	for (std::shared_ptr<Shape> child : children_)
	{
		child.get()->rotate(f, v);
	}
}

void Composite::scale(glm::vec3 v)
{
	for (std::shared_ptr<Shape> child : children_)
	{
		child.get()->scale(v);
	}
}