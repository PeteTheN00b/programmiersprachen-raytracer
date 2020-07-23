#ifndef BOX_HPP
#define BOX_HPP

#include "shape.hpp"

class Box : public Shape
{
public:
	Box(std::string name, Color color, float h, float l, float w);

	float area() const override;
	float volume() const override;

	HitPoint intersect(Ray const& r) const override;

	std::ostream& print(std::ostream& os) const override;

protected:
	float height_;
	float length_;
	float width_;
};

#endif