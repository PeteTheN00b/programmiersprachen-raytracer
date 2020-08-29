#ifndef SHAPE_HPP
#define SHAPE_HPP

#include <string>
#include <glm/gtx/transform.hpp>

#include "color.hpp"
#include "ray.hpp"
#include "hitpoint.hpp"
#include "material.hpp"

class Shape
{
public:
	Shape(std::string const& name, Material const& m);
	~Shape(); //if it is virtual, a shape pointer to a sphere object can call the destructor of a sphere, if it isn't it will only call the shape destructor
		//more generally speaking, if a parent class' destructor is virtual, then a pointer to objects of said parent class' type, will first call the derived class' destructor when possible

	virtual float area() const = 0;
	virtual float volume() const = 0;

	virtual HitPoint intersect(Ray const& r) const = 0;

	virtual std::ostream& print(std::ostream& os) const;

	std::string getName() const;

	glm::mat4 getWorldTransformation() const;
	virtual void translate(glm::vec3);
	virtual void rotate(float, glm::vec3);
	virtual void rotateX(float);
	virtual void rotateY(float);
	virtual void rotateZ(float);
	virtual void scale(glm::vec3);

	Ray transformRay(Ray const& r) const;

protected:
	std::string name_;

	Material m_;

	glm::mat4 world_translation_; //using these instead of a single variable to allow for transformations to be described in any order
	glm::mat4 world_rotation_;
	glm::mat4 world_scale_;
};

std::ostream& operator<<(std::ostream& os, Shape const& s);

#endif