#include "shape.hpp"

Shape::Shape(std::string const& name, Material const& m) :
	name_{ name },
	m_{m} {std::cout << "Shape ctor" << std::endl; }

Shape::~Shape()
{
	std::cout << "Shape dtor" << std::endl;
}

std::ostream& Shape::print(std::ostream& os) const
{
	os << "Name: " << name_ << std::endl
		<< "Color: " << m_.color;
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