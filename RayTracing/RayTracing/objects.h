#ifndef __objects_h__
#define __objects_h__

#include "vec3.h"
#include "rays.h"

#define MATH_PI 3.14159265358979323846

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

  double cosVec3(const vec3& v1, const vec3& v2) const;
public:
  object();

  virtual material GetMaterial(const ray& currentRay) const;
  virtual int CheckCollision(const ray& currentRay) const;
  virtual vec3 GetNormalToPoint(const ray& currentRay) const;
};

class plane : public object {
private:
  vec3 normal, center;
public:
  plane(const vec3& normal, const vec3& center, const material& mtr);

  int CheckCollision(const ray& currentRay) const;
  vec3 GetNormalToPoint(const ray& currentRay) const;
};

class sphere : public object {
protected:
  vec3 center;
  double radius;
public:
  sphere();
  sphere(const vec3& center, const double& radius, const material& mtr);

  vec3 GetCenter() const;
  double GetRadius() const;

  int CheckCollision(const ray& currentRay) const;
  vec3 GetNormalToPoint(const ray& currentRay) const;
};

class rectangle : public object {
protected:
  vec3 LDcorn, RDcorn, LUcorn;

  double ThreeVecMul(const vec3& v1, const vec3& v2, const vec3& v3) const;
public:
  rectangle();
  rectangle(const vec3& LDcorn, const vec3& RDcorn, const vec3& LUcorn, const material& mtr);

  int CheckCollision(const ray& currentRay) const;
  vec3 GetNormalToPoint(const ray& currentRay) const;
};

class chessRect : public rectangle {
private:
  int gridSizeW, gridSizeH;
  material secondMaterial;
public:
  chessRect(const vec3& LDcorn, const vec3& RDcorn, const vec3& LUcorn, const int& gridSizeW, const int& gridSizeH, const material& mtr1, const material& mtr2);
  
  material GetMaterial(const ray& currentRay) const;
};

class box : public rectangle {
protected:
  // dot1 - dot to corner
  vec3 dot1, dot2, dot3, dot4;
public:
  box();
  box(const vec3& dot1, const vec3& dot2, const vec3& dot3, const vec3& dot4, const material& mtr);

  int isInBox(const vec3& vec) const;

  int CheckCollision(const ray& currentRay) const;
  vec3 GetNormalToPoint(const ray& currentRay) const;
};

class boxAndSphere : public box {
protected:
  box bx;
  sphere sph;
  material secondMtr;
public:
  boxAndSphere(const vec3& dot1, const vec3& dot2, const vec3& dot3, const vec3& dot4, const vec3& center, const double& radius, const material& mtr1, const material& mtr2);

  int CheckCollision(const ray& currentRay) const;
  vec3 GetNormalToPoint(const ray& currentRay) const;
  material GetMaterial(const ray& currentRay) const;
};

class triangle : public object {
private:
  vec3 d1, d2, d3;

  double ThreeVecMul(const vec3& v1, const vec3& v2, const vec3& v3) const;
public:
  triangle(const vec3& dot1, const vec3& dot2, const vec3& dot3, const material& mtr);

  int CheckCollision(const ray& currentRay) const;
  vec3 GetNormalToPoint(const ray& currentRay) const;
};

class light {
private:
  vec3 pos, color;
public:
  light(const vec3& pos, const vec3& color);

  vec3 GetPos() const;
  vec3 GetColor() const;

  int isNearLightSource(const ray& currentRay) const;
};

#endif