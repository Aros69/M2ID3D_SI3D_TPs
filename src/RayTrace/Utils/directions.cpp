
#include <cmath>
#include <random>

#include "vec.h"
#include "image.h"
#include "image_hdr.h"
#include "Utils.h"


// a remplacer par votre generation de directions
Vector direction(const float u1, const float u2, int isUniform) {
  /*// test, probablement faux...
  // generer une direction uniforme sur l'hemisphere
  float phi= 2.f * float(M_PI) * u2;
  float theta= u1 * float(M_PI / 2);
  float cos_theta= std::cos(theta);
  float sin_theta= std::sin(theta);
  return Vector(std::cos(phi) * sin_theta, std::sin(phi) * sin_theta, cos_theta);*/

  float a;
  if (isUniform == 1) {
    // GI Compenfium 34
    return Vector(randomPointHemisphereUniform(a));
  } else {
    // GI Compendium 35
    return Vector(randomPointHemisphereDistributed(a));
  }
}

// a remplacer par la densite supposee de la fonction de generation de directions
float pdf(const Vector &v, int isUniform) {
  // test, densite uniforme sur l'hemisphere
  //return (v.z < 0) ? 0 : 1 / float(2 * M_PI);
  float pdf;
  if (isUniform == 1) {
    // GI Compenfium 34
    randomPointHemisphereUniform(pdf);
  } else {
    // GI Compendium 35
    randomPointHemisphereDistributed(pdf);
  }
  return pdf;
}


Image density;

void sample(const int N, int isUniform) {
  std::random_device seed;
  std::mt19937 rng(seed());
  std::uniform_real_distribution<float> u01(0.f, 1.f);

  for (int i = 0; i < N; i++) {
    Vector v = direction(u01(rng), u01(rng), isUniform);

    float cos_theta = v.z;
    float phi = std::atan2(v.y, v.x) + float(M_PI);

    int x = phi / float(2 * M_PI) * density.width();
    int y = cos_theta * density.height();
    density(x, y) = density(x, y) + Color(1.f / float(N));
  }
}

Color eval(const Vector &v) {
  float cos_theta = v.z;
  float phi = std::atan2(v.y, v.x) + float(M_PI);

  int x = phi / float(2 * M_PI) * density.width();
  int y = cos_theta * density.height();
  return density(x, y);
}

void plot(const char *filename, int isUniform, const Vector &wo = Vector(0, 1, 0)) {
  const int image_size = 512;
  Image image(image_size, image_size);

  // camera Y up
  Vector Z = normalize(Vector(-1, 1, 1));
  Vector X = normalize(Vector(1, 1, 0));
  X = normalize(X - Z * dot(X, Z));
  Vector Y = -cross(X, Z);

  // loop over pixels
  for (int j = 0; j < image.height(); ++j)
    for (int i = 0; i < image.width(); ++i) {
      // intersection point on the sphere
      float x = -1.1f + 2.2f * (i + .5f) / (float) image.width();
      float y = -1.1f + 2.2f * (j + .5f) / (float) image.height();
      if (x * x + y * y > 1.0f)
        continue;

      float z = std::sqrt(1.0f - x * x - y * y);
      Vector v = normalize(x * X + y * Y + z * Z);
      if (v.z < 0)
        continue;

      // evaluate function
      Color value = eval(v) / pdf(v, isUniform);
      image(i, image.height() - 1 - j) = Color(value, 1);
    }

  write_image_hdr(image, filename);
}


int main(const int argc, const char **argv) {
  density = Image(512, 512);
  int isUniform = 1;
  if (argc == 2) {
    isUniform = std::stoi(argv[1]);
  }


  sample(1024 * 1024 * 16, isUniform);
  plot("sphere.hdr", isUniform);
  write_image_hdr(density, "density.hdr");

  return 0;
}
