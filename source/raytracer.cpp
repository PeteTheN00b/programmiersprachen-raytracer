#include <renderer.hpp>
#include <window.hpp>

#include <GLFW/glfw3.h>
#include <thread>
#include <utility>
#include <cmath>
#include <fstream>
#include <iostream>

#include "shape.hpp"
#include "sphere.hpp"
#include "box.hpp"
#include "light.hpp"
#include "material.hpp"
#include "world.hpp"

float nextFloatFromString(size_t & i, size_t & nextSpace, std::string const& str) //function used in file reading to act similar to a delimiter for the purpose of finding new floats
{
    for (nextSpace = i + 1; nextSpace < str.size() && " " == &str.at(nextSpace); nextSpace++);
    float f = stof(str.substr(i, nextSpace - 1));
    i = nextSpace;

    return f;
}

//now single threaded again
int main(int argc, char* argv[])
{
  unsigned const image_width = 800;
  unsigned const image_height = 600;
  std::string const filename = "./checkerboard.ppm";

  Renderer renderer{image_width, image_height, filename};

  renderer.render();

  Color const bgColor{0.1f, 0.4f, 0.2f}; //background color
  glm::vec3 const observerLoc{ 0.f, 0.f, 0.f };


  World world{};
  //world.createMaterial("Red", { 0.7f, 0.f, 0.2f }, 0.3f, 0.8f, 0.2f);
  //world.createMaterial("Purple", { 0.7f, 0.1f, 0.9f }, 0.3f, 0.3f, 0.9f);
  //world.createMaterial("Table", { 0.7f, 0.6f, 0.f }, 0.3f, 0.7f, 1.f);
  //world.createMaterial("Cyan", { 0.f, 0.9f, 0.8f }, 0.3f, 1.f, 0.5f);

  ////world.createSphere("Red", { 0.f, 0.f, -150.f }, 50.f);
  //world.createSphere("Purple", { /*-20.f*/0.f, -70.f, -150.f }, 80.f);

  //world.createBox("Table", { 0.f, -140.f, -300.f }, 500.f, 20.f, 500.f);
  ////world.createBox("Cyan", { 150.f, -120.f, -100.f }, 100.f, 40.f, 60.f);

  //world.createLight({ 3000.f, 500.f, -150.f }, 1.f);
  //world.createLight({ -3000.f, 500.f, -150.f }, 1.f);

  std::ifstream freader;
  freader.open("scene.sdf", std::ios::in);
  std::string sdf_contents;
  while (getline(freader, sdf_contents))
      //Notes for tomorrow : material reads the first 2 or 3 floats as part of the name
      //z for spheres doesn't initialize properly
      //boxes don't initialize properly
      //lights have proper x, but improper y and z
  {
      size_t i = 0; //index for what we are looking at in our string

      if (i + 7 < sdf_contents.size() && "define " == sdf_contents.substr(i, 7)) //space is also a symbol
      {
          i += 7;

          if (i + 9 < sdf_contents.size() && "material " == sdf_contents.substr(i, 9)) //format: define material red 1 0 0 1 0 0 1 0 0 1
          {
              i += 9;

              size_t nextSpace;
              for (nextSpace = i; nextSpace < sdf_contents.size() && " " == &sdf_contents.at(nextSpace); nextSpace++); //kind of like a delimiter, find the next space in our string

              std::string matName = sdf_contents.substr(i, nextSpace - 1);

              i += nextSpace;


              //gotta change this later, my diffusive properties don't contain colors, only intensity, the color is separate
              float colorR = nextFloatFromString(i, nextSpace, sdf_contents);
              float colorG = nextFloatFromString(i, nextSpace, sdf_contents);
              float colorB = nextFloatFromString(i, nextSpace, sdf_contents);
              float ambient = nextFloatFromString(i, nextSpace, sdf_contents);
              float diffusive = nextFloatFromString(i, nextSpace, sdf_contents);
              float specular = nextFloatFromString(i, nextSpace, sdf_contents);

              world.createMaterial(matName, { colorR, colorG, colorB }, ambient, diffusive, specular);
          }
          else if (i + 6 < sdf_contents.size() && "shape " == sdf_contents.substr(i, 6))
          {
              i += 6;

              if (i + 4 < sdf_contents.size() && "box " == sdf_contents.substr(i, 4))
              {
                  i += 4;
                  size_t nextSpace = i;

                  float p1x = nextFloatFromString(i, nextSpace, sdf_contents);
                  float p1y = nextFloatFromString(i, nextSpace, sdf_contents);
                  float p1z = nextFloatFromString(i, nextSpace, sdf_contents);
                  float p2x = nextFloatFromString(i, nextSpace, sdf_contents);
                  float p2y = nextFloatFromString(i, nextSpace, sdf_contents);
                  float p2z = nextFloatFromString(i, nextSpace, sdf_contents);

                  glm::vec3 p1{ p1x, p1y, p1z };
                  glm::vec3 p2{ p2x, p2y, p2z };

                  for (nextSpace = i; nextSpace < sdf_contents.size() && " " == &sdf_contents.at(nextSpace); nextSpace++); //kind of like a delimiter, find the next space in our string

                  std::string matName = sdf_contents.substr(i, nextSpace - 1);

                  i += nextSpace;

                  world.createBox(matName, p1 / 2.f + p2 / 2.f, abs(p2x - p1x) / 2.f, abs(p2y - p1y) / 2.f, abs(p2z - p1z) / 2.f);
              }
              else if (i + 7 < sdf_contents.size() && "sphere " == sdf_contents.substr(i, 7))
              {
                  i += 7;
                  size_t nextSpace = i;

                  float px = nextFloatFromString(i, nextSpace, sdf_contents);
                  float py = nextFloatFromString(i, nextSpace, sdf_contents);
                  float pz = nextFloatFromString(i, nextSpace, sdf_contents);
                  float r = nextFloatFromString(i, nextSpace, sdf_contents);

                  glm::vec3 p{ px, py, pz };

                  for (nextSpace = i; nextSpace < sdf_contents.size() && " " == &sdf_contents.at(nextSpace); nextSpace++); //kind of like a delimiter, find the next space in our string

                  std::string matName = sdf_contents.substr(i, nextSpace - 1);

                  i += nextSpace;

                  world.createSphere(matName, p, r);
              }
          }
          else if (i + 6 < sdf_contents.size() && "light " == sdf_contents.substr(i, 6)) //add color later
          {
              i += 6;
              size_t nextSpace = i;

              float px = nextFloatFromString(i, nextSpace, sdf_contents);
              float py = nextFloatFromString(i, nextSpace, sdf_contents);
              float pz = nextFloatFromString(i, nextSpace, sdf_contents);
              float intensity = nextFloatFromString(i, nextSpace, sdf_contents);

              glm::vec3 p{ px, py, pz };

              world.createLight(p, intensity);
          }
      }

      std::cout << sdf_contents << std::endl;
  }

  freader.close();


  float ambientIntensity = 0.5;

  //This is how we write to the renderer
  for (unsigned int i = 0; i < image_width; i++)
  {
      for (unsigned int j = 0; j < image_height; j++)
      {
          HitPoint closestHit;

          for (std::shared_ptr<Shape> s : world.getShapes())
          {
              glm::vec3 direction{ (float)i - (float)image_width / 2.f, (float)j - (float)image_height / 2.f, -100.f };
              direction = glm::normalize(direction);

              HitPoint h = s.get()->intersect(Ray{ observerLoc,  direction});

              if (h.intersect && (h.dist < closestHit.dist || !closestHit.intersect))
              {
                  closestHit = h;
              }
          }

          //by default our pixel has the background color
          Color pColor = bgColor;
          
          //if an object was intersected we use that instead
          if (closestHit.intersect)
          {
              pColor = closestHit.objMat.color;

              float intensity = closestHit.objMat.ambient * ambientIntensity;

              for (std::shared_ptr<PointLight> l : world.getLights())
              {
                  int notObstructed = 1; //assume that our light isn't obstructed unless proven otherwise

                  glm::vec3 direction{ {l.get()->origin - closestHit.intersectPoint} };
                  direction = glm::normalize(direction);

                  for (std::shared_ptr<Shape> s : world.getShapes()) //check whether or not our ray is obstructed by the light source
                  {
                      if (s.get()->intersect(Ray{ closestHit.intersectPoint + closestHit.objNormal, direction }).intersect)
                          //adding normal to make sure it doesn't re-intersect with the intersection point
                      {
                          notObstructed = 0;
                          break;
                      }
                  }

                  float dotProduct1 = abs(glm::dot<float>(closestHit.objNormal, direction));

                  glm::vec3 reflectedLightDirection = closestHit.intersectPoint + closestHit.objNormal - l.get()->origin;
                  reflectedLightDirection *= 2;
                  reflectedLightDirection -= closestHit.intersectPoint;
                  reflectedLightDirection = glm::normalize(reflectedLightDirection);

                  glm::vec3 reverseOrigin = observerLoc - closestHit.intersectPoint;
                  reverseOrigin = glm::normalize(reverseOrigin);
                  float dotProduct2 = pow(abs(glm::dot<float>(reflectedLightDirection, reverseOrigin)), 5);
                  intensity += l.get()->intensity * (closestHit.objMat.diffusive * (float)notObstructed * dotProduct1 + closestHit.objMat.specular * dotProduct2);
              }

              intensity /= (intensity + 1);

              pColor.r *= intensity;
              pColor.g *= intensity;
              pColor.b *= intensity;
          }

          Pixel p{ i, j, pColor };
          renderer.write(p);
      }
  }

  Window window{{image_width, image_height}};

  while (!window.should_close()) {
    if (window.get_key(GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      window.close();
    }
    window.show(renderer.color_buffer());
  }

  return 0;
}
