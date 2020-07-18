#ifndef BOX_HPP
#define BOX_HPP

#include "shape.hpp"

class Box : public Shape
{
public:
	Box(float h, float l, float w);

	float area() const override;
	float volume() const override;

protected:
	float height_;
	float length_;
	float width_;
};

#endif