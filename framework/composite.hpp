#ifndef COMPOSITE_HPP
#define COMPOSITE_HPP

#include "shape.hpp"

#include <vector>

class Composite : public Shape
{
public:
	Composite(std::string const& name);

	std::vector<std::shared_ptr<Shape>> getChildren();
	std::vector<std::shared_ptr<Shape>> getAllChildren();
	void addChild(std::shared_ptr<Shape>);
	std::shared_ptr<Shape> removeChild(std::string const& name);

	float area() const override;
	float volume() const override;

	HitPoint intersect(Ray const& r) const override;

	void translate(glm::vec3) override; //transform the composite's children rather than the composite itself
	void rotate(float, glm::vec3) override;
	void scale(glm::vec3) override;

protected:
	std::vector<std::shared_ptr<Shape>> children_;
};

#endif