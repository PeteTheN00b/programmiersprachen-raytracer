#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include <map>

#include "sphere.hpp"
#include "box.hpp"
#include "light.hpp"
#include "material.hpp"

class World
{
public:
	World();

	void createMaterial(std::string const& matName, Color const& ambient, Color const& diffusive, Color const& specular, int specularExp,
		float reflectivity, float refractivity, float refractiveIndex);
	std::shared_ptr<Material> findMat(std::string const& matName) const;

	void createSphere(std::string name, std::string const& matName, glm::vec3 const& centre, float radius);
	void createBox(std::string name, std::string const& matName, glm::vec3 const& origin, float w, float h, float l);
	void createLight(glm::vec3 const& origin, Color color, float intensity);

	std::vector<std::shared_ptr<Shape>> getShapes();
	std::shared_ptr<Shape> findShape(std::string name);

	std::vector<std::shared_ptr<PointLight>> getLights();

private:
	int sphereCount_; //init as 0
	int boxCount_;

	std::vector<std::shared_ptr<Shape>> shapes_;
	std::vector<std::shared_ptr<PointLight>> lights_;
	std::map<std::string, std::shared_ptr<Material>> mats_; //materials are mapped to colors to make them easy to find
};

#endif