#ifndef __matr3_h__
#define __matr3_h__

#include <math.h>
#include <vector>
#include "vec3.h"

class matr3 {
private:
  std::vector<double> values;
public:
  matr3() {
    values = std::vector<double>(9, 0);
  };

  matr3(const std::vector<double>& newValues) {
    values = std::vector<double>(newValues);
  };

  double GetValue(const int& indW, const int& indH) const {
    if (indW < 0 || indW >= 3 || indH < 0 || indH >= 3)
      return 0;
    return values[indH * 3 + indW];
  }

  matr3 operator+(const matr3& mtr2) const {
    std::vector<double> newValues(9);

    for (int i = 0; i < 9; i++)
      newValues[i] = values[i] + mtr2.values[i];

    return matr3(newValues);
  }

  matr3 operator*(const matr3& mtr2) const {
    std::vector<double> newValues(9, 0);

    for (int x = 0; x < 3; x++)
      for (int y = 0; y < 3; y++)
        for (int k = 0; k < 3; k++)
          newValues[x + y * 3] += values[y * 3 + k] * mtr2.values[k * 3 + x];

    return matr3(newValues);
  }

  vec3 operator*(const vec3& vec) const {
    double x = vec.getX(), y = vec.getY(), z = vec.getZ();
    return vec3(values[0] * x + values[1] * y + values[2] * z,
                values[3] * x + values[4] * y + values[5] * z,
                values[6] * x + values[7] * y + values[8] * z);
  }

  matr3& operator=(const matr3& mtr2) {
    if (this == &mtr2)
      return *this;

    values = mtr2.values;
    return *this;
  }
};

matr3 MatrRotateX(const double& alpha) {
  std::vector<double> newValues(9);

  newValues[0] = 1;
  newValues[1] = newValues[2] = newValues[3] = newValues[6] = 0;
  newValues[4] = newValues[8] = cos(alpha);
  newValues[5] = (-1) * sin(alpha);
  newValues[7] = sin(alpha);

  return matr3(newValues);
}

matr3 MatrRotateY(const double& beta) {
  std::vector<double> newValues(9);

  newValues[4] = 1;
  newValues[1] = newValues[3] = newValues[5] = newValues[7] = 0;
  newValues[0] = newValues[8] = cos(beta);
  newValues[6] = (-1) * sin(beta);
  newValues[2] = sin(beta);

  return matr3(newValues);
}

matr3 MatrRotateZ(const double& gamma) {
  std::vector<double> newValues(9);

  newValues[8] = 1;
  newValues[2] = newValues[5] = newValues[6] = newValues[7] = 0;
  newValues[0] = newValues[4] = cos(gamma);
  newValues[1] = (-1) * sin(gamma);
  newValues[3] = sin(gamma);

  return matr3(newValues);
}

#endif // __matr_h__
