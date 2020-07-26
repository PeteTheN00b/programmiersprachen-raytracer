#ifndef BOX_HPP
#define BOX_HPP

#include "shape.hpp"

enum class Dim
{
	posX,
	negX,
	posY,
	negY,
	posZ,
	negZ
};

class Box : public Shape
{
public:
	Box(std::string const& name, Material const& m, glm::vec3 const& origin, float w, float h, float l);

	float area() const override;
	float volume() const override;

	void intersectPlane(HitPoint& h, Ray const& r, Dim plane) const; //function to be called by box's intersect for the purpose of simplifying checking the individual planes
	HitPoint intersect(Ray const& r) const override;

	std::ostream& print(std::ostream& os) const override;

protected:
	glm::vec3 origin_;

	float width_; //x
	float height_; //y
	float length_; //z
};

#endif