#include "RayTraceImageProcessing.h"
#include <string>

int main(const int argc, const char **argv) {
  std::string mesh_filename = "data/RayTracingData/cornell.obj",
      orbiter_filename = "data/RayTracingData/cornellOrbiter.txt",
      directLightSaveFile = "",//"data/Result/ZDirectLightEx5.taf",
      pathAndFilenameForSave = "data/Result/Render",
      rayTraceTechnique = "Ex5",
      noiseRemover = "",
      triangleParametrization = "square2Triangle",
      directionParametrization = "",
      pdf = "AreaSources";
  int nbRayDirect = 100, nbRayIndirect = 100;
  unsigned int imgWidth = 1024, imgHeight = 640;
  bool applyTonemapping = false, debug = false;

  const int nbPrefixe = 15;
  std::string parameterPrefixe[nbPrefixe] =
      {"-mesh", "-orbiter", "-directLightSave", "-savePathAndFilename", "-nbDirectRay",
       "-nbIndirectRay", "-tonemapping", "-rayTraceTechnique", "-noiseRemover",
       "-trianglePointParametrization", "-directionParametrization", "-pdf",
       "-imgWidth", "-imgHeight", "-debug"};
  for (int i = 2; i <= argc; ++i) {
    std::string programParameter = argv[i - 1];
    for (int j = 0; j < nbPrefixe; ++j) {
      if (programParameter.find(parameterPrefixe[j]) != std::string::npos) {
        /*std::cout << "Its a match : " << programParameter << " and "
                  << parameterPrefixe[j] << std::endl;*/
        std::string realParam = programParameter.substr(programParameter.find('=') + 1,
                                                        std::string::npos);

        switch (j) {
          case 0: {
            mesh_filename = realParam;
            break;
          }
          case 1: {
            orbiter_filename = realParam;
            break;
          }
          case 2: {
            directLightSaveFile = realParam;
            break;
          }
          case 3: {
            pathAndFilenameForSave = realParam;
            break;
          }
          case 4: {
            nbRayDirect = std::stoi(realParam);
            break;
          }
          case 5: {
            nbRayIndirect = std::stoi(realParam);
            break;
          }
          case 6: {
            applyTonemapping = std::stoi(realParam);
            break;
          }
          case 7: {
            rayTraceTechnique = realParam;
            break;
          }
          case 8: {
            noiseRemover = realParam;
            break;
          }
          case 9: {
            triangleParametrization = realParam;
            break;
          }
          case 10: {
            directionParametrization = realParam;
            break;
          }
          case 11: {
            pdf = realParam;
            break;
          }
          case 12: {
            imgWidth = std::stoi(realParam);
            break;
          }
          case 13: {
            imgHeight = std::stoi(realParam);
            break;
          }
          case 14: {
            debug = std::stoi(realParam);
            break;
          }
        }
      }
    }
  }
  /*std::cout << "Prefixe parametrer available :\n";
  for (int i = 0; i < nbPrefixe; ++i) {
    std::cout << "  " << parameterPrefixe[i] << std::endl;
  }*/

  std::cout << "Parameters used are :\n" <<
            "  mesh = " << mesh_filename << "\n" <<
            "  orbiter = " << orbiter_filename << "\n" <<
            "  directLightSave = " << directLightSaveFile << "\n" <<
            "  resultSavePath = " << pathAndFilenameForSave << "\n" <<
            "  rayTraceTechnique = " << rayTraceTechnique << "\n" <<
            "  noiseRemover = " << noiseRemover << "\n" <<
            "  triangleParam = " << triangleParametrization << "\n" <<
            "  directionParam = " << directionParametrization << "\n" <<
            "  pdf = " << pdf << "\n" <<
            "  nbDirectRay = " << nbRayDirect << "\n" <<
            "  nbIndirectRay = " << nbRayIndirect << "\n" <<
            "  imgWidth = " << imgWidth << "\n" <<
            "  imgHeight = " << imgHeight << "\n" <<
            "  applyTonemapping = " << applyTonemapping << "\n" <<
            "  debug = " << debug << std::endl;

  //printf("%s: '%s' '%s'\n", argv[0], mesh_filename, orbiter_filename);

  RayTraceImageProcessing p(mesh_filename, orbiter_filename, pathAndFilenameForSave,
                            directLightSaveFile, rayTraceTechnique, pdf,
                            triangleParametrization, directionParametrization,
                            nbRayDirect, nbRayIndirect, applyTonemapping, debug, imgWidth,
                            imgHeight);
  p.rayTrace();

  return 0;
}
