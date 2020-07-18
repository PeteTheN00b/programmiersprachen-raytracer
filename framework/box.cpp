#include "box.hpp"

Box::Box(std::string name, Color color, float l, float w, float h) :
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