#ifndef GKIT2LIGHTCMAKE_RAYTRACEIMAGEPROCESSING_H
#define GKIT2LIGHTCMAKE_RAYTRACEIMAGEPROCESSING_H

#include <chrono>
#include <random>
#include <omp.h>

#include "image.h"
#include "orbiter.h"
#include "wavefront.h"

#include "../Utils/Utils.h"

class RayTraceImageProcessing {
public:
  int pxDebug, pyDebug;

private :
  Image imageResultat;
  Image directLighthningImage;
  Mesh mesh;
  BVH *bvh;
  Sources *sources;
  Orbiter camera;
  std::string pathAndFilename = "data/Result/TempRender";
  std::string directLightningFile = "";
  std::string renderWanted = "Ex5"; // Possible are : "Ex2", "Ex5", "Ambiant", "Ex7"
  std::string pdfTechnique = "AreaSources"; // Possible Values : "AreaSources", "CosAndDistance", "Combined"
  int nbDirectRay = 10, nbIndirectRay = 10;
  bool useTonemapping = false, directLightningBeforeBounceDone = false, showColorOverFlow = false;
  Point (*triangleParametrization)(Source source);
  Point (*directionParametrization)(float & pdf);

public:
  RayTraceImageProcessing(std::string meshPath, std::string orbiterPath,
                          std::string savePathFile, std::string directLightningSave,
                          std::string renderTechnique, std::string pdfMethod,
                          std::string triangleParam, std::string directionParam,
                          int directRay, int indirectRay, bool applyTonemapping,
                          bool debug, unsigned int resImageWidth,
                          unsigned int resImageHeight);

  ~RayTraceImageProcessing();

  void rayTrace();

  void computePixel(int px, int py,
                    std::default_random_engine &rng,
                    std::uniform_real_distribution<float> &u01);

  Color exercice2Material(Hit hitInfo, Ray usedRay);

  Color exercice5DirectLightning(Hit hitInfo,
                                 Ray usedRay,
                                 std::default_random_engine &rng,
                                 std::uniform_real_distribution<float> &u01);

  Color occlusionAmbiante(Hit hitInfo, Ray usedRay);

  Color exercice7Rebond(Hit hitInfo, Ray usedRay,
                        std::default_random_engine &rng,
                        std::uniform_real_distribution<float> &u01,
                        int px, int py);

  void applyTonemapping();

  void hardSaveDirectLightning();

  void hardLoadDirectLightning();
};


#endif //GKIT2LIGHTCMAKE_RAYTRACEIMAGEPROCESSING_H
