#ifndef __VEC3_H__
#define __VEC3_H__

#include <math.h>

class vec3 {
private:
  double X, Y, Z;

  float fastRevSqrt(float number) const {
    int i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y = number;
    i = *(long*)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(float*)&i;
    y = y * (threehalfs - (x2 * y * y));
    
    return y;
  }
public:
  // constructor #1
  vec3() {
    X = Y = 0;
  }

  // constructor #2
  vec3(double dotX, double dotY, double dotZ) {
    X = dotX;
    Y = dotY;
    Z = dotZ;
  }

  // methods
  double getX() const {
    return X;
  }

  double getY() const {
    return Y;
  }

  double getZ() const {
      return Z;
  }

  vec3 normal() const {
      if (X == 0 && Y == 0 && Z == 0)
      return vec3(0, 0, 0);

    double length = sqrt(X * X + Y * Y + Z * Z);
    return vec3(X / length, Y / length, Z / length);
  }

  vec3 fastNormal() const {
    if (X == 0 && Y == 0 && Z == 0)
      return vec3(0, 0, 0);

    float length = fastRevSqrt(float(X * X + Y * Y + Z * Z));
    return vec3(X * length, Y * length, Z * length);
  }

  double vecMul(const vec3& vec) const {
    return X * vec.X + Y * vec.Y + Z * vec.Z;
  }

  double length() const {
    return sqrt(X * X + Y * Y + Z * Z);
  }

  /*
   * operators
   */
  vec3 operator+(const vec3& vec) const {
    return vec3(X + vec.X, Y + vec.Y, Z + vec.Z);
  }
  vec3 operator-(const vec3& vec) const {
    return vec3(X - vec.X, Y - vec.Y, Z - vec.Z);
  }

  vec3 operator*(double mul) const {
    return vec3(X * mul, Y * mul, Z * mul);
  }

  double operator*(const vec3& vec) const {
    return X * vec.X + Y * vec.Y + Z * vec.Z;
  }

  vec3& operator=(const vec3& vec) {
    if (this == &vec)
      return *this;

    X = vec.X;
    Y = vec.Y;
    Z = vec.Z;
    return *this;
  }
};

#endif // __VEC3_H__
