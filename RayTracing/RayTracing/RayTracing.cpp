#include <cstdio>
#include <math.h>

#include "labengine.h"
#include "RayTracing.h"

scene::scene(const vec3& cam, const vec3& screenCenter, const std::list<object*>& objects, const std::list<light*>& lights) {
  this->cam = cam;
  this->screenCenter = screenCenter;
  this->objects = objects;
  this->lights = lights;
}

scene::scene(int width, int height, const vec3& cam, const vec3& screenCenter, const std::list<object*>& objects, const std::list<light*>& lights) {
  this->windowWidth = width;
  this->windowHeight = height;
  this->cam = cam;
  this->cam = cam;
  this->screenCenter = screenCenter;
  this->objects = objects;
  this->lights = lights;
}

vec3 scene::GetRefl(const vec3& vec, const vec3& normal) const {
  return normal * (2 * vec.vecMul(normal)) - vec;
}

vec3 scene::RayCast(ray &rayToCast, int currentDepth) const {
  vec3 pixelColor = backgroundColor; 
  material currentMtr;
  object* collObj = nullptr;
  int wasCollision = 0;

  while (rayToCast.GetRayDist() < MAX_RAY_DIST) {
    // check reflection
    for (auto& oneObj : objects)
      if (oneObj->CheckCollision(rayToCast) == 1) {
        currentMtr = oneObj->GetMaterial(rayToCast);
        collObj = oneObj;
        pixelColor = currentMtr.GetColor() * currentMtr.GetKoefA();
        wasCollision = 1;
        break;
      }

    // if reflection -> build secondary ray
    if (wasCollision == 1) {
      ray rayToLight;

      if (currentMtr.GetKoefR() != 1) {
        // build sec ray to each light sources
        for (auto& oneLight : lights) {
          rayToLight = ray(rayToCast.GetPos(), (oneLight->GetPos() - rayToCast.GetPos()).normal());
          wasCollision = 0;

          // check collisions while ray go to the light source
          while (oneLight->isNearLightSource(rayToLight) != 1) {
            for (auto& oneObj : objects)
              if (oneObj->CheckCollision(rayToLight) == 1) {
                wasCollision = 1;
                break;
              }

            // if colision -> exit
            if (wasCollision)
              break;

            // if no colision -> update ray
            rayToLight.rayUpdate();
          }

          // if go to light source -> add to pixel color
          if (oneLight->isNearLightSource(rayToLight) == 1) {
            vec3 lightDir = rayToLight.GetDir(),
                 normal = collObj->GetNormalToPoint(rayToCast),
                 Rm = GetRefl(lightDir, normal);

            vec3 Idif = (oneLight->GetColor() * (currentMtr.GetKoefD() * std::max((double)0, lightDir.vecMul(normal)))),
                 Ispec = (oneLight->GetColor() * (currentMtr.GetKoefS() * pow(std::max((double)0, Rm.vecMul(rayToCast.GetDir() * (-1))), currentMtr.GetKoefAlpha())));

            pixelColor = pixelColor + Idif + Ispec;
          }
        }
      }

      // add reflection component
      if (currentMtr.GetKoefR() != 0 && currentDepth < maxRefDepth) {
        vec3 newRayDir = GetRefl(rayToCast.GetDir(), collObj->GetNormalToPoint(rayToCast)) * (-1);
        ray newRay = ray(rayToCast.GetPos(), newRayDir);

        pixelColor = pixelColor + RayCast(newRay, currentDepth + 1) * currentMtr.GetKoefR();
      }
      break;
    }

    //if no reflection -> update ray
    rayToCast.rayUpdate();
  }

  return pixelColor;
}

void scene::render() const {
  labparams_t params = { (unsigned int)windowWidth, (unsigned int)windowHeight, 1 };

  if (LabInitWith(&params))    // Èíèöèàëèçàöèÿ
  {
    double scrCoordW = (screenCenter - cam).length(),
      scrCoordH = (screenCenter - cam).length() * windowHeight / windowWidth,
      pixelSize = scrCoordW / windowWidth, xCoord, yCoord, zCoord;

    for (int y = 0; y < windowHeight; y++){
      for (int x = 0; x < windowWidth; x++) {

        // ÏÐÅÎÁÐÀÇÎÂÀÍÈß ÑÊ ÄËß ÓÄÎÁÍÎÃÎ ÎÒÎÁÐÀÆÅÍÈß

        xCoord = screenCenter.getX(); // screenCenter.getZ() - scrCoordZ / 2 + x * (pixelSize + 1.0 / 2);
        yCoord = screenCenter.getY() - scrCoordW / 2 + pixelSize * (x + 1.0 / 2);
        zCoord = screenCenter.getZ() + scrCoordH / 2 - pixelSize * (y + 1.0 / 2);

        vec3 screenPointCrd = vec3(xCoord, yCoord, zCoord);
        ray screenRay = ray(screenPointCrd, (screenPointCrd - cam).normal());

        vec3 pixelColor = RayCast(screenRay, 0);

        LabSetColorRGB(std::min(255, (int)(pixelColor.getX() * 255)),
                       std::min(255, (int)(pixelColor.getY() * 255)),
                       std::min(255, (int)(pixelColor.getZ() * 255)));
        LabDrawPoint(x, y);
        LabDrawFlush();
      }
    }
    getchar();
    LabTerm();      // Çàâåðøåíèå ðàáîòû
  }
  return;
}
