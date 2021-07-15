#include <fstream>
#include <sstream>
#include <iterator>

#include <queue>

#include "matr3.h"
#include "model.h"


static vec3 crosMulVec3(vec3& v1, vec3& v2) {
  return vec3((v1.getY() * v2.getZ() - v2.getY() * v1.getZ()),
    (v1.getX() * v2.getZ() - v2.getX() * v1.getZ()) * (-1),
    (v1.getX() * v2.getY() - v2.getX() * v1.getY()));
}

modelTriangle::modelTriangle() {
  d1 = d2 = d3 = normal = vec3();
}

modelTriangle::modelTriangle(const vec3& dot1, const vec3& dot2, const vec3& dot3) {
  d1 = dot1;
  d2 = dot2;
  d3 = dot3;

  vec3 e1 = d2 - d1, e2 = d3 - d1;

  normal = crosMulVec3(e2, e1).normal();
}

double modelTriangle::cosVec3(const vec3& v1, const vec3& v2) const {
  return (v1.getX() * v2.getX() + v1.getY() * v2.getY() + v1.getZ() * v2.getZ()) / (v1.length() * v2.length());
}

double modelTriangle::ThreeVecMul(const vec3& v1, const vec3& v2, const vec3& v3) const {
  return (v1.getX() * v2.getY() * v3.getZ() + v1.getY() * v2.getZ() * v3.getX() + v1.getZ() * v2.getX() * v3.getY()) -
    (v1.getZ() * v2.getY() * v3.getX() + v1.getY() * v2.getX() * v3.getZ() + v1.getX() * v2.getZ() * v3.getY());
}

int modelTriangle::CheckCollision(const ray& currentRay, vec3& collisionDot) const {
  // if not in plane
  if (currentRay.GetDir() * normal == 0)
    return 0;

  double t = (normal * d1 - currentRay.GetPos() * normal) / (currentRay.GetDir() * normal);

  if (t <= 0 || t > RAY_INF)
    return 0;

  // if in plane
  vec3 e1 = (d2 - d1), e2 = (d3 - d1), v = (currentRay.GetPos() + currentRay.GetDir() * t) - d1, xPart, yPart;
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

  if (a >= 0 && b >= 0 && a + b <= 1) {
    collisionDot = currentRay.GetPos() + currentRay.GetDir() * (t - RAY_SHIFT);
    if ((collisionDot - d1) * (currentRay.GetPos() - d1) < 0)
      collisionDot = currentRay.GetPos() + currentRay.GetDir() * (t + RAY_SHIFT);
    return 1;
  }
  else
    return 0;
}

vec3 modelTriangle::GetNormalToPoint(const vec3& point, const vec3& dir, const ray& rayToCast) const {
  if (cosVec3(normal, dir) > 0)
    return normal.normal() * (-1);
  else
    return normal.normal();
}

octoTree::octoTree() {
  minX = maxX = minY = maxY = minZ = maxZ = 0;
  siblings = std::vector<octoTree*>(8, nullptr);
}

octoTree::octoTree(const double& minXn, const double& maxXn, const double& minYn, const double& maxYn, const double& minZn, const double& maxZn) {
  minX = minXn;
  maxX = maxXn;
  minY = minYn;
  maxY = maxYn;
  minZ = minZn;
  maxZ = maxZn;
  siblings = std::vector<octoTree*>(8, nullptr);
}

void octoTree::SplitSibling(void) {
  siblings = std::vector<octoTree*>(8, nullptr);
  siblings[0] = new octoTree((minX + maxX) / 2, maxX, (minY + maxY) / 2, maxY, (minZ + maxZ) / 2, maxZ);
  siblings[1] = new octoTree(minX, (minX + maxX) / 2, (minY + maxY) / 2, maxY, (minZ + maxZ) / 2, maxZ);
  siblings[2] = new octoTree(minX, (minX + maxX) / 2, minY, (minY + maxY) / 2, (minZ + maxZ) / 2, maxZ);
  siblings[3] = new octoTree((minX + maxX) / 2, maxX, minY, (minY + maxY) / 2, (minZ + maxZ) / 2, maxZ);
  siblings[4] = new octoTree((minX + maxX) / 2, maxX, (minY + maxY) / 2, maxY, minZ, (minZ + maxZ) / 2);
  siblings[5] = new octoTree(minX, (minX + maxX) / 2, (minY + maxY) / 2, maxY, minZ, (minZ + maxZ) / 2);
  siblings[6] = new octoTree(minX, (minX + maxX) / 2, minY, (minY + maxY) / 2, minZ, (minZ + maxZ) / 2);
  siblings[7] = new octoTree((minX + maxX) / 2, maxX, minY, (minY + maxY) / 2, minZ, (minZ + maxZ) / 2);

  modelTriangle triangleToAdd;
  while (!triangles.empty()) {
    triangleToAdd = triangles.front();
    triangles.pop_front();

    for (int i = 0; i < 8; i++)
      if (siblings[i]->IsTriangleInOctoTree(triangleToAdd) == 1)
        siblings[i]->triangles.push_back(triangleToAdd);
  }

  for (int i = 0; i < 8; i++)
    if (siblings[i]->triangles.size() > 20)
      siblings[i]->SplitSibling();
}

// Cohen–Sutherland algorithm
#define BOTTOM 1              // 000001
#define TOP 2                 // 000010
#define RIGHT 4               // 000100
#define LEFT 8                // 001000
#define NEAR 16               // 010000
#define FAR 32                // 100000

unsigned char octoTree::CountCode(const vec3& d1) const {
  unsigned char d1Code = 0;

  if (d1.getX() < minX)
    d1Code += FAR;
  else if (d1.getX() > maxX)
    d1Code += NEAR;

  if (d1.getY() < minY)
    d1Code += LEFT;
  else if (d1.getY() > maxY)
    d1Code += RIGHT;

  if (d1.getZ() < minZ)
    d1Code += BOTTOM;
  else if (d1.getZ() > maxZ)
    d1Code += TOP;

  return d1Code;
}

int octoTree::IsSegmentInOctoTree(const vec3& d1, const vec3& d2) const {
  unsigned char d1Code = CountCode(d1), d2Code = CountCode(d2), totalCode = 0;

  if (d1Code == 0 || d2Code == 0)
    return 1;
  else if ((d1Code & d2Code) == 0)
    return 1;
  else
    return 0;
}

octoTree::octoTree(std::list<modelTriangle>& trianglesToAdd, const double& minXn, const double& maxXn, const double& minYn, const double& maxYn, const double& minZn, const double& maxZn) {
  minX = minXn;
  maxX = maxXn;
  minY = minYn;
  maxY = maxYn;
  minZ = minZn;
  maxZ = maxZn;

  triangles = trianglesToAdd;
  
  this->SplitSibling();
}

int octoTree::IsTriangleInOctoTree(const modelTriangle& mt) const {
  if (IsInOctoTree(mt.GetD1()) || IsInOctoTree(mt.GetD2()) || IsInOctoTree(mt.GetD3()))
    return 1;
  else if (IsSegmentInOctoTree(mt.GetD1(), mt.GetD2()) ||
           IsSegmentInOctoTree(mt.GetD1(), mt.GetD3()) ||
           IsSegmentInOctoTree(mt.GetD2(), mt.GetD3()))
    return 1;
  return 0;
}

int octoTree::IsInOctoTree(const vec3& v) const {
  double x = v.getX(),
         y = v.getY(),
         z = v.getZ();

  if (x >= minX && x <= maxX && y >= minY && y <= maxY && z >= minZ && z <= maxZ)
    return 1;
  else
    return 0;
}

int model::ParseIntInStr(std::string str) {
  int res = 0, i = 0;

  while (str[i] >= '0' && str[i] <= '9')
    res = res * 10 + (str[i++] - '0');

  return res;
}

model::model(const material& mtr, std::string filename, const double& size, double alpha, double beta, double gamma, vec3 v1) {
  mater = mtr;

  std::ifstream fileStream(filename);
  std::string oneStringFromFile;

  std::vector<vec3> vertexes(30000);
  std::list<modelTriangle> triangles;
  int lastIndex = 0, capasity = 1, triangleNo = 0;

  double x, y, z,
    maxX = -999, minX = 999, maxY = -999, minY = 999, maxZ = -999, minZ = 999;
  matr3 matrRot = MatrRotateX(alpha) * MatrRotateY(beta) * MatrRotateZ(gamma);
  vec3 newVertex;

  int ind1, ind2, ind3;

  double medX = 0, medY = 0, medZ = 0;

  while (getline(fileStream, oneStringFromFile)) {
    std::stringstream sStream(oneStringFromFile);
    std::istream_iterator<std::string> wordsInStream(sStream);

    if (oneStringFromFile == "")
      continue;

    if ((*wordsInStream) == "v") {
      wordsInStream++;

      if (lastIndex >= 30000 * capasity)
        vertexes.resize(30000 * (++capasity));

      x = atof((*wordsInStream++).c_str());
      y = atof((*wordsInStream++).c_str());
      z = atof((*wordsInStream++).c_str());

      newVertex = (matrRot * vec3(x, y, z) + v1) * size;

      if (minX > newVertex.getX())
        minX = newVertex.getX();
      if (maxX < newVertex.getX())
        maxX = newVertex.getX();

      if (minY > newVertex.getY())
        minY = newVertex.getY();
      if (maxY < newVertex.getY())
        maxY = newVertex.getY();

      if (minZ > newVertex.getZ())
        minZ = newVertex.getZ();
      if (maxZ < newVertex.getZ())
        maxZ = newVertex.getZ();

      medX += newVertex.getX(), medY += newVertex.getY(), medZ += newVertex.getZ();

      vertexes[lastIndex++] = newVertex;
    }
    else if ((*wordsInStream) == "f") {
      wordsInStream++;

      ind1 = ParseIntInStr((*wordsInStream++));
      ind2 = ParseIntInStr((*wordsInStream++));
      ind3 = ParseIntInStr((*wordsInStream++));

      if (ind1 - 1 < lastIndex && ind2 - 1 < lastIndex && ind3 - 1 < lastIndex)
        triangles.push_back(modelTriangle(vertexes[ind1 - 1], vertexes[ind2 - 1], vertexes[ind3 - 1]));

      while (wordsInStream != std::istream_iterator<std::string>()) {
        ind2 = ind3;
        ind3 = atoi((*wordsInStream++).c_str());
        triangles.push_back(modelTriangle(vertexes[ind1 - 1], vertexes[ind2 - 1], vertexes[ind3 - 1]));
      }

      triangleNo++;
    }
    else
      continue;
  }

  medX /= lastIndex;
  medY /= lastIndex;
  medZ /= lastIndex;

  trianglesTree = new octoTree(triangles, minX, maxX, minY, maxY, minZ, maxZ);
}

static modelTriangle lastHittenTriangle;

int model::CheckCollision(const ray& currentRay, vec3& collisionDot, const int& isFindLight) const {
  octoTree* currentOctoTree;
  std::queue<octoTree*> octosToSeek;
  std::list<octoTree*> octosToCheck;
  vec3 d1 = currentRay.GetPos(), d2 = currentRay.GetPos() + currentRay.GetDir() * RAY_INF;
  std::list<modelTriangle> trianglesToCheck;

  if (trianglesTree->IsSegmentInOctoTree(d1, d2) == 0)
    return 0;
  else {
    octosToSeek.push(trianglesTree);

    while (octosToSeek.size() != 0) {
      currentOctoTree = octosToSeek.front();
      octosToSeek.pop();

      if (currentOctoTree->siblings[0] != nullptr) {
        for (int i = 0; i < 8; i++)
          if (currentOctoTree->siblings[i]->IsSegmentInOctoTree(d1, d2))
            octosToSeek.push(currentOctoTree->siblings[i]);
      }
      else
        octosToCheck.push_back(currentOctoTree);
    }
  }

  vec3 collDot, nearestCollDot;
  modelTriangle nearestTriangle;
  double minDist = RAY_INF;
  for (auto& oct : octosToCheck)
    for (auto& trngl : oct->triangles)
      if (trngl.CheckCollision(currentRay, collDot) == 1) {
        if ((collDot - currentRay.GetPos()).length() < minDist) {
          minDist = (collDot - currentRay.GetPos()).length();
          nearestTriangle = trngl;
          nearestCollDot = collDot;

          if (isFindLight == 0)
            lastHittenTriangle = nearestTriangle;
        }
      }
   
  if (minDist == RAY_INF)
    return 0;
  else {
    collisionDot = nearestCollDot;
    return 1;
  }
}

vec3 model::GetNormalToPoint(const vec3& point, const vec3& dir, const ray& rayToCast) const {
  return lastHittenTriangle.GetNormalToPoint(point, dir, rayToCast);
}

void model::DeleteObject(void) const {
  std::queue<octoTree*> nodesToDelete;
  octoTree* nodeToDelete;

  nodesToDelete.push(trianglesTree);
  while (!nodesToDelete.empty()) {
    nodeToDelete = nodesToDelete.front();
    nodesToDelete.pop();

    if (nodeToDelete != nullptr) {
      for (int i = 0; i < 8; i++)
        nodesToDelete.push(nodeToDelete->siblings[i]);

      delete nodeToDelete;
    }
  }
}
