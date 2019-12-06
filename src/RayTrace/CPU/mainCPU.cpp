#include "RayTraceImageProcessing.h"

int main(const int argc, const char **argv) {
  const char *mesh_filename = "data/RayTracingData/cornell.obj";
  const char *orbiter_filename = "data/RayTracingData/cornellBetterOrbiter.txt";

  if (argc > 1) mesh_filename = argv[1];
  if (argc > 2) orbiter_filename = argv[2];

  printf("%s: '%s' '%s'\n", argv[0], mesh_filename, orbiter_filename);

  RayTraceImageProcessing p(mesh_filename, orbiter_filename);
  p.rayTraceExercice5();

  return 0;
}
