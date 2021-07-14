#include <list>
#include "model.h"
#include "RayTracing.h"

material materials[] = {
  material(),                                                         // metal - 0
  material(vec3(1, 1, 0), 0.35, 0.5, 0.1, 128, 0.4),                  // gold - 1
  material(vec3(0.25, 0.25, 1), 0.5, 0.1, 0.5, 5, 0),                 // blue plastic - 2
  material(vec3(1, 0.25, 0.25), 0.5, 0.1, 0.5, 5, 0),                 // red plastic - 3
  material(vec3(1, 1, 1), 0.05, 0.5, 0, 128, 0.8),                    // mirror - 4
  material(vec3(0.9, 0.9, 0.9), 0.5, 0.2, 0.3, 128, 0.13),            // ceramic - 5
  material(vec3(0.6, 0.6, 0.6), 0.05, 0.5, 0, 128, 0.7),              // dark mirror - 6
  material(vec3(0.98, 0.988, 0.651), 0.5, 0.1, 0.4, 128, 0.08),       // yellow wood - 7
  material(vec3(0.345, 0.196, 0.035), 0.5, 0.1, 0.4, 128, 0.08),      // dark wood - 8
};

void SceneConstructor(void) {
  // make scene params
  int width = 640, height = 320;
  vec3 camera = vec3(5, 0, 0), screenCenter = vec3(0, 0, 0);

  // push here scene's objects
  std::list<object*> objects;
  object* newObject;

  newObject = new sphere(vec3(-6, 0, 0), 1, material(vec3(0, 1, 0), 0.3, 0.9, 0.4, 127, 0.1));
  objects.push_back(newObject);

  newObject = new sphere(vec3(-8, -1, 1.2), 0.9, material(vec3(0, 1, 1), 0.3, 0.9, 0.4, 127, 0.2));
  objects.push_back(newObject);

  newObject = new sphere(vec3(-9, 1, 1.2), 0.9, materials[6]);
  objects.push_back(newObject);

  newObject = new sphere(vec3(-6, 1, -0.5), 0.6, material(vec3(1, 0, 1), 0.3, 0.9, 0.4, 127, 0.3));
  objects.push_back(newObject);

  newObject = new box(vec3(-5.5, -3, -1.75), vec3(-4.5, -4, -1.75), vec3(-4.5, -2, -1.75), vec3(-5.5, -3, -0.3357), materials[1]);
  objects.push_back(newObject);

  newObject = new boxAndSphere(vec3(-4, 2, -1.75), vec3(-3, 1, -1.75), vec3(-3, 3, -1.75), vec3(-4, 2, -0.3357), vec3(-3, 2, -1.0429), 0.85, materials[3], materials[2]);
  objects.push_back(newObject);

  newObject = new sphere(vec3(-3, -2, 1.4), 0.3, material(vec3(0.2, 0.2, 0.2), 0.5, 0.5, 0.6, 127, 0.1));
  objects.push_back(newObject);

  // floor
  newObject = new chessRect(vec3(2, -10, -2.5), vec3(2, 10, -2.5), vec3(-18, -10, -2.5), 20, 20, materials[7], materials[8]);
  objects.push_back(newObject);

  // ceiling
  newObject = new plane(vec3(0, 0, -1).normal(), vec3(-6, 0, 4), materials[5]);
  objects.push_back(newObject);

  // right wall
  newObject = new plane(vec3(0, 1, 0).normal(), vec3(0, -10, 0), materials[2]);
  objects.push_back(newObject);

  // left wall
  newObject = new plane(vec3(0, -1, 0).normal(), vec3(0, 10, 0), materials[3]);
  objects.push_back(newObject);

  // back mirror
  newObject = new plane(vec3(1, 0, 0).normal(), vec3(-18, 0, 0), materials[6]);
  objects.push_back(newObject);

  // back mirror
  newObject = new plane(vec3(1, 0, 0).normal(), vec3(2, 0, 0), materials[6]);
  objects.push_back(newObject);

  // push here scene's lights
  std::list<light*> lights;
  light* newLight;

  newLight = new light(vec3(-3, -4, 3), vec3(1, 1, 1));
  lights.push_back(newLight);

  newLight = new light(vec3(-3, 3, 2), vec3(0.4, 0.4, 1));
  lights.push_back(newLight);

  newLight = new light(vec3(-1, 4, -3), vec3(0.8, 0.3, 0.5));
  lights.push_back(newLight);

  // call scene constructor and render it
  scene newScene = scene(width, height, camera, screenCenter, objects, lights);
  newScene.render();

  for (auto& obj : objects)
    delete obj;

  for (auto& lght : lights)
    delete lght;
}

void SceneConstructor2(void) {
  // make scene params
  int width = 320, height = 160;
  vec3 camera = vec3(5, 0, 0), screenCenter = vec3(0, 0, 0);

  // push here scene's objects
  std::list<object*> objects;
  object* newObject;


  newObject = new plane(vec3(0, 0, 1).normal(), vec3(-6, 0, -1), material(vec3(0.1, 0.1, 0.1), 1, 1, 1, 100, 0.5));
  objects.push_back(newObject);

  // push here scene's lights
  std::list<light*> lights;
  light* newLight;

  newLight = new light(vec3(-3, -1.1, 1.5), vec3(0, 0, 1));
  lights.push_back(newLight);

  newLight = new light(vec3(-6, 0, 1.5), vec3(0, 1, 0));
  lights.push_back(newLight);

  newLight = new light(vec3(-3, 1.1, 1.5), vec3(1, 0, 0));
  lights.push_back(newLight);

  // call scene constructor and render it
  scene newScene = scene(width, height, camera, screenCenter, objects, lights);
  newScene.render();
}

void SceneConstructor3(void) {
  // make scene params
  int width = 320, height = 160;
  vec3 camera = vec3(5, 0, 0), screenCenter = vec3(0, 0, 0);

  // push here scene's objects
  std::list<object*> objects;
  object* newObject;

  newObject = new sphere(vec3(-4, 0, -0.5), 1.2, material(vec3(0, 1, 0), 0.3, 0.9, 0.4, 127, 0.1));
  objects.push_back(newObject);

  newObject = new rectangle(vec3(-9, -2, -1.5), vec3(-9, 2, -1.5), vec3(-9, -2, 1.5), materials[5]);
  objects.push_back(newObject);

  newObject = new rectangle(vec3(-6, -4, -1.5), vec3(-9, -2, -1.5), vec3(-6, -4, 1.5), materials[4]);
  objects.push_back(newObject);

  newObject = new rectangle(vec3(-9, 2, -1.5), vec3(-6, 4, -1.5), vec3(-9, 2, 1.5), materials[4]);
  objects.push_back(newObject);

  // push here scene's lights
  std::list<light*> lights;
  light* newLight;

  newLight = new light(vec3(-1, -1, 1.5), vec3(1, 1, 1));
  lights.push_back(newLight);

  // call scene constructor and render it
  scene newScene = scene(width, height, camera, screenCenter, objects, lights);
  newScene.render();
}

void SceneConstructor4(void) {
  // make scene params
  int width = 640, height = 320;
  vec3 camera = vec3(5, 0, 0), screenCenter = vec3(0, 0, 0);

  // push here scene's objects
  std::list<object*> objects;
  object* newObject;

  newObject = new boxAndSphere(vec3(-4, 2, -1.75), vec3(-3, 1, -1.75), vec3(-3, 3, -1.75), vec3(-4, 2, -0.3357), vec3(-3, 2, -1.0429), 0.85, materials[3], materials[2]);
  objects.push_back(newObject);

  // push here scene's lights
  std::list<light*> lights;
  light* newLight;

  newLight = new light(vec3(-3, -4, 3), vec3(1, 1, 1));
  lights.push_back(newLight);

  newLight = new light(vec3(-3, 3, 2), vec3(0.4, 0.4, 1));
  lights.push_back(newLight);

  newLight = new light(vec3(-1, 4, -3), vec3(0.8, 0.3, 0.5));
  lights.push_back(newLight);

  // call scene constructor and render it
  scene newScene = scene(width, height, camera, screenCenter, objects, lights);
  newScene.render();

  for (auto& obj : objects)
    delete obj;

  for (auto& lght : lights)
    delete lght;
}

void SceneConstructor5(void) {
  // make scene params
  int width = 160, height = 80;
  vec3 camera = vec3(5, 0, 0), screenCenter = vec3(0, 0, 0);

  // push here scene's objects
  std::list<object*> objects;
  object* newObject;

  newObject = new triangle(vec3(-6, 0, 2), vec3(-5, 0, 0), vec3(-6, 1, 0), materials[3]);
  objects.push_back(newObject);
  newObject = new triangle(vec3(-6, 0, 2), vec3(-6, 1, 0), vec3(-7, 0, 0), materials[3]);
  objects.push_back(newObject);
  newObject = new triangle(vec3(-6, 0, 2), vec3(-7, 0, 0), vec3(-6, -1, 0), materials[3]);
  objects.push_back(newObject);
  newObject = new triangle(vec3(-6, 0, 2), vec3(-6, -1, 0), vec3(-5, 0, 0), materials[3]);
  objects.push_back(newObject);
  newObject = new triangle(vec3(-6, 0, -2), vec3(-5, 0, 0), vec3(-6, 1, 0), materials[3]);
  objects.push_back(newObject);
  newObject = new triangle(vec3(-6, 0, -2), vec3(-6, 1, 0), vec3(-7, 0, 0), materials[3]);
  objects.push_back(newObject);
  newObject = new triangle(vec3(-6, 0, -2), vec3(-7, 0, 0), vec3(-6, -1, 0), materials[3]);
  objects.push_back(newObject);
  newObject = new triangle(vec3(-6, 0, -2), vec3(-6, -1, 0), vec3(-5, 0, 0), materials[3]);
  objects.push_back(newObject);

  // push here scene's lights
  std::list<light*> lights;
  light* newLight;

  newLight = new light(vec3(-3, -4, 3), vec3(1, 1, 1));
  lights.push_back(newLight);

  newLight = new light(vec3(-3, 3, 2), vec3(0.4, 0.4, 1));
  lights.push_back(newLight);

  newLight = new light(vec3(-1, 4, -3), vec3(0.8, 0.3, 0.5));
  lights.push_back(newLight);

  // call scene constructor and render it
  scene newScene = scene(width, height, camera, screenCenter, objects, lights);
  newScene.render();

  for (auto& obj : objects)
    delete obj;

  for (auto& lght : lights)
    delete lght;
}

void SceneConstructor6(void) {
  // make scene params
  int width = 640, height = 320;
  vec3 camera = vec3(5, 0, 0), screenCenter = vec3(0, 0, 0);

  // push here scene's objects
  std::list<object*> objects;
  object* newObject;

  //newObject = new model(materials[3], "./prism.obj", 1, 0, 0, 0, vec3(-6, 0, 0));
  //objects.push_back(newObject);

  newObject = new model(materials[3], "./Deer2.obj", 1, MATH_PI / 2, 0, MATH_PI / 4, vec3(-6.758, -3.85, -0.757));
  objects.push_back(newObject);

  // push here scene's lights
  std::list<light*> lights;
  light* newLight;

  newLight = new light(vec3(-3, -4, 3), vec3(1, 1, 1));
  lights.push_back(newLight);

  newLight = new light(vec3(-3, 3, 2), vec3(0.4, 0.4, 1));
  lights.push_back(newLight);

  newLight = new light(vec3(-1, 4, -3), vec3(0.8, 0.3, 0.5));
  lights.push_back(newLight);

  // call scene constructor and render it
  scene newScene = scene(width, height, camera, screenCenter, objects, lights);
  newScene.render();

  for (auto& obj : objects)
    delete obj;

  for (auto& lght : lights)
    delete lght;
}

int main(void) {
  SceneConstructor6();
}
