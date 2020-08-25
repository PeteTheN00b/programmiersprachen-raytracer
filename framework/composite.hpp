#ifndef COMPOSITE_HPP
#define COMPOSITE_HPP

#include "shape.hpp"

#include <vector>

class Composite : public Shape
{
public:
	Composite(std::string const& name, Material const& m);
	void addChild(std::shared_ptr<Shape>);

	float area() const override;
	float volume() const override;

	HitPoint intersect(Ray const& r) const override;

protected:
	std::vector<std::shared_ptr<Shape>> children;
};

#endif