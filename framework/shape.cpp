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
		<< "Ambient Color: " << m_.ambient << std::endl
		<< "Diffusive Color: " << m_.diffusive<< std::endl
		<< "Specular Color: " << m_.specular << ", " << m_.specularExp << std::endl;
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