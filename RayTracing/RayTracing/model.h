#ifndef __model_h__
#define __model_h__

#include <string>
#include <vector>
#include <list>
#include "objects.h"

class modelTriangle {
private:
  vec3 d1, d2, d3;

  double ThreeVecMul(const vec3& v1, const vec3& v2, const vec3& v3) const;
  double cosVec3(const vec3& v1, const vec3& v2) const;

  //vec3 crosMulVec3(vec3& v1, vec3& v2);
public:
  modelTriangle();
  modelTriangle(const vec3& dot1, const vec3& dot2, const vec3& dot3);

  int CheckCollision(const ray& currentRay) const;
  vec3 GetNormalToPoint(const ray& currentRay) const;
};

class ortoTree {
public:
  double minX, maxX, minY, maxY, minZ, maxZ;

  std::list<modelTriangle> triangles;
  std::vector<ortoTree*> siblings;

  ortoTree();
  ortoTree(const double& minXn, const double& maxXn, const double& minYn, const double& maxYn, const double& minZn, const double& maxZn);
  ortoTree(std::list<modelTriangle>& trianglesToAdd, const double& minXn, const double& maxXn, const double& minYn, const double& maxYn, const double& minZn, const double& maxZn);

  void AddTriangle(const modelTriangle& newTriangle);

  int IsInOrtoTree(const ray& currentRay) const;
};

class model : public object {
private:
  ortoTree trianglesTree;

  int ParseIntInStr(std::string str);
public:
  model(const material& mtr, std::string filename, const double& size, double alpha = 0, double beta = 0, double gamma = 0, vec3 v1 = vec3());

  int CheckCollision(const ray& currentRay) const;
  vec3 GetNormalToPoint(const ray& currentRay) const;
};

#endif // __model_h__
