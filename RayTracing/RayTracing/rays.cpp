#include "rays.h"

ray::ray() {
  pos = vec3(0, 0, 0);
  dir = vec3(0, 0, 0);
}

ray::ray(const vec3& startPos, const vec3& dir) {
  this->pos = startPos;
  this->dir = dir;
}

vec3 ray::GetPos() const {
  return pos;
}

vec3 ray::GetDir() const {
  return dir;
}
