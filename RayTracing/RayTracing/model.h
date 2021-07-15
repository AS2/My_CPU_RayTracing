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

  vec3 GetD1(void) const { return d1; };
  vec3 GetD2(void) const { return d2; };
  vec3 GetD3(void) const { return d3; };

  int CheckCollision(const ray& currentRay) const;
  vec3 GetNormalToPoint(const ray& currentRay) const;
};

class octoTree {
private:
  unsigned char CountCode(const vec3& d1) const;
  int IsSegmentInOctoTree(const vec3& d1, const vec3& d2) const;

  void SplitSibling(void);
public:
  double minX, maxX, minY, maxY, minZ, maxZ;

  std::list<modelTriangle> triangles;
  std::vector<octoTree*> siblings;

  octoTree();
  octoTree(const double& minXn, const double& maxXn, const double& minYn, const double& maxYn, const double& minZn, const double& maxZn);
  octoTree(std::list<modelTriangle>& trianglesToAdd, const double& minXn, const double& maxXn, const double& minYn, const double& maxYn, const double& minZn, const double& maxZn);

  int IsTriangleInOctoTree(const modelTriangle& mt) const;
  int IsInOctoTree(const vec3& v) const;
};

class model : public object {
private:
  octoTree* trianglesTree;

  int ParseIntInStr(std::string str);
public:
  model(const material& mtr, std::string filename, const double& size, double alpha = 0, double beta = 0, double gamma = 0, vec3 v1 = vec3());

  int CheckCollision(const ray& currentRay) const;
  vec3 GetNormalToPoint(const ray& currentRay) const;

  void DeleteObject(void) const;
};

#endif // __model_h__
