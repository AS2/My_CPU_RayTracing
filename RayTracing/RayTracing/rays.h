#ifndef __rays_h__
#define __rays_h__

#include "vec3.h"

#define MAX_RAY_DIST 2500
#define RAY_STEP 0.025

const vec3 backgroundColor = vec3(0.3, 0.5, 0.7);

class ray {
private:
  vec3 pos, dir;
  int currentRayDist;
public:
  ray();
  ray(const vec3& startPos, const vec3& dir);

  vec3 GetPos() const;
  vec3 GetDir() const;
  int GetRayDist() const;

  void rayUpdate();
  void ChangeDir(const vec3& newDir);
};

#endif // __rays_h__
