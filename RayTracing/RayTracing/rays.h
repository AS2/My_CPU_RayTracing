#ifndef __rays_h__
#define __rays_h__

#include "vec3.h"

#define RAY_INF 1000
#define RAY_SHIFT 0.001

const vec3 backgroundColor = vec3(0.3, 0.5, 0.7);

class ray {
private:
  vec3 pos, dir;
public:
  ray();
  ray(const vec3& startPos, const vec3& dir);

  vec3 GetPos() const;
  vec3 GetDir() const;
};

#endif // __rays_h__
