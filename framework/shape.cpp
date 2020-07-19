#include "shape.hpp"

Shape::Shape(std::string name, Color color) :
	name_{ name },
	color_{ color } {}

std::ostream& Shape::print(std::ostream& os) const
{
	os << "Name: " << name_ << std::endl
		<< "Color: " << color_;
	return os;
}

std::ostream& operator<<(std::ostream& os, Shape const& s)
{
	s.print(os);

	return os;
}