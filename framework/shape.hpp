#ifndef SHAPE_HPP
#define SHAPE_HPP

#include <string>

#include "color.hpp"
#include "ray.hpp"
#include "hitpoint.hpp"

class Shape
{
public:
	Shape(std::string name, Color color);
	~Shape(); //if it is virtual, a shape pointer to a sphere object can call the destructor of a sphere, if it isn't it will only call the shape destructor
		//more generally speaking, if a parent class' destructor is virtual, then a pointer to objects of said parent class' type, will first call the derived class' destructor when possible

	virtual float area() const = 0;
	virtual float volume() const = 0;

	virtual HitPoint intersect(Ray const& r) const = 0;

	virtual std::ostream& print(std::ostream& os) const;

	std::string getName() const;

protected:
	std::string name_;
	Color color_;
};

std::ostream& operator<<(std::ostream& os, Shape const& s);

#endif