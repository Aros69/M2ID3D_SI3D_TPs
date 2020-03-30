#ifndef GKIT2LIGHTCMAKE_RAYTRACEIMAGEPROCESSING_H
#define GKIT2LIGHTCMAKE_RAYTRACEIMAGEPROCESSING_H

#include <chrono>
#include <random>

#include "image.h"
#include "orbiter.h"
#include "wavefront.h"

#include "../Utils/Utils.h"

class RayTraceImageProcessing {
private :
  Image image;
  Mesh mesh;
  BVH *bvh;
  Sources *sources;
  Orbiter camera;

public:
  RayTraceImageProcessing(const char *meshPath, const char *orbiterPath);

  ~RayTraceImageProcessing();

  // TODO Explain what the function do
  void rayTrace();

  void computePixel(int px, int py,
                    std::default_random_engine rng,
                    std::uniform_real_distribution<float> u01);

  Color exercice2Material(Hit hitInfo, Ray usedRay);

  Color exercice5DirectLightning(Hit hitInfo,
                                 Ray usedRay,
                                 std::default_random_engine rng,
                                 std::uniform_real_distribution<float> u01);
};


#endif //GKIT2LIGHTCMAKE_RAYTRACEIMAGEPROCESSING_H
