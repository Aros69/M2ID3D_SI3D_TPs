#include <SDL_types.h>
#include "image_hdr.h"
#include "image_io.h"

#include "RayTraceImageProcessing.h"

RayTraceImageProcessing::RayTraceImageProcessing(std::string meshPath,
                                                 std::string orbiterPath,
                                                 std::string savePathFile,
                                                 std::string directLightningSave,
                                                 std::string renderTechnique,
                                                 std::string pdfMethod,
                                                 std::string triangleParam,
                                                 std::string directionParam,
                                                 int directRay, int indirectRay,
                                                 float epsilonUsed,
                                                 bool applyTonemapping,
                                                 bool debug, unsigned int resImageWidth,
                                                 unsigned int resImageHeight)
// creer l'image resultat : ne pas changer les dimensions en cas d'utilisation
// de la sauvegarde de l'ecalirage direct sinon bug tres probable
    : imageResultat(resImageWidth, resImageHeight),
      directLighthningImage(resImageWidth, resImageHeight) {

  // charger un objet
  mesh = read_mesh(meshPath.c_str());
  if (mesh.triangle_count() == 0)
    // erreur de chargement, pas de triangles
    exit(0);

  // creer l'ensemble de triangles / structure acceleratrice
  bvh = new BVH(mesh);
  sources = new Sources(mesh);

  // charger la camera
  if (camera.read_orbiter(orbiterPath.c_str()))
    // erreur, pas de camera
    exit(0);
  nbDirectRay = directRay;
  nbIndirectRay = indirectRay;
  epsilon = epsilonUsed;
  useTonemapping = applyTonemapping;
  showColorOverFlow = debug;
  if (!savePathFile.empty()) { pathAndFilename = savePathFile; }
  if (!directLightningSave.empty()) {
    directLightningFile = directLightningSave;
    hardLoadDirectLightning();
    directLightningBeforeBounceDone = true;
  }
  if (!renderTechnique.empty()) { renderWanted = renderTechnique; }
  if (!pdfMethod.empty()) { pdfTechnique = pdfMethod; }
  if (triangleParam ==
      "squareRoot") { triangleParametrization = squareRootParametrization; }
  else { triangleParametrization = square2TriangleParametrization; }
  if (directionParam ==
      "uniform") { directionParametrization = randomPointHemisphereUniform; }
  else { directionParametrization = randomPointHemisphereDistributed; }
}

RayTraceImageProcessing::~RayTraceImageProcessing() {
  delete bvh;
  delete sources;
  bvh = nullptr;
  sources = nullptr;
}

void RayTraceImageProcessing::rayTrace() {
  std::cout << "Computing Ray Trace\n";
  auto cpu_start = std::chrono::high_resolution_clock::now();

  // parcourir tous les pixels de l'image
  // en parallele avec openMP, un thread par bloc de 16 lignes
  int nbPixelDone = 0;
  if (renderWanted != "Ex7" || !directLightningBeforeBounceDone) {
#pragma omp parallel for schedule(dynamic, 1) default(none) shared(nbPixelDone, std::cout)
    for (int py = 0; py < imageResultat.height(); py++) {
      // nombres aleatoires, version c++11
      std::random_device seed;
      // un generateur par thread... pas de synchronisation
      std::default_random_engine rng(seed());
      // nombres aleatoires entre 0 et 1
      std::uniform_real_distribution<float> u01(0.f, 1.f);
      for (int px = 0; px < imageResultat.width(); px++) {
        if (nbPixelDone % (imageResultat.width() * imageResultat.height() / 100) == 0 &&
            ((nbPixelDone / float(imageResultat.width() * imageResultat.height()) * 100) +
             1) > 1) {
          std::cout
              << int(floor(
                  (nbPixelDone / float(imageResultat.width() * imageResultat.height()) *
                   100) + 1))
              << "% Direct Lightning Done" << std::endl;
        }
        computePixel(px, py, rng, u01);
        nbPixelDone++;
      }
    }
    directLightningBeforeBounceDone = true;
    hardSaveDirectLightning();
  }
  if (renderWanted == "Ex7") {
    nbPixelDone = 0;
#pragma omp parallel for schedule(dynamic, 1) default(none) shared(nbPixelDone, std::cout)
    for (int py = 0; py < imageResultat.height(); py++) {
      // nombres aleatoires, version c++11
      std::random_device seed;
      // un generateur par thread... pas de synchronisation
      std::default_random_engine rng(seed());
      // nombres aleatoires entre 0 et 1
      std::uniform_real_distribution<float> u01(0.f, 1.f);
      for (int px = 0; px < imageResultat.width(); px++) {
        if (nbPixelDone % (imageResultat.width() * imageResultat.height() / 100) == 0 &&
            ((nbPixelDone / float(imageResultat.width() * imageResultat.height()) * 100) +
             1) > 1) {
          std::cout
              << int(floor(
                  (nbPixelDone / float(imageResultat.width() * imageResultat.height()) *
                   100) + 1))
              << "% Indirect Lightning Done" << std::endl;
        }
        computePixel(px, py, rng, u01);
        nbPixelDone++;
      }
    }
  }
  if (useTonemapping) { applyTonemapping(); }


  auto cpu_stop = std::chrono::high_resolution_clock::now();
  int cpu_time = std::chrono::duration_cast<std::chrono::milliseconds>(
      cpu_stop - cpu_start).count();
  printf("cpu  %ds %03dms\n", int(cpu_time / 1000), int(cpu_time % 1000));

  // enregistrer l'image resultat
  write_image(imageResultat, (pathAndFilename + ".png").c_str());
  write_image_hdr(imageResultat, (pathAndFilename + ".hdr").c_str());
}

void RayTraceImageProcessing::computePixel(int px, int py,
                                           std::default_random_engine &rng,
                                           std::uniform_real_distribution<float> &u01) {
  Transform model = Identity();
  Transform view = camera.view();
  Transform projection = camera.projection(imageResultat.width(),
                                           imageResultat.height(),
                                           45);
  Transform viewport = Viewport(imageResultat.width(), imageResultat.height());
  Color color = Black();

  // generer le rayon pour le pixel (x, y)
  float x = px + u01(rng);
  float y = py + u01(rng);

  // Set Points for ray
  // origine dans l'image
  Point o(view.inverse()(projection.inverse()(viewport.inverse()(Point(x, y, 0)))));
  // extremite dans l'image
  Point e(view.inverse()(projection.inverse()(viewport.inverse()(Point(x, y, 1)))));

  Ray ray(o, e);
  // calculer les intersections
  if (Hit hit = bvh->intersect(ray)) {
    pxDebug = px, pyDebug = py;
    if (renderWanted == "Ex2") { color = exercice2Material(hit, ray); }
    else if (renderWanted == "Ex5") {
      color = exercice5DirectLightning(hit, ray, rng, u01);
    } else if (renderWanted == "Ex7")
      if (directLightningBeforeBounceDone) {
        color = exercice7Rebond(hit, ray, rng, u01, px, py);
      } else {
        color = exercice5DirectLightning(hit, ray, rng, u01);
        Color resColor;
        resColor.r = std::min(color.r, 1.f);
        resColor.g = std::min(color.g, 1.f);
        resColor.b = std::min(color.b, 1.f);
        directLighthningImage(px, py) = Color(resColor, 1);
      }
    else {
      color = occlusionAmbiante(hit, ray);
    }
  }

  if (showColorOverFlow && (color.r > 1.f || color.g > 1.f || color.b > 1.f)) {
    color = Yellow();
  }

  imageResultat(px, py) = Color(color, 1);
}

Color RayTraceImageProcessing::exercice2Material(Hit hitInfo, Ray usedRay) {
  const Material &material = mesh.triangle_material(hitInfo.triangle_id);
  const TriangleData &triangle = mesh.triangle(hitInfo.triangle_id);
  Vector pn = normal(hitInfo, triangle);
  float cos_theta = std::max(0.f, dot(pn, normalize(-usedRay.d)));
  if (material.emission.power() > 0) { return material.emission * cos_theta; }
  else { return material.diffuse * cos_theta; }
}

Color RayTraceImageProcessing::exercice5DirectLightning(Hit hitInfo,
                                                        Ray usedRay,
                                                        std::default_random_engine &rng,
                                                        std::uniform_real_distribution<float> &u01) {
  Color color = Black();
  // recuperer du triangle touche et de sa matiere
  const TriangleData &triangle = mesh.triangle(hitInfo.triangle_id);
  const Material &material = mesh.triangle_material(hitInfo.triangle_id);

  Point pointCameraMesh = point(hitInfo, usedRay);
  // normale interpolee du triangle au point d'intersection
  Vector normalCameraMesh = normal(hitInfo, triangle);
  // retourne la normale pour faire face a la camera / origine du rayon...
  normalCameraMesh = normalOrientationIncomingRay(normalCameraMesh, usedRay);

  if (nbDirectRay % sources->sources.size() != 0) {
    nbDirectRay += sources->sources.size() - nbDirectRay % sources->sources.size();
  }
  if (pdfTechnique == "AreaSources") {
    color = areaSourcesSampling(pointCameraMesh, normalCameraMesh, material);
  } else if (pdfTechnique == "CosAndDistance") {
    color = cosAndDistanceSampling(pointCameraMesh, normalCameraMesh, material);
  } else { // MIS Method
    color = multiImportanceSampling(pointCameraMesh, normalCameraMesh, material);
  }
  color = color * (1.0f / float(nbDirectRay));
  color = color + material.emission;

  return
      color;
}

Color RayTraceImageProcessing::areaSourcesSampling(Point pointCameraMesh,
                                                   Vector normalCameraMesh,
                                                   const Material &material) {
  Color color = Black();
  float areaSources = 0;
  for (auto s : sources->sources) {
    areaSources += aireTriangle(s.a, s.b, s.c);
  }
  areaSources = 1.0f / areaSources;
  float pdf = areaSources;
  for (auto s : sources->sources) {
    Point origin = pointCameraMesh;
    for (int i = 0; i < nbDirectRay / sources->sources.size(); ++i) {
      Point extremite(triangleParametrization(s));
      Vector lightPointNormal = normal(s, extremite);
      lightPointNormal = normalOrientationIncomingRay(lightPointNormal,
                                                      Ray(extremite, origin));
      origin = origin + epsilon * normalCameraMesh;
      extremite = extremite + epsilon * lightPointNormal;
      Ray rayLight(origin, extremite);
      if (Hit hitLight = bvh->intersect(rayLight)) {
        const TriangleData &triangleLight = mesh.triangle(hitLight.triangle_id);
        const Material &materialLight = mesh.triangle_material(hitLight.triangle_id);
        if (materialLight.emission.power() > 0) {
          float cosCameraMesh =
              std::max(0.f, dot(normalCameraMesh, normalize(rayLight.d)));
          Vector normalMeshLight = normal(hitLight, triangleLight);
          normalMeshLight = normalOrientationIncomingRay(normalMeshLight, rayLight);
          float cosMeshLight =
              std::max(0.f, dot(normalMeshLight, normalize(-rayLight.d)));
          color = color +
                  (materialLight.emission *
                   1.f / float(M_PI) * material.diffuse *
                   ((cosCameraMesh * cosMeshLight) / distance2(origin, extremite)) *
                   1.0f / pdf);
        }
      }
    }
  }
  return color;
}

Color RayTraceImageProcessing::cosAndDistanceSampling(Point pointCameraMesh,
                                                      Vector normalCameraMesh,
                                                      const Material &material) {
  Color color = Black();
  World w = World(normalCameraMesh);
  Point origin = pointCameraMesh;
  float pdf, miniPdf;
  origin = origin + epsilon * normalCameraMesh;
  for (int i = 0; i < nbDirectRay; ++i) {
    Point extremite = directionParametrization(miniPdf);
    Ray rayLight = Ray(origin, w(Vector(extremite)));
    float cosCameraMesh = miniPdf * M_PI;
    if (Hit hitLight = bvh->intersect(rayLight)) {
      const TriangleData &triangleLight = mesh.triangle(hitLight.triangle_id);
      const Material &materialLight = mesh.triangle_material(hitLight.triangle_id);
      Vector normalMeshLight = normal(hitLight, triangleLight);
      normalMeshLight = normalOrientationIncomingRay(normalMeshLight, rayLight);
      float cosMeshLight =
          std::max(0.f, dot(normalMeshLight, normalize(-rayLight.d)));
      if (materialLight.emission.power() > 0) {
        Point e = point(hitLight, rayLight);
        pdf = (miniPdf / M_PI) * (cosMeshLight / distance2(origin, e));
        color = color +
                (materialLight.emission *
                 1.f / float(M_PI) * material.diffuse *
                 ((cosCameraMesh * cosMeshLight) / distance2(origin, e)) *
                 1.0f / pdf);
      }
    }
  }
  return color;
}

Color RayTraceImageProcessing::multiImportanceSampling(Point pointCameraMesh,
                                                       Vector normalCameraMesh,
                                                       const Material &material) {
  Color color = Black();
//Generating ray with area of sources as pdf
  float areaSources = 0;
  for (auto s : sources->sources) {
    areaSources += aireTriangle(s.a, s.b, s.c);
  }
  areaSources = 1.0f / areaSources;
  float pdfAreaSources = areaSources;
  for (auto s : sources->sources) {
    Point origin = pointCameraMesh;
    for (int i = 0; i < nbDirectRay / sources->sources.size(); ++i) {
      Point extremite(triangleParametrization(s));
      Vector lightPointNormal = normal(s, extremite);
      lightPointNormal = normalOrientationIncomingRay(lightPointNormal,
                                                      Ray(extremite, origin));
      origin = origin + epsilon * normalCameraMesh;
      extremite = extremite + epsilon * lightPointNormal;
      Ray rayLight(origin, extremite);
      if (Hit hitLight = bvh->intersect(rayLight)) {
        const TriangleData &triangleLight = mesh.triangle(hitLight.triangle_id);
        const Material &materialLight = mesh.triangle_material(hitLight.triangle_id);
        if (materialLight.emission.power() > 0) {
          float cosCameraMesh =
              std::max(0.f, dot(normalCameraMesh, normalize(rayLight.d)));
          Vector normalMeshLight = normal(hitLight, triangleLight);
          normalMeshLight = normalOrientationIncomingRay(normalMeshLight, rayLight);
          float cosMeshLight =
              std::max(0.f, dot(normalMeshLight, normalize(-rayLight.d)));
          float pdfCosAndDistance =
              (cosCameraMesh / M_PI) * (cosMeshLight / distance2(origin, extremite));
          color = color +
                  (materialLight.emission *
                   1.f / float(M_PI) * material.diffuse *
                   ((cosCameraMesh * cosMeshLight) / distance2(origin, extremite)) *
                   1.0f / pdfAreaSources) *
                  // Ponderations de l'estimateur
                  (pdfAreaSources / (pdfAreaSources + pdfCosAndDistance));
        }
      }
    }
  }
  // Generating ray using cosAndDistance as pdf
  World w = World(normalCameraMesh);
  Point origin = pointCameraMesh;
  float pdf, miniPdf;
  origin = origin + epsilon * normalCameraMesh;
  for (int i = 0; i < nbDirectRay; ++i) {
    Point extremite = directionParametrization(miniPdf);
    Ray rayLight = Ray(origin, w(Vector(extremite)));
    float cosCameraMesh = miniPdf * M_PI;
    if (Hit hitLight = bvh->intersect(rayLight)) {
      const TriangleData &triangleLight = mesh.triangle(hitLight.triangle_id);
      const Material &materialLight = mesh.triangle_material(hitLight.triangle_id);
      Vector normalMeshLight = normal(hitLight, triangleLight);
      normalMeshLight = normalOrientationIncomingRay(normalMeshLight, rayLight);
      float cosMeshLight =
          std::max(0.f, dot(normalMeshLight, normalize(-rayLight.d)));
      if (materialLight.emission.power() > 0) {
        Point e = point(hitLight, rayLight);
        pdf = (miniPdf / M_PI) * (cosMeshLight / distance2(origin, e));
        color = color +
                ((materialLight.emission *
                  1.f / float(M_PI) * material.diffuse *
                  ((cosCameraMesh * cosMeshLight) / distance2(origin, e)) *
                  1.0f / pdf)) *
                (pdf / (pdfAreaSources + pdf));
      }
    }
  }
  return color;
}

Color RayTraceImageProcessing::occlusionAmbiante(Hit hitInfo, Ray usedRay) {
  const Material &material = mesh.triangle_material(hitInfo.triangle_id);
  const TriangleData &triangle = mesh.triangle(hitInfo.triangle_id);
  if (material.emission.power() > 0) { return material.emission; }
  else {
    Point origin = point(hitInfo, usedRay);
    Vector normalCameraMesh = normal(hitInfo, triangle);
    normalCameraMesh = normalOrientationIncomingRay(normalCameraMesh, usedRay);
    origin = origin + normalCameraMesh * epsilon;
    World w = World(normalCameraMesh);
    Color color = material.diffuse;
    float pdf, ambianteOclusionFactor = 0;
    for (int i = 0; i < nbDirectRay; ++i) {
      Point extremite = directionParametrization(pdf);
      Ray r = Ray(origin, w(Vector(extremite)));
      float cosThetaK = std::max(0.f, dot(normalCameraMesh, normalize(r.d)));
      Hit hit = bvh->intersect(r);
      if (hit != 1) {
        ambianteOclusionFactor += cosThetaK * (1.0 / pdf);
      }
    }
    color = color
            * (ambianteOclusionFactor * (1.0f / M_PI) * (1.0f / float(nbDirectRay)));
    return color;
  }
}

Color RayTraceImageProcessing::exercice7Rebond(Hit hitInfo, Ray usedRay,
                                               std::default_random_engine &rng,
                                               std::uniform_real_distribution<float> &u01,
                                               int px, int py) {
  Transform view = camera.view();
  Transform projection = camera.projection(imageResultat.width(),
                                           imageResultat.height(),
                                           45);
  Transform viewport = Viewport(imageResultat.width(), imageResultat.height());
  Color color = Black();
  const Material &material = mesh.triangle_material(hitInfo.triangle_id);
  const TriangleData &triangle = mesh.triangle(hitInfo.triangle_id);
  Point pointCameraMesh = point(hitInfo, usedRay);
  // normale interpolee du triangle au point d'intersection
  Vector normalCameraMesh = normal(hitInfo, triangle);
  // retourne la normale pour faire face a la camera / origine du rayon...
  normalCameraMesh = normalOrientationIncomingRay(normalCameraMesh, usedRay);

  float pdf;
  pointCameraMesh = pointCameraMesh + normalCameraMesh * epsilon;
  World w = World(normalCameraMesh);
  for (int i = 0; i < nbIndirectRay; ++i) {
    Point extremite = directionParametrization(pdf);
    Ray r = Ray(pointCameraMesh, w(Vector(extremite)));
    float cosThetaK = std::max(0.f, dot(normalCameraMesh, normalize(r.d)));
    Hit hit = bvh->intersect(r);
    if (hit) {
      // Trouver le pixel correspondant dans l'image pour obtenir sa couleur
      Point oWorld = point(hit, r);
      Point o = Point(viewport(projection(view(oWorld))));
      int pixelTouchedX = floor(o.x), pixelTouchedY = floor((o.y));
      if (pixelTouchedX >= 0 && pixelTouchedX <= directLighthningImage.width()
          && pixelTouchedY >= 0 && pixelTouchedY <= directLighthningImage.height()
          && o.z >= 0 && o.z <= 1) {
        Point origin(view.inverse()(projection.inverse()(
            viewport.inverse()(Point(o.x, o.y, 0)))));
        // extremite dans l'image
        Point extremite(view.inverse()(projection.inverse()(
            viewport.inverse()(Point(o.x, o.y, 1)))));
        Ray ray(origin, extremite);
        Hit isSamePixel = bvh->intersect(ray);
        Point t = point(isSamePixel, ray);
        float seuil = 0.001;
        if (oWorld.x >= t.x - seuil && oWorld.x <= t.x + seuil
            && oWorld.y >= t.y - seuil && oWorld.y <= t.y + seuil
            && oWorld.z >= t.z - seuil && oWorld.z <= t.z + seuil) {
          color = color +
                  // Lumière emise par le point touche : Le(s)
                  directLighthningImage(pixelTouchedX, pixelTouchedY) *
                  // fr(sk,pointCameraMesh,o) couleur du materiaux
                  1.f / float(M_PI) * material.diffuse *
                  // cosTeta
                  cosThetaK *
                  (1.0 / pdf);
        }
      }
    }
  }
  color = color / float(nbIndirectRay) + directLighthningImage(px, py);
  return color;
}

void RayTraceImageProcessing::applyTonemapping() {
  for (int py = 0; py < imageResultat.height(); py++) {
    for (int px = 0; px < imageResultat.width(); px++) {
      imageResultat(px, py).r = pow(imageResultat(px, py).r, 1.0 / 2.2);
      imageResultat(px, py).g = pow(imageResultat(px, py).g, 1.0 / 2.2);
      imageResultat(px, py).b = pow(imageResultat(px, py).b, 1.0 / 2.2);
    }
  }
}

void RayTraceImageProcessing::hardSaveDirectLightning() {
  FILE *fichier;
  fichier = fopen((pathAndFilename + ".taf").c_str(), "w");
  if (fichier == NULL) {
    fprintf(stderr, "Erreur à l'ouvertur du fichier\n");
  } else {
    Color color;
    for (int py = 0; py < imageResultat.height(); py++) {
      for (int px = 0; px < imageResultat.width(); px++) {
        color = imageResultat(px, py);
        int r = (int) std::min(std::floor(color.r * 255.f), 255.f);
        int g = (int) std::min(std::floor(color.g * 255.f), 255.f);
        int b = (int) std::min(std::floor(color.b * 255.f), 255.f);
        fprintf((fichier), "%d %d %d ", r, g, b);
      }
      fprintf((fichier), "\n");
    }
  }
}

void RayTraceImageProcessing::hardLoadDirectLightning() {
  FILE *fichier;
  fichier = fopen((directLightningFile).c_str(), "r");
  if (fichier == NULL) {
    fprintf(stderr, "Erreur à l'ouvertur du fichier\n");
  } else {
    Color color;
    for (int py = 0; py < imageResultat.height(); py++) {
      for (int px = 0; px < imageResultat.width(); px++) {
        unsigned int r, g, b;
        fscanf((fichier), "%d %d %d ", &r, &g, &b);
        directLighthningImage(px, py) = Color(((float) r) / 255.f,
                                              ((float) g) / 255.f,
                                              ((float) b) / 255.f, 1);
      }
      fscanf((fichier), "\n");
    }
  }
}
