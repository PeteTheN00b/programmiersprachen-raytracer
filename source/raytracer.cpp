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

Ray transformRay(glm::mat4 const& mat, Ray const& ray)
{
    glm::vec4 p4{ ray.origin.x, ray.origin.y, ray.origin.z, 1.f };
    glm::vec4 d4{ ray.direction.x, ray.direction.y, ray.direction.z, 0.f };

    p4 = glm::inverse(mat) * p4;
    d4 = glm::inverse(mat) * d4;

    return{
        {p4.x, p4.y, p4.z},
        {d4.x, d4.y, d4.z}
    };
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

  //camera variables
  glm::vec3 observerLoc{ 0.f, 0.f, 0.f };
  float fovX = 60.f;
  float const fovY = 60.f;

  glm::vec3 dir{ 0.f, 0.f, -1.f }; //-z direction (camera forward)
  glm::vec3 up{ 0.f, 1.f, 0.f }; //y direction


  World world{};

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
              float reflectivity, 
                  refractivity, refractiveIndex; //refractivity of 0 is no refraction, refractivity of 1 means we don't display the object itself, only what is refracted behind it

              in_sstream >> 
              matName >> 
              ambient.r >> ambient.g >> ambient.b >> 
              diffusive.r >> diffusive.g >> diffusive.b >> 
              specular.r >> specular.g >> specular.b >> 
              specularExp >>
              reflectivity >>
              refractivity >> refractiveIndex;

              world.createMaterial(matName, ambient, diffusive, specular, specularExp, 
                  reflectivity, 
                  refractivity, refractiveIndex);
          }
          else if("shape" == identifier)
          {
              in_sstream >> identifier;

              if ("box" == identifier)
              {
                  glm::vec3 p1, p2;
                  std::string name, matName;

                  in_sstream >> name >> p1.x >> p1.y >> p1.z >> p2.x >> p2.y >> p2.z >> matName;

                  world.createBox(name, matName, p1 / 2.f + p2 / 2.f, abs(p2.x - p1.x) / 2.f, abs(p2.y - p1.y) / 2.f, abs(p2.z - p1.z) / 2.f);
              }
              else if ("sphere" == identifier)
              {
                  glm::vec3 p;
                  float r;
                  std::string name, matName;

                  in_sstream >> name >> p.x >> p.y >> p.z >> r >> matName;

                  world.createSphere(name, matName, p, r);
              }
              else if ("composite" == identifier)
              {
                  std::string name;

                  in_sstream >> name;

                  std::shared_ptr<Composite> comp = world.createComposite(name);

                  std::string childName;
                  while (in_sstream >> childName)
                  {
                      comp.get()->addChild(world.findShape(childName));
                  }
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
          else if ("camera" == identifier)
          {
              std::string name;

              in_sstream >> name >> fovX >> observerLoc.x >> observerLoc.y >> observerLoc.z >> dir.x >> dir.y >> dir.z >> up.x >> up.y >> up.z;
              dir = glm::normalize(dir);
              up = glm::normalize(up);
          }
      }
      else if ("transform" == identifier)
      {
          in_sstream >> identifier;
          std::cout << "transform" << std::endl;

          std::shared_ptr<Shape> s = world.findShape(identifier);

          if (nullptr != s)
          {
              in_sstream >> identifier;

              if ("translate" == identifier)
              {
                  glm::vec3 translation;

                  in_sstream >> translation.x >> translation.y >> translation.z;

                  s.get()->translate(translation);
              }
              else if ("rotate" == identifier)
              {
                  float theta;
                  glm::vec3 axis;

                  in_sstream >> theta >> axis.x >> axis.y >> axis.z;

                  s.get()->rotate(theta / 180 * M_PI, axis);
              }
              else if ("scale" == identifier)
              {
                  std::cout << "scale" << std::endl;

                  glm::vec3 scale;

                  in_sstream >> scale.x >> scale.y >> scale.z;

                  s.get()->scale(scale);
              }
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
          Color pColor = { 0.f, 0.f, 0.f };

          float x = ((float)i / (float)image_width - 0.5f) * 2 * (float)tan(fovX / 180.f * M_PI);
          float y = ((float)j / (float)image_width - 0.5f) * 2 * (float)tan(fovY / 180.f * M_PI);

          glm::vec3 const right = glm::cross(dir, up); //x direction

          glm::vec3 direction = x * right + y * up + dir;
          direction = glm::normalize(direction);

          glm::vec3 origin = observerLoc;

          int reflectionCount = 0;
          int maxReflectionCount = 10;
          int refractionCount = 0;

          //lambda for lighting using reflections (had to use std::function because you can't initialize a lambda recursively)
          std::function<void(float, float)> lighting = [&](float reflectivity, float refractivity) //reflectivity of previous material, 1 for first material
          {
              HitPoint closestHit;

              for (std::shared_ptr<Shape> s : world.getShapes())
              {
                  Ray r{ origin, direction };
                  //r = transformRay(s.get()->getWorldTransformation(), r); //transform our ray using the inverse of the shape's world transformation

                  HitPoint h = s.get()->intersect(r);

                  if (h.intersect && (h.dist < closestHit.dist || !closestHit.intersect))
                  {
                      closestHit = h;
                  }
              }

              //if an object was intersected we use that instead
              if (closestHit.intersect)
              {
                  reflectivity *= (1.f - closestHit.objMat.reflectivity); //reduce own material intensity in exchange for adding reflected objects intensity
                  refractivity *= (1.f - closestHit.objMat.refractivity);

                  if (0 == reflectionCount && 0 == refractionCount) pColor = { 0.f, 0.f, 0.f };
                  pColor += closestHit.objMat.ambient * ambientIntensity *
                      reflectivity * refractivity;

                  for (std::shared_ptr<PointLight> l : world.getLights())
                  {
                      int notObstructed = 1; //assume that our light isn't obstructed unless proven otherwise

                      glm::vec3 direction{ {l.get()->origin - closestHit.intersectPoint} };
                      direction = glm::normalize(direction);

                      for (std::shared_ptr<Shape> s : world.getShapes()) //check whether or not our ray is obstructed by the light source
                      {
                          Ray r{ closestHit.intersectPoint + closestHit.objNormal, direction };
                          //r = transformRay(s.get()->getWorldTransformation(), r);

                          if (s.get()->intersect(r).intersect)
                              //adding normal to make sure it doesn't re-intersect with the intersection point
                          {
                              notObstructed = 0;
                              break;
                          }
                      }

                      float dotProduct1 = glm::dot<float>(closestHit.objNormal, direction);
                      if (dotProduct1 < 0.f) dotProduct1 = 0.f;


                      glm::vec3 lightDirection = closestHit.intersectPoint + closestHit.objNormal - l.get()->origin;

                      glm::vec4 lightDir4{ lightDirection.x, lightDirection.y, lightDirection.z, 1.f };
                      glm::vec3 rotAxis = closestHit.objNormal;
                      glm::mat4 rotMat = glm::rotate((float)M_PI, rotAxis); //rotate 180 degrees around the normal
                      glm::vec4 reflectedLightDir4 = -lightDir4 * rotMat;

                      glm::vec3 reflectedLightDirection{reflectedLightDir4};
                      reflectedLightDirection = glm::normalize(reflectedLightDirection);

                      glm::vec3 reverseOrigin = origin - closestHit.intersectPoint;
                      reverseOrigin = glm::normalize(reverseOrigin);

                      float dotProduct2 = pow(abs(glm::dot<float>(reflectedLightDirection, reverseOrigin)), closestHit.objMat.specularExp);
                      if (dotProduct2 < 0.f) dotProduct2 = 0.f;

                      pColor += (closestHit.objMat.diffusive * (float)notObstructed * dotProduct1 + closestHit.objMat.specular * pow(dotProduct2, closestHit.objMat.specularExp)) 
                          * l.get()->color * l.get()->intensity
                          * reflectivity * refractivity;
                  }


                  //NOTE: refraction does not work with multiple reflactive surfaces inside of each other
                  //this is still inside of the intersection if statement, so a hit must have occurred
                  if (closestHit.objMat.refractivity > 0.001f) //if our object is refractive, perform another lighting call using refractive information
                  {
                      std::shared_ptr<Shape> s = world.findShape(closestHit.objName);

                      if (nullptr != s)
                      {
                          //our surface normal and direction should already be normalized
                          glm::vec3 rotAxis = glm::cross(closestHit.objNormal, direction); //we want to rotate around the line which is perpendicular to both our incident ray and surface normal
                          float rotAngle = asin(
                              sin(
                                  acos(glm::dot(closestHit.objNormal, direction)) //the angle between our normal and incident ray
                              )
                              / closestHit.objMat.refractiveIndex
                          );

                          if (refractionCount > 1) std::cout << "Refraction Repetition Fail!" << std::endl;

                          glm::mat4 rotMat = glm::rotate(rotAngle, rotAxis);
                          glm::vec4 norm4{ closestHit.objNormal.x, closestHit.objNormal.y, closestHit.objNormal.z, 1.f };

                          glm::vec4 refractDir4 = -norm4 * rotMat; //rotating the normal instead of the direction
                          glm::vec3 refractDir{ refractDir4 };

                          HitPoint h = s.get()->intersect(Ray{ //attempt to intersect with our shape (should work if it isn't unreasonably thin at this point)
                              closestHit.intersectPoint - closestHit.objNormal * 0.01f, //trace a ray starting inside of our shape
                              refractDir });

                          if (h.intersect)
                          {
                              //save our origin, because we are going to need to revert it back after recursively calling the function
                              glm::vec3 oldOrigin = origin;

                              //we will recursively call our function to add lighting based on what we find upon leaving our surface,
                              //this however renders objects within a refractive object invisible
                              origin = h.intersectPoint + h.objNormal * 0.01f;

                              ++refractionCount;
                              lighting(1.f, h.objMat.refractivity); //currently crashes, conditions cause infinite recursion

                              origin = oldOrigin;
                          }
                          else
                          {
                              pColor += bgColor * reflectivity; //ignore refractivvity since no refraction occurred
                              return;
                          }
                      }
                  }
              }
              else
              {
                  pColor += bgColor * reflectivity * refractivity;
                  return;
              }

              

              //Recursive Reflection if necessary
              if (closestHit.intersect && //only reflect if a hit occurred (technically unnecessary because we return if no intersection occurred, moreso here for clarity, might delete later)
                  closestHit.objMat.reflectivity > 0.001f && //only reflect when necessary
                  reflectionCount < maxReflectionCount) //prevent infinite loops between reflective surfaces
              {
                  //preparation for reflection
                  origin = closestHit.intersectPoint + closestHit.objNormal * 0.01f; //shift origin (add normal to make sure our hit test doesn't collide with the object)

                  glm::mat4 rotMat = glm::rotate((float)M_PI, closestHit.objNormal);
                  glm::vec4 direction4{ direction.x, direction.y, direction.z, 1.f };

                  glm::vec4 newDir4 = -direction4 * rotMat;
                  direction = glm::vec3{ newDir4 }; //reflect direction
                  direction = glm::normalize(direction);

                  ++reflectionCount;
                  lighting(closestHit.objMat.reflectivity, 1.f);
              }
          };

          lighting(1.f, 1.f);

          pColor.r /= (pColor.r + 1.f);
          pColor.g /= (pColor.g + 1.f);
          pColor.b /= (pColor.b + 1.f);

          Pixel p{ i, j, pColor };
          renderer.write(p);
      }

      std::cout << (float)(i + 1) / (float)(image_width) * 100.f << "% completed" << std::endl;
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
