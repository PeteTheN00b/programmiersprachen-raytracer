#include "shape.hpp"

Shape::Shape(std::string name, Color color) :
	name_{ name },
	color_{ color } {std::cout << "Shape ctor" << std::endl; }

Shape::~Shape()
{
	std::cout << "Shape dtor" << std::endl;
}

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