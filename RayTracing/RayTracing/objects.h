#ifndef __objects_h__
#define __objects_h__

#include "vec3.h"
#include "rays.h"

class material {
private:
  vec3 color;
  double kAmb, kSpec, kDif, alpha, kRefl;
public:
  material();
  material(const vec3& color, const double& kAmb, const double& kSpec, const double& kDif, const double& alpha, const double& kRefl);

  vec3 GetColor() { return color; };
  double GetKoefA() { return kAmb; };
  double GetKoefS() { return kSpec; };
  double GetKoefD() { return kDif; };
  double GetKoefAlpha() { return alpha; };
  double GetKoefR() { return kRefl; };
};

class object {
protected:
  material mater;

  double cosVec3(vec3& v1, vec3& v2);
public:
  object();

  virtual material GetMaterial(ray& currentRay);
  virtual int CheckCollision(ray& currentRay);
  virtual vec3 GetNormalToPoint(ray& currentRay);
};

class plane : public object {
private:
  vec3 normal, center;
public:
  plane(const vec3& normal, const vec3& center, const material& mtr);

  int CheckCollision(ray& currentRay);
  vec3 GetNormalToPoint(ray& currentRay);
};

class sphere : public object {
protected:
  vec3 center;
  double radius;
public:
  sphere();
  sphere(const vec3& center, const double& radius, const material& mtr);

  vec3 GetCenter();
  double GetRadius();

  int CheckCollision(ray& currentRay);
  vec3 GetNormalToPoint(ray& currentRay);
};

class rectangle : public object {
protected:
  vec3 LDcorn, RDcorn, LUcorn;

  double ThreeVecMul(vec3& v1, vec3& v2, vec3& v3);
public:
  rectangle();
  rectangle(const vec3& LDcorn, const vec3& RDcorn, const vec3& LUcorn, const material& mtr);

  int CheckCollision(ray& currentRay);
  vec3 GetNormalToPoint(ray& currentRay);
};

class chessRect : public rectangle {
private:
  int gridSizeW, gridSizeH;
  material secondMaterial;
public:
  chessRect(const vec3& LDcorn, const vec3& RDcorn, const vec3& LUcorn, const int& gridSizeW, const int& gridSizeH, const material& mtr1, const material& mtr2);
  
  material GetMaterial(ray& currentRay);
};

class box : public rectangle {
protected:
  // dot1 - dot to corner
  vec3 dot1, dot2, dot3, dot4;
public:
  box();
  box(const vec3& dot1, const vec3& dot2, const vec3& dot3, const vec3& dot4, const material& mtr);

  int isInBox(vec3& vec);

  int CheckCollision(ray& currentRay);
  vec3 GetNormalToPoint(ray& currentRay);
};

class boxAndSphere : public box {
protected:
  box bx;
  sphere sph;
  material secondMtr;
public:
  boxAndSphere(const vec3& dot1, const vec3& dot2, const vec3& dot3, const vec3& dot4, const vec3& center, const double& radius, const material& mtr1, const material& mtr2);

  int CheckCollision(ray& currentRay);
  vec3 GetNormalToPoint(ray& currentRay);
  material GetMaterial(ray& currentRay);
};

class light {
private:
  vec3 pos, color;
public:
  light(const vec3& pos, const vec3& color);

  vec3 GetPos();
  vec3 GetColor();

  int isNearLightSource(ray& currentRay);
};

#endif