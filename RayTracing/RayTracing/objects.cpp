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

material object::GetMaterial(const ray& currentRay) const {
  return mater;
}

int object::CheckCollision(const ray& currentRay) const {
  return -1;
}

vec3 object::GetNormalToPoint(const ray& currentRay) const {
  return vec3(0, 0, 0);
}

double object::cosVec3(const vec3& v1, const vec3& v2) const {
  return (v1.getX() * v2.getX() + v1.getY() * v2.getY() + v1.getZ() * v2.getZ()) / (v1.length() * v2.length());
}

static vec3 crosMulVec3(vec3& v1, vec3& v2) {
  return vec3((v1.getY() * v2.getZ() - v2.getY() * v1.getZ()),
              (v1.getX() * v2.getZ() - v2.getX() * v1.getZ()) * (-1),
              (v1.getX() * v2.getY() - v2.getX() * v1.getY()));
}

plane::plane(const vec3& normal, const vec3& center, const material& mtr) {
  this->mater = mtr;
  
  this->normal = normal;
  this->center = center;
}

int plane::CheckCollision(const ray& currentRay) const {
  vec3 pos1 = currentRay.GetPos() - center, pos2 = (currentRay.GetPos() + currentRay.GetDir() * RAY_STEP - center);

  if (cosVec3(pos1, normal) * cosVec3(pos2, normal) <= 0)
    return 1;
  return 0;
}

vec3 plane::GetNormalToPoint(const ray& currentRay) const {
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

double rectangle::ThreeVecMul(const vec3& v1, const vec3& v2, const vec3& v3) const {
  return (v1.getX() * v2.getY() * v3.getZ() + v1.getY() * v2.getZ() * v3.getX() + v1.getZ() * v2.getX() * v3.getY()) -
         (v1.getZ() * v2.getY() * v3.getX() + v1.getY() * v2.getX() * v3.getZ() + v1.getX() * v2.getZ() * v3.getY());
}

int rectangle::CheckCollision(const ray& currentRay) const {
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

  if (alpha >= 0 - RAY_STEP && alpha <= 1 + RAY_STEP && beta >= 0 - RAY_STEP && beta <= 1 + RAY_STEP)
    return 1;
  else
    return 0;
}

vec3 rectangle::GetNormalToPoint(const ray& currentRay) const {
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

material chessRect::GetMaterial(const ray& currentRay) const {
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

box::box() {
  mater = material();
  dot1 = dot2 = dot3 = dot4 = vec3();
}

box::box(const vec3& dot1, const vec3& dot2, const vec3& dot3, const vec3& dot4, const material& mtr) {
  this->mater = mtr;

  this->dot1 = dot1;
  this->dot2 = dot2;
  this->dot3 = dot3;
  this->dot4 = dot4;
}

int box::isInBox(const vec3& vec) const {
  vec3 e1 = (dot2 - dot1), e2 = (dot3 - dot1), e3 = (dot4 - dot1), pVec = vec - dot1;
  vec3 zPart = e3.normal() * cosVec3(pVec, e3) * pVec.length(), pVecProj = pVec - zPart,
       yPart = e2.normal() * cosVec3(pVecProj, e2) * pVecProj.length(),
       xPart = e1.normal() * cosVec3(pVecProj, e1) * pVecProj.length();

  double alpha, beta, gamma;

  if (e2.getX() != 0)
    alpha = xPart.getX() / e2.getX();
  else if (e2.getY() != 0)
    alpha = xPart.getY() / e2.getY();
  else
    alpha = xPart.getZ() / e2.getZ();

  if (e1.getX() != 0)
    beta = yPart.getX() / e1.getX();
  else if (e1.getY() != 0)
    beta = yPart.getY() / e1.getY();
  else
    beta = yPart.getZ() / e1.getZ();

  if (e3.getX() != 0)
    gamma = zPart.getX() / e3.getX();
  else if (e3.getY() != 0)
    gamma = zPart.getY() / e3.getY();
  else
    gamma = zPart.getZ() / e3.getZ();

  if (alpha >= 0 && alpha <= 1 &&
      beta >= 0 && beta <= 1 &&
      gamma >= 0 && gamma <= 1)
    return 1;
  else
    return 0;
}

int box::CheckCollision(const ray& currentRay) const {
  vec3 pos = currentRay.GetPos(), nextPos = currentRay.GetPos() + currentRay.GetDir() * RAY_STEP;
  if (isInBox(pos) == 0 && isInBox(nextPos) == 1)
    return 1;
  return 0;
}

vec3 box::GetNormalToPoint(const ray& currentRay) const {
  vec3 dot5 = dot4 + (dot2 - dot1),
       dot6 = dot4 + (dot3 - dot1),
       dot7 = dot6 + (dot2 - dot1);

  rectangle p1 = rectangle(dot1, dot2, dot4, mater),
            p2 = rectangle(dot1, dot3, dot4, mater),
            p3 = rectangle(dot1, dot3, dot2, mater),
            p4 = rectangle(dot4, dot6, dot5, mater),
            p5 = rectangle(dot5, dot2, dot7, mater),
            p6 = rectangle(dot6, dot3, dot7, mater);

  if (p1.CheckCollision(currentRay))
    return p1.GetNormalToPoint(currentRay);
  else if (p2.CheckCollision(currentRay))
    return p2.GetNormalToPoint(currentRay);
  else if (p3.CheckCollision(currentRay))
    return p3.GetNormalToPoint(currentRay);
  else if (p4.CheckCollision(currentRay))
    return p4.GetNormalToPoint(currentRay);
  else if (p5.CheckCollision(currentRay))
    return p5.GetNormalToPoint(currentRay);
  else
    return p6.GetNormalToPoint(currentRay);
}

sphere::sphere() {
  this->mater = material();

  this->center = vec3();
  this->radius = 0;
}

sphere::sphere(const vec3& center, const double& radius, const material& mtr) {
  this->mater = mtr;
  
  this->center = center;
  this->radius = radius;
}

vec3 sphere::GetCenter() const {
  return center;
}

double sphere::GetRadius() const {
  return radius;
}

int sphere::CheckCollision(const ray& currentRay) const {
  if ((currentRay.GetPos() - center).length() > radius &&
    (currentRay.GetPos() + currentRay.GetDir() * RAY_STEP - center).length() <= radius)
    return 1;
  return 0;
}

vec3 sphere::GetNormalToPoint(const ray& currentRay) const {
  return (currentRay.GetPos() - center).normal();
}

boxAndSphere::boxAndSphere(const vec3& dot1, const vec3& dot2, const vec3& dot3, const vec3& dot4, const vec3& center, const double& radius, const material& mtr1, const material& mtr2) {
  this->bx = box(dot1, dot2, dot3, dot4, mtr1);
  this->sph = sphere(center, radius, mtr2);

  this->mater = mtr1;
  this->secondMtr = mtr2;
}

int boxAndSphere::CheckCollision(const ray& currentRay) const {
  vec3 pos = currentRay.GetPos(), newPos = currentRay.GetPos() + currentRay.GetDir() * RAY_STEP;
  
  if ((bx.CheckCollision(currentRay) == 1 && (pos - sph.GetCenter()).length() > sph.GetRadius() && (newPos - sph.GetCenter()).length() > sph.GetRadius()) ||
    (bx.isInBox(pos) == 1 && (pos - sph.GetCenter()).length() <= sph.GetRadius() && (newPos - sph.GetCenter()).length() > sph.GetRadius()))
    return 1;
  return 0;
}

vec3 boxAndSphere::GetNormalToPoint(const ray & currentRay) const {
  vec3 pos = currentRay.GetPos(), newPos = currentRay.GetPos() + currentRay.GetDir() * RAY_STEP;

  if ((bx.CheckCollision(currentRay) == 1 &&
      (pos - sph.GetCenter()).length() > sph.GetRadius() &&
      (newPos - sph.GetCenter()).length() > sph.GetRadius()))
    return bx.GetNormalToPoint(currentRay);
  else
    return (pos - sph.GetCenter()).normal();
}

material boxAndSphere::GetMaterial(const ray& currentRay) const {
  vec3 pos = currentRay.GetPos(), newPos = currentRay.GetPos() + currentRay.GetDir() * RAY_STEP;
  double rad = sph.GetRadius();

  if ((bx.CheckCollision(currentRay) == 1 &&
      (pos - sph.GetCenter()).length() > rad &&
      (newPos - sph.GetCenter()).length() > rad))
    return mater;
  else
    return secondMtr;
}

triangle::triangle(const vec3& dot1, const vec3& dot2, const vec3& dot3, const material& mtr) {
  this->mater = mtr;

  d1 = dot1;
  d2 = dot2;
  d3 = dot3;
}

double triangle::ThreeVecMul(const vec3& v1, const vec3& v2, const vec3& v3) const {
  return (v1.getX() * v2.getY() * v3.getZ() + v1.getY() * v2.getZ() * v3.getX() + v1.getZ() * v2.getX() * v3.getY()) -
         (v1.getZ() * v2.getY() * v3.getX() + v1.getY() * v2.getX() * v3.getZ() + v1.getX() * v2.getZ() * v3.getY());
}

int triangle::CheckCollision(const ray& currentRay) const {
  vec3 e1 = (d2 - d1), e2 = (d3 - d1);

  // if not in plane
  double res1 = ThreeVecMul(currentRay.GetPos() - d1, e1, e2),
         res2 = ThreeVecMul(currentRay.GetPos() + currentRay.GetDir() * RAY_STEP - d1, e1, e2);
  if (res1 * res2 > 0)
    return 0;

  // if in plane
  vec3 n = this->GetNormalToPoint(currentRay);
  vec3 c1 = (currentRay.GetPos() - d1), 
       c2 = (currentRay.GetPos() - d2),
       c3 = (currentRay.GetPos() - d3),
       v1 = (d2 - d1), v2 = (d3 - d2), v3 = (d1 - d3);

  if ((n * crosMulVec3(c1, v1) >= 0 && n * crosMulVec3(c2, v2) >= 0 && n * crosMulVec3(c3, v3) >= 0) ||
      (n * crosMulVec3(c1, v1) <= 0 && n * crosMulVec3(c2, v2) <= 0 && n * crosMulVec3(c3, v3) <= 0))
    return 1;
  else
    return 0;
}

vec3 triangle::GetNormalToPoint(const ray& currentRay) const {
  vec3 e1 = d2 - d1, e2 = d3 - d1, normal, dir = currentRay.GetDir();
  double x, y, z;

  if (e1.getX() == 0 && e2.getX() == 0) {
    x = 1;
    y = z = 0;
  }
  else if (e1.getY() == 0 && e2.getY() == 0) {
    y = 1;
    x = z = 0;
  }
  else if (e1.getZ() == 0 && e2.getZ() == 0) {
    z = 1;
    x = y = 0;
  }
  else {
    x = 1;
    y = (e1.getZ() * e2.getX() - e1.getX() * e2.getZ()) / (e2.getZ() * e1.getY() - e2.getY() * e1.getZ());
    z = (e1.getY() * e2.getX() - e1.getX() * e2.getY()) / (e2.getZ() * e1.getY() - e2.getY() * e1.getZ());
  }

  normal = vec3(x, y, z).normal();

  if (cosVec3(dir, normal) > 0)
    normal = normal * (-1);

  return normal;
}
