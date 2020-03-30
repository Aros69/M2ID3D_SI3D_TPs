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

Point squareRootParametrization(Source source) {
  std::random_device seed;
  std::default_random_engine rng(seed());
  std::uniform_real_distribution<float> u01(0.f, 1.f);
  float r1 = u01(rng), r2 = u01(rng);
  return Point((1 - sqrt(r1)) * source.a
               + sqrt(r1) * (r2 * source.b + (1 - r2) * source.c));
}

Point square2TriangleParametrization(Source source) {
  std::random_device seed;
  std::default_random_engine rng(seed());
  std::uniform_real_distribution<float> u01(0.f, 0.5f);
  float r1 = u01(rng), r2 = u01(rng);
  float offset = r2 - r1;
  if (offset > 0) { r2 += offset; }
  else { r1 -= offset; }
  return Point(source.a * r1 + source.b * r2 + source.c * (1 - r1 - r2));
}

double calcBrbf(double kFactor, double mFactor, double cosTeta) {
  return (kFactor / M_PI) + (1 - kFactor) * ((mFactor + 8) / (8 * M_PI)) * cosTeta;
}