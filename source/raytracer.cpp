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
#include <iomanip> //for stringstream set precision

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

void makeAnimationSdf(int iteration, int maxIteration)
{
    std::ofstream ofs("scene.sdf", std::ios_base::out | std::ios_base::binary);

    ofs << "define material purple 0.7 0.1 0.9 0.7 0.1 0.9 1.0 1.0 1.0 10 0.6 0.0 1.0" << std::endl
        << "define material table 0.7 0.6 0.0 0.7 0.6 0.0 1.0 1.0 1.0 10 0.2 0.0 1.0" << std::endl;

    ofs << "define material blue 0.0 0.3 0.9 0.0 0.3 0.9 1.0 1.0 1.0 10 ";
    if (iteration >= 40) ofs << 0.9;
    else if (iteration < 20) ofs << 0.0;
    else
    {
        std::stringstream boxReflectivity;

        boxReflectivity << std::fixed << std::setprecision(2) << (float)(iteration - 20.f) * 0.9f / 19.f;
        ofs << boxReflectivity.str();
    }
    ofs << " 0.0 1.0" << std::endl << std::endl;

    ofs << "define shape sphere testSphere 0 -70 -150 80 purple" << std::endl << std::endl
        << "define shape box table -400 -160 -3300 400 -140 2700 table" << std::endl
        << "define shape box testBox -150 -140 40 -350 -90 150 blue" << std::endl << std::endl
        << "define shape composite testComposite testSphere testBox table" << std::endl << std::endl
        << "define light -3000 1000 -1500 1.0 ";

    if (iteration < 90) ofs << "1.0 1.0";
    else
    {
        std::stringstream light;
        float const completion = (float)(iteration - 90.f) / 29.f; //ranges from 0 to 1

        light << std::fixed << std::setprecision(2) << 1 - completion << " " << 1 - completion;
        ofs << light.str();
    }
        
    ofs << " 1.0" << std::endl << std::endl;


    ofs << "define camera cam1 60 ";
    if (iteration >= 70) ofs << "500 500 -500 -7 -1 0 1 7 0";
    else if (iteration < 40) ofs << "0 200 500 0 -1 -7 0 7 1";
    else
    {
        std::stringstream camera;
        float const completion = (float)(iteration - 40.f) / 29.f; //ranges from 0 to 1
        float const cosVal = cos(completion * M_PI / 2); //ranges from 0 to PI / 2 -> 1 to 0
        float const z = cosVal;
        float const x = (1.f - cosVal);

        camera << std::fixed << std::setprecision(2) << completion * 500.f << " " << 200.f + completion * 300.f << " " << 500.f - completion * 1000.f << " " <<
            -7.f * x << " " << -1 << " " << -7.f * z << " " << x << " " << 7 << " " << z;

        ofs << camera.str();
    }

    ofs << std::endl << std::endl
        << "transform testSphere scale ";
    if (iteration >= 20) ofs << "2 2 2";
    else
    {
        std::stringstream sphereScale;
        sphereScale << std::fixed << std::setprecision(2) << 1.f + (float)(iteration) / 19.f; //from 1 to 2
        ofs << sphereScale.str() + " " + sphereScale.str() + " " + sphereScale.str();
    }
    ofs << std::endl;

    if (iteration >= 90) ofs << "transform testComposite translate 0 300 0";
    else if (iteration >= 70)
    {
        float const completion = (float)(iteration - 70.f) / 19.f;

        std::stringstream compositeRotation;
        compositeRotation << std::fixed << std::setprecision(2) << completion * 300.f;
        ofs << "transform testComposite translate 0 " << compositeRotation.str() << " 0";
    }
}

void raytrace(int iteration, int maxIteration)
{
    unsigned const image_width = 800;
    unsigned const image_height = 600;
    std::string const filename = "./checkerboard.ppm";

    Renderer renderer{ image_width, image_height, filename };

    renderer.render();

    Color const bgColor{ 0.1f, 0.4f, 0.2f }; //background color

    //camera variables
    glm::vec3 observerLoc{ 0.f, 0.f, 0.f };
    float fovX = 60.f;
    float const fovY = 55.f;

    glm::vec3 dir{ 0.f, 0.f, -1.f }; //-z direction (camera forward)
    glm::vec3 up{ 0.f, 1.f, 0.f }; //y direction

    int outputFileCount = iteration + 1;
    std::string outputFileName = "animation";
    if (outputFileCount < 100) outputFileName += "0";
    if (outputFileCount < 10) outputFileName += "0";
    outputFileName += std::to_string(outputFileCount);
    outputFileName += ".ppm";


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
            else if ("shape" == identifier)
            {
                in_sstream >> identifier;

                if ("box" == identifier)
                {
                    glm::vec3 p1, p2;
                    std::string name, matName;

                    in_sstream >> name >> p1.x >> p1.y >> p1.z >> p2.x >> p2.y >> p2.z >> matName;

                    world.createBox(name, matName, (p1 + p2) / 2.f,
                        abs(p2.x - p1.x), abs(p2.y - p1.y), abs(p2.z - p1.z));
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
                        world.reparent(childName, world.getRoot(), comp);
                        //comp.get()->addChild(world.findShape(childName));
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


    std::ofstream ofs(outputFileName, std::ios_base::out | std::ios_base::binary); //prepare to write to this image file to store raytracer image

    ofs << "P6" << std::endl << image_width << ' ' << image_height << std::endl << "255" << std::endl;

    //This is how we write to the renderer
    for (int i = image_height - 1; i >= 0; --i)
        //i is an integer and not an unsigned, because when unsigned this loop cannot reach negatives and will therefore infinitely repeat unless an unintuitive condition is used
    {
        for (unsigned int j = 0; j < image_width; ++j)
        {
            //by default our pixel has the background color
            Color pColor = { 0.f, 0.f, 0.f };

            float y = ((float)i / (float)image_height - 0.5f) * 2 * (float)tan(fovY / 180.f * M_PI); //how far along the y-axis (up) should we move for every unit along the z-axis (direction)
            float x = ((float)j / (float)image_width - 0.5f) * 2 * (float)tan(fovX / 180.f * M_PI); //how far along the x-axis (right) should we move for every unit along the z-axis

            glm::vec3 const right = glm::cross(dir, up); //calculate the x direction by finding the axis perpendicular to both our y and z axis

            glm::vec3 direction = x * right + y * up + dir; //calculate the direction of the ray we will trace by adding a movement along the z-axis to our pixel based movement along the x and y axis
            direction = glm::normalize(direction); //normalize direction, our calculations using directions and normals will always assume they have been normalized beforehand

            glm::vec3 origin = observerLoc;

            int reflectionCount = 0;
            int maxReflectionCount = 10; //limit our number of reflections in the case of an error or an infinite loop caused by a camera within a cage of reflective surfaces
            int refractionCount = 0;

            //lambda for lighting using reflections (had to use std::function because you can't initialize a lambda recursively)
                //we use a lambda because reflection and refraction will require recursive lighting calculations
            std::function<void(float, float)> lighting = [&](float reflectivity, float refractivity) //reflectivity of previous material, 1 for first material
            {
                HitPoint closestHit; //first we must find the closest hit traced by our ray

                //to do so we will check whether it intersects with any shape, and if it does compare the distance to see if this object hit is closer than a previously determined one
                for (std::shared_ptr<Shape> s : world.getShapes())
                {
                    Ray r{ origin, direction };

                    HitPoint h = s.get()->intersect(r); //transformations are handled within the intersect function

                    if (h.intersect && (h.dist < closestHit.dist || !closestHit.intersect)) //check if distance is shorter than previous hit or if there was a previous hit
                    {
                        closestHit = h;
                    }
                }

                //if an intersection occurred we can use calculations to predict the color formed by the lighting at that point
                if (closestHit.intersect)
                {
                    //reflective and refractive materials will show fewer of their own properties to show the properties of the reflected / refracted objects they display
                    reflectivity *= (1.f - closestHit.objMat.reflectivity);
                    refractivity *= (1.f - closestHit.objMat.refractivity);

                    //add our object's ambient color which should always be present to our material
                    pColor += closestHit.objMat.ambient * ambientIntensity *
                        reflectivity * refractivity;

                    //determine our remaining shading based on the individual lights
                    for (std::shared_ptr<PointLight> l : world.getLights())
                    {
                        bool notObstructed = true; //assume that our lighting isn't obstructed unless proven otherwise (if it obstructed we should notice a shadow)

                        //we have to find the direction from the point our ray hit, to the light source (and once again normalize it)
                        glm::vec3 direction{ {l.get()->origin - closestHit.intersectPoint} };
                        direction = glm::normalize(direction);

                        //we use a similar method to before to check if any object in the scene is obstructing the light source from this point
                        for (std::shared_ptr<Shape> s : world.getShapes())
                        {
                            //we add the normal to our intersection to avoid cases where our hit would cause the object to block its own lighting
                            Ray r{ closestHit.intersectPoint + closestHit.objNormal, direction };

                            //in this case we don't need to save any information about the intersection, we simply need to know if an intersection occurred
                            if (s.get()->intersect(r).intersect)
                            {
                                //if it did our light source is obstructed, and thus diffusive and specular lighting will be ineffective
                                notObstructed = false;
                                break;
                            }
                        }

                        if (notObstructed) //if our lighting isn't blocked, perform diffusive and specular calculations
                        {
                            //find the angle between our normal, and the direction from the object to the light source (this is the dot product we will use for our diffusive lighting)
                            float dotProduct1 = glm::dot<float>(closestHit.objNormal, direction);
                            if (dotProduct1 < 0.f) dotProduct1 = 0.f; //an angle below 0 would produce negative lighting, we don't want that

                            //we want to find the direction light is reflected in, by rotating it 180 degrees around the normal and inverting it (necessary for specular lighting)
                            glm::vec3 lightDirection = closestHit.intersectPoint + closestHit.objNormal - l.get()->origin;

                            glm::vec4 lightDir4{ lightDirection.x, lightDirection.y, lightDirection.z, 0.f };
                            glm::vec3 rotAxis = closestHit.objNormal;
                            glm::mat4 rotMat = glm::rotate((float)M_PI, rotAxis); //rotate 180 degrees around the normal
                            glm::vec4 reflectedLightDir4 = rotMat * -lightDir4;

                            glm::vec3 reflectedLightDirection{ reflectedLightDir4 };
                            reflectedLightDirection = glm::normalize(reflectedLightDirection);

                            //we also want to find the simple direction from the object to our observer, to use in combination with our reflected light direction to produce specular lighting
                            glm::vec3 reverseOrigin = origin - closestHit.intersectPoint;
                            reverseOrigin = glm::normalize(reverseOrigin);

                            //our dot product for specular lighting must be raised to the power of the specular exponent to reduce the area it is applied to
                            float dotProduct2 = pow(abs(glm::dot<float>(reflectedLightDirection, reverseOrigin)), closestHit.objMat.specularExp);

                            pColor += (closestHit.objMat.diffusive * dotProduct1 + closestHit.objMat.specular * dotProduct2)
                                * l.get()->color * l.get()->intensity
                                * reflectivity * refractivity;
                        }
                    }


                    //NOTE: refraction does not work with multiple reflactive surfaces inside of each other
                    //this is still inside of the intersection if statement, so a hit must have occurred
                    //if (closestHit.objMat.refractivity > 0.001f) //if our object is refractive, perform another lighting call using refractive information
                    //{
                    //    std::shared_ptr<Shape> s = world.findShape(closestHit.objName);

                    //    if (nullptr != s)
                    //    {
                    //        //our surface normal and direction should already be normalized
                    //        glm::vec3 rotAxis = glm::cross(closestHit.objNormal, direction); //we want to rotate around the line which is perpendicular to both our incident ray and surface normal
                    //        float rotAngle = asin(
                    //            sin(
                    //                acos(glm::dot(closestHit.objNormal, direction)) //the angle between our normal and incident ray
                    //            )
                    //            / closestHit.objMat.refractiveIndex
                    //        );

                    //        if (refractionCount > 1) std::cout << "Refraction Repetition Fail!" << std::endl;

                    //        glm::mat4 rotMat = glm::rotate(rotAngle, rotAxis);
                    //        glm::vec4 norm4{ closestHit.objNormal.x, closestHit.objNormal.y, closestHit.objNormal.z, 0.f };

                    //        glm::vec4 refractDir4 = rotMat * -norm4; //rotating the normal instead of the direction
                    //        glm::vec3 refractDir{ refractDir4 };

                    //        HitPoint h = s.get()->intersect(Ray{ //attempt to intersect with our shape (should work if it isn't unreasonably thin at this point)
                    //            closestHit.intersectPoint - closestHit.objNormal * 0.01f, //trace a ray starting inside of our shape
                    //            refractDir });

                    //        if (h.intersect)
                    //        {
                    //            //save our origin, because we are going to need to revert it back after recursively calling the function
                    //            glm::vec3 oldOrigin = origin;

                    //            //we will recursively call our function to add lighting based on what we find upon leaving our surface,
                    //            //this however renders objects within a refractive object invisible
                    //            origin = h.intersectPoint + h.objNormal * 0.01f;

                    //            ++refractionCount;
                    //            lighting(1.f, h.objMat.refractivity); //currently crashes, conditions cause infinite recursion

                    //            origin = oldOrigin;
                    //        }
                    //        else
                    //        {
                    //            pColor += bgColor * reflectivity; //ignore refractivvity since no refraction occurred
                    //            return;
                    //        }
                    //    }
                    //}
                }
                else //if an intersection didn't occur, we will instead render the background color at this point
                    //(we multiply it by the reflectivity and refractivity in the case that this is being reflected off of / refracted through an object at less than 100% intensity)
                {
                    pColor += bgColor * reflectivity * refractivity;
                    return;
                }



                //Recursive Reflection if necessary
                if (closestHit.intersect && //only reflect if a hit occurred (technically unnecessary because we return if no intersection occurred, moreso here for clarity, might delete later)
                    closestHit.objMat.reflectivity > 0.001f && //only reflect when necessary
                    reflectionCount < maxReflectionCount) //prevent infinite loops between reflective surfaces
                {
                    //shift origin and direction to bounce off of the point of our mirror surface
                    glm::mat4 rotMat = glm::rotate((float)M_PI, closestHit.objNormal);
                    glm::vec4 direction4{ direction.x, direction.y, direction.z, 0.f };

                    glm::vec4 newDir4 = rotMat * -direction4;
                    direction = glm::vec3{ newDir4 }; //reflect direction
                    direction = glm::normalize(direction);

                    origin = closestHit.intersectPoint + direction * 0.01f; //shift origin (add direction to make sure our hit test doesn't collide with the object)

                    ++reflectionCount;
                    lighting(closestHit.objMat.reflectivity, 1.f); //should replace 1.f with refractivity parameter
                }
            };

            lighting(1.f, 1.f);

            pColor.r /= (pColor.r + 1.f);
            pColor.g /= (pColor.g + 1.f);
            pColor.b /= (pColor.b + 1.f);

            Pixel p{ j, (unsigned)i, pColor };
            renderer.write(p);

            ofs << (char)((int)(pColor.r * 255) % 256) << (char)((int)(pColor.g * 255) % 256) << (char)((int)(pColor.b * 255) % 256);
            //floats range between 0 and 1, we want to change that to between 0 and 255, (needs to use char so every character represents one value between 0 and 255)
        }

        std::cout << ((float)(image_height - i - 1) / (float)(image_height * (maxIteration + 1))
            + (float)iteration / (float)(maxIteration + 1)) * 100.f << "% completed" << std::endl;
    }

    //Window window{ {image_width, image_height} };

    ofs.close();

    /*while (!window.should_close()) {
        if (window.get_key(GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            window.close();
        }
        window.show(renderer.color_buffer());
    }*/
}


//now single threaded again
int main(int argc, char* argv[])
{
    int const frames = 120;
    for (int i = 0; i < frames; ++i)
    {
        makeAnimationSdf(i, frames - 1);
        raytrace(i, frames - 1);
    }

    return 0;
}