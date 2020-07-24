#include "shape.hpp"

Shape::Shape(std::string name, Color color, float diffusive, float specular) :
	name_{ name },
	color_{ color },
	diffusive_{diffusive},
	specular_{specular} {std::cout << "Shape ctor" << std::endl; }

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

std::string Shape::getName() const
{
	return name_;
}

std::ostream& operator<<(std::ostream& os, Shape const& s)
{
	s.print(os);

	return os;
}