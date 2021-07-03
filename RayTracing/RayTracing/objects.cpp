#include <math.h>
#include "objects.h"

material::material() {
  this->color = vec3(0, 0, 0);
  this->kAmb = 0;
  this->kSpec = 0;
  this->kDif = 0;
  this->alpha = 0;
  this->kRefl = 0;
}

material::material(const vec3& color, const double& kAmb, const double& kSpec, const double& kDif, const double& alpha, const double& kRefl) {
  this->color = color;
  this->kAmb = kAmb;
  this->kSpec = kSpec;
  this->kDif = kDif;
  this->alpha = alpha;
  this->kRefl = kRefl;
}

object::object() {
  mater = material();
}

material object::GetMaterial(ray& currentRay) {
  return mater;
}

int object::CheckCollision(ray& currentRay) {
  return -1;
}

vec3 object::GetNormalToPoint(ray& currentRay) {
  return vec3(0, 0, 0);
}

double object::cosVec3(vec3& v1, vec3& v2) {
  return (v1.getX() * v2.getX() + v1.getY() * v2.getY() + v1.getZ() * v2.getZ()) / (v1.length() * v2.length());
}

plane::plane(const vec3& normal, const vec3& center, const material& mtr) {
  this->mater = mtr;
  
  this->normal = normal;
  this->center = center;
}

int plane::CheckCollision(ray& currentRay) {
  vec3 pos1 = currentRay.GetPos() - center, pos2 = (currentRay.GetPos() + currentRay.GetDir() * RAY_STEP - center);

  if (cosVec3(pos1, normal) * cosVec3(pos2, normal) <= 0)
    return 1;
  return 0;
}

vec3 plane::GetNormalToPoint(ray& currentRay) {
  return normal.normal();
}

rectangle::rectangle() {
  this->mater = material();

  this->LDcorn = vec3();
  this->RDcorn = vec3();
  this->LUcorn = vec3();
}

rectangle::rectangle(const vec3& LDcorn, const vec3& RDcorn, const vec3& LUcorn, const material& mtr) {
  this->mater = mtr;

  this->LDcorn = LDcorn;
  this->RDcorn = RDcorn;
  this->LUcorn = LUcorn;
}

double rectangle::ThreeVecMul(vec3& v1, vec3& v2, vec3& v3) {
  return (v1.getX() * v2.getY() * v3.getZ() + v1.getY() * v2.getZ() * v3.getX() + v1.getZ() * v2.getX() * v3.getY()) -
         (v1.getZ() * v2.getY() * v3.getX() + v1.getY() * v2.getX() * v3.getZ() + v1.getX() * v2.getZ() * v3.getY());
}

int rectangle::CheckCollision(ray& currentRay) {
  // check collinear vecs
  vec3 v1 = (LUcorn - LDcorn), v2 = (RDcorn - LDcorn), 
    v3_1 = (currentRay.GetPos() - LDcorn),
    v3_2 = (currentRay.GetPos() + currentRay.GetDir() * RAY_STEP - LDcorn);
  double res1 = ThreeVecMul(v1, v2, v3_1),
    res2 = ThreeVecMul(v1, v2, v3_2);

  if (res1 * res2 > 0)
    return 0;

  // check if we are in plane piece
  vec3 dotOnPlane = currentRay.GetPos(), newVec = (dotOnPlane - LDcorn),
    xPart = v2.normal() * newVec.length() * cosVec3(newVec, v2), 
    yPart = v1.normal() * newVec.length() * cosVec3(newVec, v1);

  double alpha, beta;

  if (v2.getX() != 0)
    alpha = xPart.getX() / v2.getX();
  else if (v2.getY() != 0)
    alpha = xPart.getY() / v2.getY();
  else
    alpha = xPart.getZ() / v2.getZ();

  if (v1.getX() != 0)
    beta = yPart.getX() / v1.getX();
  else if (v1.getY() != 0)
    beta = yPart.getY() / v1.getY();
  else
    beta = yPart.getZ() / v1.getZ();

  if (alpha >= 0 && alpha <= 1 && beta >= 0 && beta <= 1)
    return 1;
  else
    return 0;
}

vec3 rectangle::GetNormalToPoint(ray& currentRay) {
  vec3 v1 = (LUcorn - LDcorn),
    v2 = (RDcorn - LDcorn), normal, dir = currentRay.GetDir();
  double x, y, z;

  if (v1.getX() == 0 && v2.getX() == 0) {
    x = 1;
    y = z = 0;
  }
  else if (v1.getY() == 0 && v2.getY() == 0) {
    y = 1;
    x = z = 0;
  }
  else if (v1.getZ() == 0 && v2.getZ() == 0) {
    z = 1;
    x = y = 0;
  }
  else {
    x = 1;
    y = (v1.getZ() * v2.getX() - v1.getX() * v2.getZ()) / (v2.getZ() * v1.getY() - v2.getY() * v1.getZ());
    z = (v1.getY() * v2.getX() - v1.getX() * v2.getY()) / (v2.getZ() * v1.getY() - v2.getY() * v1.getZ());
  }

  normal = vec3(x, y, z).normal();

  if (cosVec3(dir, normal) > 0)
    normal = normal * (-1);

  return normal;
}

chessRect::chessRect(const vec3& LDcorn, const vec3& RDcorn, const vec3& LUcorn, const int& gridSizeW, const int& gridSizeH, const material& mtr1, const material& mtr2) {
  this->mater = mtr1;
  this->secondMaterial = mtr2;

  this->LDcorn = LDcorn;
  this->RDcorn = RDcorn;
  this->LUcorn = LUcorn;

  this->gridSizeW = gridSizeW;
  this->gridSizeH = gridSizeH;
}

material chessRect::GetMaterial(ray& currentRay) {
  vec3 v1 = (LUcorn - LDcorn), v2 = (RDcorn - LDcorn),
       dotOnPlane = currentRay.GetPos(), newVec = (dotOnPlane - LDcorn),
       xPart = v2.normal() * newVec.length() * cosVec3(newVec, v2),
       yPart = v1.normal() * newVec.length() * cosVec3(newVec, v1);
  double alpha, beta;

  if (v2.getX() != 0)
    alpha = xPart.getX() / v2.getX();
  else if (v2.getY() != 0)
    alpha = xPart.getY() / v2.getY();
  else
    alpha = xPart.getZ() / v2.getZ();

  if (v1.getX() != 0)
    beta = yPart.getX() / v1.getX();
  else if (v1.getY() != 0)
    beta = yPart.getY() / v1.getY();
  else
    beta = yPart.getZ() / v1.getZ();

  double ceilW = v2.length() / gridSizeW, ceilH = v1.length() / gridSizeH;

  if ((int)(floor(alpha * v2.length() / ceilW) + floor(beta * v1.length() / ceilH)) % 2 == 0)
    return mater;
  else
    return secondMaterial;
}

sphere::sphere(const vec3& center, const double& radius, const material& mtr) {
  this->mater = mtr;
  
  this->center = center;
  this->radius = radius;
}

int sphere::CheckCollision(ray& currentRay) {
  if ((currentRay.GetPos() - center).length() > radius &&
    (currentRay.GetPos() + currentRay.GetDir() * RAY_STEP - center).length() <= radius)
    return 1;
  return 0;
}

vec3 sphere::GetNormalToPoint(ray& currentRay) {
  return (currentRay.GetPos() - center).normal();
}
