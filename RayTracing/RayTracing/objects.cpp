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

material object::GetMaterial(const vec3& point, const vec3& dir, const ray& rayToCast) const {
  return mater;
}

int object::CheckCollision(const ray& currentRay, vec3& collisionDot, const int& isFindLight) const {
  return -1;
}

vec3 object::GetNormalToPoint(const vec3& point, const vec3& dir, const ray& rayToCast) const {
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

int plane::CheckCollision(const ray& currentRay, vec3& collisionDot, const int& isFindLight) const {
  if (currentRay.GetDir() * normal == 0)
    return 0;

  double d = (-1) * (normal * center), 
    t = -(currentRay.GetPos() * normal + d) / (currentRay.GetDir() * normal);

  if (t <= 0 || t > RAY_INF)
    return 0;
  collisionDot = currentRay.GetPos() + currentRay.GetDir() * (t - RAY_SHIFT);
  return 1;
}

vec3 plane::GetNormalToPoint(const vec3& point, const vec3& dir, const ray& rayToCast) const {
  if (cosVec3(normal, dir) > 0)
    return normal.normal() * (-1);
  else
    return normal.normal();
}

rectangle::rectangle() {
  this->mater = material();

  this->normal = vec3();
  this->LDcorn = vec3();
  this->RDcorn = vec3();
  this->LUcorn = vec3();
}

rectangle::rectangle(const vec3& LDcorn, const vec3& RDcorn, const vec3& LUcorn, const material& mtr) {
  this->mater = mtr;

  this->LDcorn = LDcorn;
  this->RDcorn = RDcorn;
  this->LUcorn = LUcorn;

  vec3 v1 = (LUcorn - LDcorn),
    v2 = (RDcorn - LDcorn), normal;
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

  this->normal = vec3(x, y, z).normal();
}

double rectangle::ThreeVecMul(const vec3& v1, const vec3& v2, const vec3& v3) const {
  return (v1.getX() * v2.getY() * v3.getZ() + v1.getY() * v2.getZ() * v3.getX() + v1.getZ() * v2.getX() * v3.getY()) -
         (v1.getZ() * v2.getY() * v3.getX() + v1.getY() * v2.getX() * v3.getZ() + v1.getX() * v2.getZ() * v3.getY());
}

int rectangle::CheckCollision(const ray& currentRay, vec3& collisionDot, const int& isFindLight) const {
  // find collision dot with plane
  if (currentRay.GetDir() * normal == 0)
    return 0;

  double d = (-1) * (normal * LDcorn),
    t = -(currentRay.GetPos() * normal + d) / (currentRay.GetDir() * normal);

  if (t <= 0 || t > RAY_INF)
    return 0;

  // check if we are in plane piece
  vec3 dotOnPlane = currentRay.GetPos() + currentRay.GetDir() * t,
       newVec = (dotOnPlane - LDcorn), v1 = (RDcorn - LDcorn), v2 = (LUcorn - LDcorn),
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

  if (alpha >= 0 && alpha <= 1 && beta >= 0 && beta <= 1) {
    collisionDot = currentRay.GetPos() + currentRay.GetDir() * (t - RAY_SHIFT);
    if ((collisionDot - LDcorn) * (currentRay.GetPos() - LDcorn) < 0)
      collisionDot = currentRay.GetPos() + currentRay.GetDir() * (t + RAY_SHIFT);
    return 1;
  }
  else
    return 0;
}

vec3 rectangle::GetNormalToPoint(const vec3& point, const vec3& dir, const ray& rayToCast) const {
  if (cosVec3(normal, dir) > 0)
    return normal.normal() * (-1);
  else
    return normal.normal();
}

chessRect::chessRect(const vec3& LDcorn, const vec3& RDcorn, const vec3& LUcorn, const int& gridSizeW, const int& gridSizeH, const material& mtr1, const material& mtr2) {
  this->mater = mtr1;
  this->secondMaterial = mtr2;

  this->LDcorn = LDcorn;
  this->RDcorn = RDcorn;
  this->LUcorn = LUcorn;

  vec3 v1 = (LUcorn - LDcorn),
    v2 = (RDcorn - LDcorn), normal;
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

  this->normal = vec3(x, y, z).normal();

  this->gridSizeW = gridSizeW;
  this->gridSizeH = gridSizeH;
}

material chessRect::GetMaterial(const vec3& point, const vec3& dir, const ray& rayToCast) const {
  vec3 v1 = (LUcorn - LDcorn), v2 = (RDcorn - LDcorn),
       dotOnPlane = point, newVec = (dotOnPlane - LDcorn),
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

int box::CheckCollision(const ray& currentRay, vec3& collisionDot, const int& isFindLight) const {
  vec3 dot5 = dot4 + (dot2 - dot1),
       dot6 = dot4 + (dot3 - dot1),
       dot7 = dot6 + (dot2 - dot1),
       vecColl;

  rectangle p1 = rectangle(dot1, dot2, dot4, mater),
            p2 = rectangle(dot1, dot3, dot4, mater),
            p3 = rectangle(dot1, dot3, dot2, mater),
            p4 = rectangle(dot4, dot6, dot5, mater),
            p5 = rectangle(dot5, dot2, dot7, mater),
            p6 = rectangle(dot6, dot3, dot7, mater);

  double minDist = RAY_INF;
  vec3 colDot, nearestColDot;

  if (p1.CheckCollision(currentRay, colDot, isFindLight) == 1 && (colDot - currentRay.GetPos()).length() < minDist) {
    minDist = (colDot - currentRay.GetPos()).length();
    nearestColDot = colDot;
  }
  if (p2.CheckCollision(currentRay, colDot, isFindLight) == 1 && (colDot - currentRay.GetPos()).length() < minDist) {
    minDist = (colDot - currentRay.GetPos()).length();
    nearestColDot = colDot;
  }
  if (p3.CheckCollision(currentRay, colDot, isFindLight) == 1 && (colDot - currentRay.GetPos()).length() < minDist) {
    minDist = (colDot - currentRay.GetPos()).length();
    nearestColDot = colDot;
  }
  if (p4.CheckCollision(currentRay, colDot, isFindLight) == 1 && (colDot - currentRay.GetPos()).length() < minDist) {
    minDist = (colDot - currentRay.GetPos()).length();
    nearestColDot = colDot;
  }
  if (p5.CheckCollision(currentRay, colDot, isFindLight) == 1 && (colDot - currentRay.GetPos()).length() < minDist) {
    minDist = (colDot - currentRay.GetPos()).length();
    nearestColDot = colDot;
  }
  if (p6.CheckCollision(currentRay, colDot, isFindLight) == 1 && (colDot - currentRay.GetPos()).length() < minDist) {
    minDist = (colDot - currentRay.GetPos()).length();
    nearestColDot = colDot;
  }

  if (minDist == RAY_INF)
    return 0;
  else {
    collisionDot = nearestColDot;
    return 1;
  }
}

vec3 box::GetNormalToPoint(const vec3& point, const vec3& dir, const ray& rayToCast) const {
  vec3 dot5 = dot4 + (dot2 - dot1),
       dot6 = dot4 + (dot3 - dot1),
       dot7 = dot6 + (dot2 - dot1),
       vecColl;

  rectangle p1 = rectangle(dot1, dot2, dot4, mater),
            p2 = rectangle(dot1, dot3, dot4, mater),
            p3 = rectangle(dot1, dot3, dot2, mater),
            p4 = rectangle(dot4, dot6, dot5, mater),
            p5 = rectangle(dot5, dot2, dot7, mater),
            p6 = rectangle(dot6, dot3, dot7, mater),
            nearestPlane;

  double minDist = RAY_INF;
  vec3 colDot;

  if (p1.CheckCollision(rayToCast, colDot) == 1 && (colDot - rayToCast.GetPos()).length() < minDist) {
    minDist = (colDot - rayToCast.GetPos()).length();
    nearestPlane = p1;
  }
  if (p2.CheckCollision(rayToCast, colDot) == 1 && (colDot - rayToCast.GetPos()).length() < minDist) {
    minDist = (colDot - rayToCast.GetPos()).length();
    nearestPlane = p2;
  }
  if (p3.CheckCollision(rayToCast, colDot) == 1 && (colDot - rayToCast.GetPos()).length() < minDist) {
    minDist = (colDot - rayToCast.GetPos()).length();
    nearestPlane = p3;
  }
  if (p4.CheckCollision(rayToCast, colDot) == 1 && (colDot - rayToCast.GetPos()).length() < minDist) {
    minDist = (colDot - rayToCast.GetPos()).length();
    nearestPlane = p4;
  }
  if (p5.CheckCollision(rayToCast, colDot) == 1 && (colDot - rayToCast.GetPos()).length() < minDist) {
    minDist = (colDot - rayToCast.GetPos()).length();
    nearestPlane = p5;
  }
  if (p6.CheckCollision(rayToCast, colDot) == 1 && (colDot - rayToCast.GetPos()).length() < minDist) {
    minDist = (colDot - rayToCast.GetPos()).length();
    nearestPlane = p6;
  }

  if (minDist == RAY_INF)
    return vec3(1, 0, 0);
  else 
    return nearestPlane.GetNormalToPoint(point, dir);
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

int sphere::CheckCollision(const ray& currentRay, vec3& collisionDot, const int& isFindLight) const {
  vec3 l = center - currentRay.GetPos();
  double t1 = l * currentRay.GetDir();

  if (t1 <= 0)
    return 0;
  else {
    double d = l * l - t1 * t1;
    if (d > radius * radius)
      return 0;

    double t2 = sqrt(radius * radius - d), t = t1 - t2;
    if (t > RAY_INF)
      return 0;

    vec3 cD = currentRay.GetPos() + currentRay.GetDir() * t;
    collisionDot = center + (cD - center) * (1 + RAY_SHIFT);
    return 1;
  }
}

vec3 sphere::GetNormalToPoint(const vec3& point, const vec3& dir, const ray& rayToCast) const {
  return (point - center).normal();
}

boxAndSphere::boxAndSphere(const vec3& dot1, const vec3& dot2, const vec3& dot3, const vec3& dot4, const vec3& center, const double& radius, const material& mtr1, const material& mtr2) {
  this->bx = box(dot1, dot2, dot3, dot4, mtr1);
  this->sph = sphere(center, radius, mtr2);

  this->mater = mtr1;
  this->secondMtr = mtr2;
}

int sphere::CheckInnerSphereCollision(const ray& currentRay, vec3& collisionDot, const int& isFindLight) const {
  vec3 l = center - currentRay.GetPos();
  double t1 = l * currentRay.GetDir();

  if (t1 <= 0)
    return 0;
  else {
    double d = l * l - t1 * t1;
    if (d > radius * radius)
      return 0;

    double t2 = sqrt(radius * radius - d), t = t1 + t2;
    if (t > RAY_INF)
      return 0;

    vec3 cD = currentRay.GetPos() + currentRay.GetDir() * t;
    collisionDot = center + (cD - center) * (1 + RAY_SHIFT);
    return 1;
  }
}

int boxAndSphere::CheckCollision(const ray& currentRay, vec3& collisionDot, const int& isFindLight) const {
  vec3 sphereCol, boxCol;

  if ((bx.CheckCollision(currentRay, boxCol) == 1 && (boxCol - sph.GetCenter()).length() >= sph.GetRadius())) {
    collisionDot = boxCol;
    return 1;
  }
  else if (sph.CheckInnerSphereCollision(currentRay, sphereCol) == 1 && bx.isInBox(sphereCol) == 1) {
    collisionDot = sph.GetCenter() + (sphereCol - sph.GetCenter()) * ((1 - RAY_SHIFT) / (1 + RAY_SHIFT));
    return 1;
  }
  return 0;
}

vec3 boxAndSphere::GetNormalToPoint(const vec3& point, const vec3& dir, const ray& rayToCast) const {
  vec3 sphereCol, boxCol;

  if (bx.CheckCollision(rayToCast, boxCol) == 1 && (boxCol - sph.GetCenter()).length() > sph.GetRadius())
    return bx.GetNormalToPoint(point, dir);
  else {
    sph.CheckCollision(rayToCast, sphereCol);
    return (sphereCol - sph.GetCenter()).normal() * (-1);
  }
}

material boxAndSphere::GetMaterial(const vec3& point, const vec3& dir, const ray& rayToCast) const {
  vec3 boxCol;

  if (bx.CheckCollision(rayToCast, boxCol) == 1 && (boxCol - sph.GetCenter()).length() > sph.GetRadius())
    return mater;
  else
    return secondMtr;
}

triangle::triangle(const vec3& dot1, const vec3& dot2, const vec3& dot3, const material& mtr) {
  this->mater = mtr;

  d1 = dot1;
  d2 = dot2;
  d3 = dot3;

  vec3 e1 = d2 - d1, e2 = d3 - d1;
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

  this->normal = vec3(x, y, z).normal();
}

double triangle::ThreeVecMul(const vec3& v1, const vec3& v2, const vec3& v3) const {
  return (v1.getX() * v2.getY() * v3.getZ() + v1.getY() * v2.getZ() * v3.getX() + v1.getZ() * v2.getX() * v3.getY()) -
         (v1.getZ() * v2.getY() * v3.getX() + v1.getY() * v2.getX() * v3.getZ() + v1.getX() * v2.getZ() * v3.getY());
}

int triangle::CheckCollision(const ray& currentRay, vec3& collisionDot, const int& isFindLight) const {
  // if not in plane
  if (currentRay.GetDir() * normal == 0)
    return 0;

  double d = (-1) * (normal * d1),
    t = -(currentRay.GetPos() * normal + d) / (currentRay.GetDir() * normal);

  if (t <= 0 || t > RAY_INF)
    return 0;

  // if in plane
  vec3 e1 = (d2 - d1), e2 = (d3 - d1), v = currentRay.GetPos() + currentRay.GetDir() * t - d1, xPart, yPart;
  double alpha = acos(cosVec3(v, e1)), beta = acos(cosVec3(v, e2)), total = acos(cosVec3(e1, e2));

  if (alpha > total || beta > total)
    return 0;

  if (alpha + beta >= MATH_PI / 2 - 0.0001 && alpha + beta <= MATH_PI / 2 + 0.0001) {
    xPart = e1.normal() * (cosVec3(v, e1) * v.length());
    yPart = e2.normal() * (cosVec3(v, e2) * v.length());
  }
  else {
    xPart = e1.normal() * (cosVec3(v, e1) - 1.0f / tan(alpha + beta) * sin(alpha)) * v.length();
    yPart = e2.normal() * (cosVec3(v, e2) - 1.0f / tan(alpha + beta) * sin(beta)) * v.length();
  }

  double a, b;

  if (e1.getX() != 0)
    a = xPart.getX() / e1.getX();
  else if (e1.getY() != 0)
    a = xPart.getY() / e1.getY();
  else
    a = xPart.getZ() / e1.getZ();

  if (e2.getX() != 0)
    b = yPart.getX() / e2.getX();
  else if (e2.getY() != 0)
    b = yPart.getY() / e2.getY();
  else
    b = yPart.getZ() / e2.getZ();

  //if (a >= 0 && b >= 0 && a + b <= 1) {
  if (a >= 0 - RAY_SHIFT && b >= 0 - RAY_SHIFT && a + b <= 1 + RAY_SHIFT) {
    collisionDot = currentRay.GetPos() + currentRay.GetDir() * (t - RAY_SHIFT);
    return 1;
  }
  else
    return 0;
}

vec3 triangle::GetNormalToPoint(const vec3& point, const vec3& dir, const ray& rayToCast) const {
  if (cosVec3(normal, dir) > 0)
    return normal.normal() * (-1);
  else
    return normal.normal();
}
