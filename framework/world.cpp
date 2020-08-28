#include "world.hpp"

#include <string>
#include <memory>
#include <utility>

World::World() :
	sphereCount_{0},
	boxCount_{0} {}

void World::createMaterial(std::string const& matName, Color const& ambient, Color const& diffusive, Color const& specular, int specularExp,
	float reflectivity, float refractivity, float refractiveIndex)
{
	mats_[matName] = std::make_shared<Material>(Material{ ambient, diffusive, specular, specularExp,
		reflectivity, refractivity, refractiveIndex });
}

std::shared_ptr<Material> World::findMat(std::string const& matName) const
{
	std::shared_ptr<Material> m;
	for (std::pair<std::string, std::shared_ptr<Material>> mat : mats_)
	{
		if (mat.first == matName) m = mat.second;
	}

	if (nullptr == m.get()) m = std::make_shared<Material>(Material{}); //if we don't currently have a material, create a default material

	return m;
}

void World::createSphere(std::string name, std::string const& matName, glm::vec3 const& centre, float radius)
{
	std::shared_ptr<Material> m = findMat(matName);

	shapes_.push_back(std::make_shared<Sphere>(
		name/*"Sphere" + std::to_string(sphereCount_)*/, *(m.get()),
		centre, radius ));

	sphereCount_++;
}

void World::createBox(std::string name, std::string const& matName, glm::vec3 const& origin, float w, float h, float l)
{
	std::shared_ptr<Material> m = findMat(matName);

	shapes_.push_back(std::make_shared<Box>(
		name/*"Box" + std::to_string(boxCount_)*/, *(m.get()),
		origin, w, h, l ));

	boxCount_++;
}

void World::createLight(glm::vec3 const& origin, Color color, float intensity)
{
	lights_.push_back(std::make_shared<PointLight>(
		PointLight{origin, color, intensity}));
}

std::shared_ptr<Composite> World::createComposite(std::string name)
{
	std::shared_ptr<Composite> comp = std::make_shared<Composite>(Composite{ name });

	shapes_.push_back(comp);

	return comp;
}

std::vector<std::shared_ptr<Shape>> World::getShapes()
{
	return shapes_;
}

std::shared_ptr<Shape> World::findShape(std::string name)
{
	for (std::shared_ptr<Shape> s : shapes_)
	{
		if (s.get()->getName() == name) return s;
	}

	return nullptr; //if we haven't found the shape
}

std::vector<std::shared_ptr<PointLight>> World::getLights()
{
	return lights_;
}