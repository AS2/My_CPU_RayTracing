#include "objects.h"

light::light(const vec3& pos, const vec3& color) {
  this->pos = pos;
  this->color = color;
}

vec3 light::GetPos() const {
  return pos;
}

vec3 light::GetColor() const {
  return color;
}

int light::isNearLightSource(const ray& currentRay) const {
  if ((pos - currentRay.GetPos()).length() <= RAY_STEP * 1.2)
    return 1;
  return 0;
}
