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

Point randomPointHemisphereUniform(float & pdf){
  //std::cout<<"Uniform used"<<std::endl;
  std::random_device seed;
  std::default_random_engine rng(seed());
  std::uniform_real_distribution<float> u01(0.f, 1.f);
  float r1 = u01(rng), r2 = u01(rng);
  float x = cos(2*M_PI*r1)*sqrt(1-r2*r2);
  float y = sin(2*M_PI*r1)*sqrt(1-r2*r2);
  float z = r2;
  pdf = 1.0/(2.0*M_PI);
  return Point(x,y,z);
}
Point randomPointHemisphereDistributed(float & pdf){
  //std::cout<<"Distributed used"<<std::endl;
  std::random_device seed;
  std::default_random_engine rng(seed());
  std::uniform_real_distribution<float> u01(0.f, 1.f);
  float r1 = u01(rng), r2 = u01(rng);
  float x = cos(2*M_PI*r1)*sqrt(1-r2);
  float y = sin(2*M_PI*r1)*sqrt(1-r2);
  float z = sqrt(r2);
  pdf = z/M_PI;
  return Point(x,y,z);
}

Vector normal(Source s, Point p){
  Vector v1(p, s.a);
  Vector v2(p, s.b);
  return normalize(cross(v1, v2));
}

Vector normalOrientationIncomingRay(Vector normal, Ray incomingRay){
  if (dot(normal, incomingRay.d) > 0) {
    normal = -normal;
  }
  return normal;
}

float aireTriangle(vec3 a, vec3 b, vec3 c) {
  float ab = distance(a, b);
  float ac = distance(a, c);
  float bc = distance(c, b);
  float s  = (ab+ac+bc)/2.0f;
  return sqrt(s*(s-ab)*(s-ac)*(s-bc));
}