#include "shape.hpp"

Shape::Shape(std::string const& name, Material const& m) :
	name_{ name },
	m_{ m },
	world_translation_{ glm::mat4(1.0f) }, //initialize transformations as identity matrices
	world_rotation_{ glm::mat4(1.0f) },
	world_scale_{ glm::mat4(1.0f) }
{}

Shape::~Shape()
{
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

glm::mat4 Shape::getWorldTransformation() const
{
	return world_translation_ * world_rotation_ * world_scale_;
}

void Shape::translate(glm::vec3 v)
{
	glm::mat4 translate = glm::mat4();

	translate[3][0] = v.x;
	translate[3][1] = v.y;
	translate[3][2] = v.z;

	world_translation_ *= translate;
}

void Shape::rotate(float angle, glm::vec3 axis)
{
	glm::mat4 rotate = glm::rotate(angle, axis);

	world_rotation_ *= rotate;
}

void Shape::rotateX(float theta)
{
	glm::mat4 rotate = glm::mat4();

	rotate[1][1] = cos(theta);
	rotate[1][2] = -sin(theta);
	rotate[2][1] = sin(theta);
	rotate[2][2] = cos(theta);

	world_rotation_ *= rotate;
}

void Shape::rotateY(float theta)
{
	glm::mat4 rotate = glm::mat4(1.0f);

	rotate[0][0] = cos(theta);
	rotate[0][2] = sin(theta);
	rotate[2][0] = -sin(theta);
	rotate[2][2] = cos(theta);

	world_rotation_ *= rotate;
}

void Shape::rotateZ(float theta)
{
	glm::mat4 rotate = glm::mat4(1.0f);

	rotate[0][0] = cos(theta);
	rotate[0][1] = -sin(theta);
	rotate[1][0] = sin(theta);
	rotate[1][1] = cos(theta);

	world_rotation_ *= rotate;
}

void Shape::scale(glm::vec3 v)
{
	glm::mat4 scale = /*glm::scale(v);*/glm::mat4();

	scale[0][0] = v.x;
	scale[1][1] = v.y;
	scale[2][2] = v.z;

	world_scale_ *= scale;
}

Ray Shape::transformRay(Ray const& ray) const
{
	glm::vec4 p4{ ray.origin.x, ray.origin.y, ray.origin.z, 1.f };
	glm::vec4 d4{ ray.direction.x, ray.direction.y, ray.direction.z, 0.f };

	p4 = glm::inverse(getWorldTransformation()) * p4;
	d4 = glm::inverse(getWorldTransformation()) * d4;

	return{
		{p4.x, p4.y, p4.z},
		{d4.x, d4.y, d4.z}
	};
}