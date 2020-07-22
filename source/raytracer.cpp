#include <renderer.hpp>
#include <window.hpp>

#include <GLFW/glfw3.h>
#include <thread>
#include <utility>
#include <cmath>

#include "shape.hpp"
#include "sphere.hpp"
#include "box.hpp"

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

  std::vector<Sphere*> world;
  world.push_back(new Sphere{ "Sphere1", Color{0.7f, 0.f, 0.2f}, {0.f, 0.f, -150.f}, 50 });

  //This is how we write to the renderer
  for (unsigned int i = 0; i < image_width; i++)
  {
      for (unsigned int j = 0; j < image_height; j++)
      {
          HitPoint closestHit;

          for (Sphere* s : world)
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
