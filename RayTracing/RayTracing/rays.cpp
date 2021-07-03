#include "rays.h"

ray::ray() {
  pos = vec3(0, 0, 0);
  dir = vec3(0, 0, 0);
  currentRayDist = 0;
}

ray::ray(const vec3& startPos, const vec3& dir) {
  this->pos = startPos;
  this->dir = dir;
  currentRayDist = 0;
}

vec3 ray::GetPos() {
  return pos;
}

vec3 ray::GetDir() {
  return dir;
}

int ray::GetRayDist() {
  return currentRayDist;
}

void ray::rayUpdate() {
  currentRayDist++;
  pos = pos + dir * RAY_STEP;
}

void ray::ChangeDir(const vec3& newDir) {
  dir = newDir;
}
