#ifndef GKIT2LIGHTCMAKE_RAYTRACEIMAGEPROCESSING_H
#define GKIT2LIGHTCMAKE_RAYTRACEIMAGEPROCESSING_H

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
  void rayTraceExercice5();

  // TODO Explain what the function do
  void rayTraceStartingByColorSource();
};


#endif //GKIT2LIGHTCMAKE_RAYTRACEIMAGEPROCESSING_H
