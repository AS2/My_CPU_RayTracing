#include <cstdio>
#include <iostream>
#include <math.h>
#include <vector>

#include "labengine.h"
#include "RayTracing.h"

#define SSAA_POW 2

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
  vec3 pixelColor = backgroundColor, collPnt;
  material currentMtr;
  object* collObj = nullptr;

  std::vector<object*> collObjs(objects.size(), nullptr);
  std::vector<vec3> collPnts(objects.size(), vec3());
  int currentIndex = 0;

  // check reflection
  for (auto& oneObj : objects)
    if (oneObj->CheckCollision(rayToCast, collPnt, 0) == 1) {
      collObjs[currentIndex] = oneObj;
      collPnts[currentIndex++] = collPnt;
    }

  // find nearest obj
  double nearestLength = RAY_INF;
  int collisionsCnt = currentIndex;
  for (int i = 0; i < collisionsCnt; i++) {
    if ((collPnts[i] - rayToCast.GetPos()).length() < nearestLength) {
      nearestLength = (collPnts[i] - rayToCast.GetPos()).length();
      collObj = collObjs[i];
      collPnt = collPnts[i];
    }
  }

  // if reflection -> build secondary ray
  if (collObj != nullptr) {
    currentMtr = collObj->GetMaterial(collPnt, rayToCast.GetDir(), rayToCast);
    pixelColor = currentMtr.GetColor() * currentMtr.GetKoefA();
    ray rayToLight;

    if (currentMtr.GetKoefR() != 1) {
      // build sec ray to each light sources
      for (auto& oneLight : lights) {
        rayToLight = ray(collPnt, (oneLight->GetPos() - collPnt).normal());
        double lightDist = oneLight->DistToLightSource(collPnt);

        vec3 collLightPnt;
        object* collLightObj = nullptr;
        std::vector<object*> collLightObjs(objects.size(), nullptr);
        std::vector<vec3> collLightPnts(objects.size(), vec3());
        currentIndex = 0;

        // check collisions while ray go to the light source
        for (auto& oneObj : objects)
          if (oneObj->CheckCollision(rayToLight, collLightPnt, 1) == 1) {
            collLightObjs[currentIndex] = oneObj;
            collLightPnts[currentIndex++] = collLightPnt;
          }

        // find nearest obj
        nearestLength = RAY_INF;
        collisionsCnt = currentIndex;
        for (int i = 0; i < collisionsCnt; i++)
          if ((collLightPnts[i] - collPnt).length() < nearestLength)
            nearestLength = (collLightPnts[i] - collPnt).length();

        if (nearestLength < lightDist)
          continue;
        // if go to light source -> add to pixel color
        else {
          vec3 lightDir = rayToLight.GetDir(),
            normal = collObj->GetNormalToPoint(collPnt, rayToCast.GetDir(), rayToCast),
            Rm = GetRefl(lightDir, normal);

          vec3 Idif = (oneLight->GetColor() * (currentMtr.GetKoefD() * std::max((double)0, lightDir.vecMul(normal)))),
            Ispec = (oneLight->GetColor() * (currentMtr.GetKoefS() * pow(std::max((double)0, Rm.vecMul(rayToCast.GetDir() * (-1))), currentMtr.GetKoefAlpha())));

          pixelColor = pixelColor + Idif + Ispec;
        }
      }
    }

    // add reflection component
    if (currentMtr.GetKoefR() != 0 && currentDepth < maxRefDepth) {
      vec3 newRayDir = GetRefl(rayToCast.GetDir(), collObj->GetNormalToPoint(collPnt, rayToCast.GetDir(), rayToCast)) * (-1);
      ray newRay = ray(collPnt, newRayDir);

      pixelColor = pixelColor + RayCast(newRay, currentDepth + 1) * currentMtr.GetKoefR();
    }
  }

  return pixelColor;
}

void scene::render() const {
  labparams_t params = { (unsigned int)windowWidth, (unsigned int)windowHeight, 1 };
  std::vector<vec3> pixelsOnScreen(windowHeight * windowWidth * SSAA_POW * SSAA_POW);
  if (LabInitWith(&params))    // Èíèöèàëèçàöèÿ
  {
    double scrCoordW = (screenCenter - cam).length(),
      scrCoordH = (screenCenter - cam).length() * windowHeight / windowWidth,
      pixelSize = scrCoordW / (windowWidth * SSAA_POW), xCoord, yCoord, zCoord;

    for (int y = 0; y < windowHeight * SSAA_POW; y++){
      for (int x = 0; x < windowWidth * SSAA_POW; x++) {

        // ÏÐÅÎÁÐÀÇÎÂÀÍÈß ÑÊ ÄËß ÓÄÎÁÍÎÃÎ ÎÒÎÁÐÀÆÅÍÈß

        xCoord = screenCenter.getX(); // screenCenter.getZ() - scrCoordZ / 2 + x * (pixelSize + 1.0 / 2);
        yCoord = screenCenter.getY() - scrCoordW / 2 + pixelSize * (x + 1.0 / 2);
        zCoord = screenCenter.getZ() + scrCoordH / 2 - pixelSize * (y + 1.0 / 2);

        vec3 screenPointCrd = vec3(xCoord, yCoord, zCoord);
        ray screenRay = ray(screenPointCrd, (screenPointCrd - cam).normal());

        pixelsOnScreen[y * (windowWidth * SSAA_POW) + x] = RayCast(screenRay, 0);
      }
      if (y % SSAA_POW == 0)
        std::cout << y / SSAA_POW << "/" << windowHeight << std::endl;
    }

    vec3 pixOnScr;
    for (int y = 0; y < windowHeight; y++) 
      for (int x = 0; x < windowWidth; x++) {
        pixOnScr = vec3(0, 0, 0);
        for (int ay = 0; ay < SSAA_POW; ay++)
          for (int ax = 0; ax < SSAA_POW; ax++)
            pixOnScr = pixOnScr + pixelsOnScreen[(y * SSAA_POW + ay) * (windowWidth * SSAA_POW) + (x * SSAA_POW + ax)];

        pixOnScr = pixOnScr * (1.0f / (SSAA_POW * SSAA_POW));

        LabSetColorRGB(std::min(255, (int)(pixOnScr.getX() * 255)),
                       std::min(255, (int)(pixOnScr.getY() * 255)),
                       std::min(255, (int)(pixOnScr.getZ() * 255)));
        LabDrawPoint(x, y);
      }
    LabDrawFlush();
    getchar();

    LabTerm();      // Çàâåðøåíèå ðàáîòû
  }
  return;
}
