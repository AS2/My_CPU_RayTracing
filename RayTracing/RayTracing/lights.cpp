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

double light::DistToLightSource(const vec3& pos) const {
  return (this->pos - pos).length();
}
