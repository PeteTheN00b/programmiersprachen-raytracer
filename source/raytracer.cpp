#include <renderer.hpp>
#include <window.hpp>

#include <GLFW/glfw3.h>
#include <thread>
#include <utility>
#include <cmath>
#include <sstream>
#include <fstream>
#include <iostream>

#define _USE_MATH_DEFINES
#include <math.h>
#include <functional>
#include <glm/gtx/transform.hpp> //used to create basic rotation matrices

#include "shape.hpp"
#include "sphere.hpp"
#include "box.hpp"
#include "light.hpp"
#include "material.hpp"
#include "world.hpp"

Color operator*(Color const& color, float f) //the color struct that was given didn't have an overload for multiplication, so I made one here
{
    return { color.r * f, color.g * f, color.b * f };
}

Color operator*(Color const& color, Color const& color2) //the color struct that was given didn't have an overload for multiplication, so I made one here
{
    return { color.r * color2.r, color.g * color2.g, color.b * color2.b };
}

Color operator*=(Color& color, float f)
{
    color = color * f;
    return color;
}

//specular is being initialized with centre point values? specularExp is bugged?

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
  /*world.createMaterial("Red", { 0.7f, 0.f, 0.2f }, { 0.7f, 0.f, 0.2f }, { 0.7f, 0.f, 0.2f }, 50);
  world.createMaterial("Purple", { 0.7f, 0.1f, 0.9f }, { 0.7f, 0.1f, 0.9f }, { 0.7f, 0.1f, 0.9f }, 50);
  world.createMaterial("Table", { 0.7f, 0.6f, 0.f }, { 0.7f, 0.6f, 0.f }, { 0.7f, 0.6f, 0.f }, 50);
  world.createMaterial("Cyan", { 0.f, 0.9f, 0.8f }, { 0.f, 0.9f, 0.8f }, { 0.f, 0.9f, 0.8f }, 50);

  world.createSphere("Red", { 0.f, 0.f, -150.f }, 50.f);
  world.createSphere("Purple", { -20.f, -70.f, -150.f }, 80.f);

  world.createBox("Table", { 0.f, -140.f, -300.f }, 500.f, 20.f, 500.f);
  world.createBox("Cyan", { 150.f, -120.f, -100.f }, 100.f, 40.f, 60.f);

  world.createLight({ 3000.f, 500.f, -150.f }, 1.f);
  world.createLight({ -3000.f, 500.f, -150.f }, 1.f);*/

  std::ifstream freader;
  freader.open("scene.sdf", std::ios::in);
  std::string sdf_contents;

  while (getline(freader, sdf_contents))
  {
      if (sdf_contents.size() == 0 || '#' == sdf_contents.at(0)) continue;

      std::istringstream in_sstream(sdf_contents);
      std::string identifier;
      in_sstream >> identifier;

      if ("define" == identifier)
      {
          in_sstream >> identifier;

          if ("material" == identifier)
          {
              std::string matName;
              Color ambient, diffusive, specular;
              int specularExp;

              in_sstream >> 
              matName >> 
              ambient.r >> ambient.g >> ambient.b >> 
              diffusive.r >> diffusive.g >> diffusive.b >> 
              specular.r >> specular.g >> specular.b >> 
              specularExp;

              world.createMaterial(matName, ambient, diffusive, specular, specularExp);
          }
          else if("shape" == identifier)
          {
              in_sstream >> identifier;

              if ("box" == identifier)
              {
                  glm::vec3 p1, p2;
                  std::string matName;

                  in_sstream >> p1.x >> p1.y >> p1.z >> p2.x >> p2.y >> p2.z >> matName;

                  world.createBox(matName, p1 / 2.f + p2 / 2.f, abs(p2.x - p1.x) / 2.f, abs(p2.y - p1.y) / 2.f, abs(p2.z - p1.z) / 2.f);
              }
              else if ("sphere" == identifier)
              {
                  glm::vec3 p;
                  float r;
                  std::string matName;

                  in_sstream >> p.x >> p.y >> p.z >> r >> matName;

                  world.createSphere(matName, p, r);
              }

          }
          else if ("light" == identifier)
          {
              glm::vec3 p;
              Color color;
              float intensity;

              in_sstream >> p.x >> p.y >> p.z >> color.r >> color.g >> color.b >> intensity;

              world.createLight(p, color, intensity);
          }
      }
  }

  freader.close();


  float ambientIntensity = 0.5;

  //This is how we write to the renderer
  for (unsigned int i = 0; i < image_width; i++)
  {
      for (unsigned int j = 0; j < image_height; j++)
      {
          //by default our pixel has the background color
          Color pColor = bgColor;

          float const fovX = 80.f;
          float const fovY = 80.f;
          float x = ((float)i / (float)image_width - 0.5f) * 2 * (float)tan(fovX / 180.f * M_PI);
          float y = ((float)j / (float)image_width - 0.5f) * 2 * (float)tan(fovY / 180.f * M_PI);

          glm::vec3 const dir{ 0.f, 0.f, -1.f }; //-z direction (camera forward)
          glm::vec3 const up{ 0.f, 1.f, 0.f }; //y direction
          glm::vec3 const right = glm::cross(dir, up); //x direction

          glm::vec3 direction = x * right + y * up + dir;

          glm::vec3 origin = observerLoc;

          int reflectionCount = 0;
          int maxReflectionCount = 0;

          //lambda for lighting using reflections (had to use std::function because you can't initialize a lambda recursively)
          std::function<void()> lighting = [&]()
          {
              HitPoint closestHit;

              for (std::shared_ptr<Shape> s : world.getShapes())
              {
                  direction = glm::normalize(direction);

                  HitPoint h = s.get()->intersect(Ray{ origin,  direction }); //CAUSES CRASHES, unrelated to filereading

                  if (h.intersect && (h.dist < closestHit.dist || !closestHit.intersect))
                  {
                      closestHit = h;
                  }
              }

              //if an object was intersected we use that instead
              if (closestHit.intersect)
              {
                  if (0 == reflectionCount) pColor = { 0.f, 0.f, 0.f };
                  pColor += closestHit.objMat.ambient * ambientIntensity;

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

                      float dotProduct1 = glm::dot<float>(closestHit.objNormal, direction);
                      if (dotProduct1 < 0.f) dotProduct1 = 0.f;


                      glm::vec3 lightDirection = closestHit.intersectPoint + closestHit.objNormal - l.get()->origin;

                      glm::vec4 lightDir4{ lightDirection.x, lightDirection.y, lightDirection.z, 0.f };
                      glm::vec3 rotAxis = closestHit.objNormal;
                      glm::mat4 rotMat = glm::rotate((float)M_PI, rotAxis); //rotate 180 degrees around the normal
                      glm::vec4 reflectedLightDir4 = -lightDir4 * rotMat;

                      glm::vec3 reflectedLightDirection{ reflectedLightDir4.w, reflectedLightDir4.x, reflectedLightDir4.y };
                      reflectedLightDirection = glm::normalize(reflectedLightDirection);

                      glm::vec3 reverseOrigin = origin - closestHit.intersectPoint;
                      reverseOrigin = glm::normalize(reverseOrigin);

                      float dotProduct2 = pow(abs(glm::dot<float>(reflectedLightDirection, reverseOrigin)), closestHit.objMat.specularExp);
                     // if (dotProduct2 < 0.f) dotProduct2 = 0.f;

                      pColor += (closestHit.objMat.diffusive * (float)notObstructed * dotProduct1 + closestHit.objMat.specular * pow(dotProduct2, closestHit.objMat.specularExp)) 
                          * l.get()->color * l.get()->intensity;
                  }
              }

              //preparation for reflection
              origin = closestHit.intersectPoint; //shift origin

              //Thanks to https://stackoverflow.com/questions/34366655/glm-make-rotation-matrix-from-vec3 for providing basic instructions on creating rotation matrices
              glm::vec3 rotAxis = glm::cross(closestHit.objNormal, direction);
              float rotAngle = acos(glm::dot(closestHit.objNormal, direction) / (glm::length(closestHit.objNormal) * glm::length(direction)));
              glm::mat4 rotMat = glm::rotate(rotAngle, rotAxis);
              glm::vec4 direction4{ direction.x, direction.y, direction.z, 0.f };

              glm::vec4 newDir4 = direction4 * rotMat;
              direction = { newDir4.w, newDir4.x, newDir4.y}; //reflect direction

              if (closestHit.intersect && reflectionCount < maxReflectionCount)
              {
                  ++reflectionCount;
                  lighting();
              }
          };

          lighting();

          pColor.r /= (pColor.r + 1.f);
          pColor.g /= (pColor.g + 1.f);
          pColor.b /= (pColor.b + 1.f);

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
