#include "Utils.h"

Vector normal(const Hit &hit, const TriangleData &triangle) {
  return normalize(
      (1 - hit.u - hit.v) * Vector(triangle.na) + hit.u * Vector(triangle.nb) +
      hit.v * Vector(triangle.nc));
}

Point point(const Hit &hit, const TriangleData &triangle) {
  return (1 - hit.u - hit.v) * Point(triangle.a) + hit.u * Point(triangle.b) +
         hit.v * Point(triangle.c);
}

Point point(const Hit &hit, const Ray &ray) {
  return ray.o + hit.t * ray.d;
}

double calcBrbf(double kFactor, double mFactor, double cosTeta) {
  return (kFactor / M_PI) + (1 - kFactor) * ((mFactor + 8) / (8 * M_PI)) * cosTeta;
}