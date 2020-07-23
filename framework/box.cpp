#include "box.hpp"

Box::Box(std::string name, Color color, float h, float l, float w) :
	Shape(name, color),
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

HitPoint Box::intersect(Ray const& r) const
{
	HitPoint h;

	return h;
}

std::ostream& Box::print(std::ostream& os) const
{
	Shape::print(os);
	os << "Dimensions: " << height_ << ", " << length_ << ", " << width_ << std::endl;

	return os;
}