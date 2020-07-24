#include <renderer.hpp>
#include <window.hpp>

#include <GLFW/glfw3.h>
#include <thread>
#include <utility>
#include <cmath>

#include "shape.hpp"
#include "sphere.hpp"
#include "box.hpp"
#include "light.hpp"

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

  std::vector<Shape*> world;
  world.push_back(new Sphere{ "Sphere1", Color{0.7f, 0.f, 0.2f}, 0.7f, 0.7f, {0.f, 0.f, -150.f}, 50 });
  world.push_back(new Sphere{ "Sphere2", Color{0.7f, 0.1f, 0.9f}, 0.7f, 0.7f, {-20.f, -70.f, -150.f}, 80 });
  world.push_back(new Box{ "Box1", Color{0.7f, 0.6f, 0.f}, 0.7f, 0.7f, {50, -140, -300}, 500, 20, 500});
  world.push_back(new Box{ "Box1", Color{0.f, 0.9f, 0.8f}, 0.7f, 0.7f, {150, -120, -100}, 100, 40, 60 });

  float const k_a = 0.5f; //reflectivity of ambient light
  float const I_a = 0.3f; //intensity of ambient light

  std::vector<PointLight*> lights;
  lights.push_back(new PointLight{ {3000.f, 500.f, 0.f}, 1.f });
  lights.push_back(new PointLight{ {-3000.f, 500.f, 0.f}, 0.5f });

  //This is how we write to the renderer
  for (unsigned int i = 0; i < image_width; i++)
  {
      for (unsigned int j = 0; j < image_height; j++)
      {
          HitPoint closestHit;

          for (Shape* s : world)
          {
              glm::vec3 direction{ (float)i - (float)image_width / 2.f, (float)j - (float)image_height / 2.f, -100.f };
              direction = glm::normalize(direction);

              HitPoint h = s->intersect(Ray{ observerLoc,  direction});

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
              pColor = closestHit.objColor;

              float intensity = k_a * I_a;

              for (PointLight* l : lights)
              {
                  int notObstructed = 1; //assume that our light isn't obstructed unless proven otherwise

                  glm::vec3 direction{ {l->origin - closestHit.intersectPoint} };
                  direction = glm::normalize(direction);

                  for (Shape* s : world) //check whether or not our ray is obstructed by the light source
                  {
                      if (s->intersect(Ray{ closestHit.intersectPoint + closestHit.objNormal, direction }).intersect)
                          //adding normal to make sure it doesn't re-intersect with the intersection point
                      {
                          notObstructed = 0;
                          break;
                      }
                  }

                  float dotProduct1 = abs(glm::dot<float>(closestHit.objNormal, direction));

                  glm::vec3 reflectedLightDirection = closestHit.intersectPoint + closestHit.objNormal - l->origin;
                  reflectedLightDirection *= 2;
                  reflectedLightDirection -= closestHit.intersectPoint;
                  reflectedLightDirection = glm::normalize(reflectedLightDirection);

                  glm::vec3 reverseOrigin = observerLoc - closestHit.intersectPoint;
                  reverseOrigin = glm::normalize(reverseOrigin);
                  float dotProduct2 = pow(abs(glm::dot<float>(reflectedLightDirection, reverseOrigin)), 5);
                  intensity += l->intensity * (closestHit.objDiffusion * (float)notObstructed * dotProduct1 + closestHit.objSpecular * dotProduct2);
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

  for (Shape* s : world)
  {
      delete s;
  }
  for (PointLight* l : lights)
  {
      delete l;
  }

  return 0;
}
