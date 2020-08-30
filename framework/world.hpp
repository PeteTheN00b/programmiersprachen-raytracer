#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include <map>

#include "sphere.hpp"
#include "box.hpp"
#include "composite.hpp"
#include "light.hpp"
#include "material.hpp"

class World
{
public:
	//World();

	void createMaterial(std::string const& matName, Color const& ambient, Color const& diffusive, Color const& specular, int specularExp,
		float reflectivity, float refractivity, float refractiveIndex);
	std::shared_ptr<Material> findMat(std::string const& matName) const;

	void createSphere(std::string name, std::string const& matName, glm::vec3 const& centre, float radius);
	void createBox(std::string name, std::string const& matName, glm::vec3 const& origin, float w, float h, float l);
	std::shared_ptr<Composite> createComposite(std::string name);
	void createLight(glm::vec3 const& origin, Color color, float intensity);

	std::vector<std::shared_ptr<Shape>> getShapes();
	std::shared_ptr<Shape> findShape(std::string name);
	//std::shared_ptr<Composite> findParent(std::string name);

	std::vector<std::shared_ptr<PointLight>> getLights();

	void reparent(std::string const& childName, std::shared_ptr<Composite> oldParent, std::shared_ptr<Composite> newParent); //move child from old parent to new parent
	void reparent(std::shared_ptr<Shape> const& child, std::shared_ptr<Composite> oldParent, std::shared_ptr<Composite> newParent);

	std::shared_ptr<Composite> getRoot();

private:
	//int sphereCount_; //init as 0
	//int boxCount_;

	Composite root_{ "Root" };
	//std::vector<std::shared_ptr<Shape>> shapes_;
	std::vector<std::shared_ptr<PointLight>> lights_;
	std::map<std::string, std::shared_ptr<Material>> mats_; //materials are mapped to colors to make them easy to find
};

#endif