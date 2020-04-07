#include <SDL_types.h>
#include "image_hdr.h"
#include "image_io.h"

#include "RayTraceImageProcessing.h"

RayTraceImageProcessing::RayTraceImageProcessing(const char *meshPath,
                                                 const char *orbiterPath)
// creer l'image resultat
    : image(1024, 640) {
  //: image(128, 64) {
  // charger un objet
  mesh = read_mesh(meshPath);
  if (mesh.triangle_count() == 0)
    // erreur de chargement, pas de triangles
    exit(0);

  // creer l'ensemble de triangles / structure acceleratrice
  bvh = new BVH(mesh);
  sources = new Sources(mesh);

  // charger la camera
  if (camera.read_orbiter(orbiterPath))
    // erreur, pas de camera
    exit(0);
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
#pragma omp parallel for schedule(dynamic, 1)
  for (int py = 0; py < image.height(); py++) {
    // nombres aleatoires, version c++11
    std::random_device seed;
    // un generateur par thread... pas de synchronisation
    std::default_random_engine rng(seed());
    // nombres aleatoires entre 0 et 1
    std::uniform_real_distribution<float> u01(0.f, 1.f);

    for (int px = 0; px < image.width(); px++) {
      computePixel(px, py, rng, u01);
    }
  }

  auto cpu_stop = std::chrono::high_resolution_clock::now();
  int cpu_time = std::chrono::duration_cast<std::chrono::milliseconds>(
      cpu_stop - cpu_start).count();
  printf("cpu  %ds %03dms\n", int(cpu_time / 1000), int(cpu_time % 1000));

  // enregistrer l'image resultat
  //hardLoadDirectLightning();
  hardSaveDirectLightning();
  write_image(image, (path + filename + ".png").c_str());
  write_image_hdr(image, (path + filename + ".hdr").c_str());
}

void RayTraceImageProcessing::computePixel(int px, int py,
                                           std::default_random_engine &rng,
                                           std::uniform_real_distribution<float> &u01) {
  Transform model = Identity();
  Transform view = camera.view();
  Transform projection = camera.projection(image.width(), image.height(), 45);
  Transform viewport = Viewport(image.width(), image.height());
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
    //color = exercice2Material(hit, ray);
    pxDebug = px, pyDebug = py;
    color = exercice5DirectLightning(hit, ray, rng, u01);
  }
  image(px, py) = Color(color, 1);
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
  if (dot(normalCameraMesh, usedRay.d) > 0) { normalCameraMesh = -normalCameraMesh; }

  // Pour toute les sources, crer un/des rayon(s) du point de l'intersection : pointCameraMesh à la sources
  // Si l'intersection touche le point source alors on l'éclair sinon non
  int nbRay = 100;
  int nbSource = sources->sources.size();
  for (auto s : sources->sources) {
    Point origin = pointCameraMesh;
    for (int i = 0; i < nbRay; ++i) {
      //Point extremite(squareRootParametrization(s));
      Point extremite(square2TriangleParametrization(s));

      float epsilon = 0.000001;

      origin = origin + epsilon * normalCameraMesh;
      extremite = extremite + epsilon * (extremite - origin);
      Ray rayLight(origin, extremite);
      if (Hit hitLight = bvh->intersect(rayLight)) {
        const TriangleData &triangleLight = mesh.triangle(hitLight.triangle_id);
        const Material &materialLight = mesh.triangle_material(hitLight.triangle_id);
        if (materialLight.emission.power() > 0) {
          float pdf = 1.0f/aireTRect(triangleLight);
          float cosCameraMesh =
              std::max(0.f, dot(normalCameraMesh, normalize(rayLight.d)));
          Vector normalMeshLight = normal(hitLight, triangleLight);
          if (dot(normalMeshLight, rayLight.d) > 0) {
            normalMeshLight = -normalMeshLight;
          }
          float cosMeshLigth =
              std::max(0.f, dot(normalMeshLight, normalize(-rayLight.d)));

// Formule lumière : Lr = Le(pointCameraMesh) + Sum(Le(s)*fr(sk,pointCameraMesh,o)*(cosTeta*costTeta_sk)/dist^2(sk,pointCameraMesh)*1/pdf(sk))
// pdf(sk) densite de proba de l'aire sur la source (si on prends 10 points uniformement sur la source, proba = 1/10)
// Vu que c'est pas forcément uniforme, pdf peux varier
          color = color +
                  // Lumière emise par le point touche : Le(s)
                  (materialLight.emission *
                   // fr(sk,pointCameraMesh,o) couleur du materiaux ici cas mixte ???
                   //calcBrbf(0.5, 0.5, cosMeshLigth) * material.diffuse *
                   // Old brbf
                   1.f / float(M_PI) * material.diffuse *
                   // (cosTeta*costTeta_sk)
                   ((cosCameraMesh * cosMeshLigth) /
                    // dist^2(sk,pointCameraMesh)
                    distance2(origin, extremite))) /
                  // 1/pdf(sk) : répartition uniforme donc proba uniforme
                  pdf;

          /*if (color.power() > 1) {
            printf("Pixel(%d, %d) : \n", pxDebug, pyDebug);
            printf("   BRBF = %f\n", calcBrbf(0.5, 0.5, cosMeshLigth));
            printf("   Dist = %f\n", distance2(origin, extremite));
            printf("   Double cos = %f, %f\n", cosCameraMesh, cosMeshLigth);
            printf("   (%f, %f, %f)\n", color.r, color.g, color.b);
          }*/
        }
      }
    }
  }
  color = color * (1.0f / float(nbRay*nbSource));
  color = color + material.emission;

  if (color.power() > 1 && material.emission.power()==0) {
    printf("Pixel(%d, %d) : \n", pxDebug, pyDebug);
    printf("   (%f, %f, %f)\n", color.r, color.g, color.b);
    color = Yellow();
  }
  return color;
}

void RayTraceImageProcessing::applyTonemapping() {
  for (int py = 0; py < image.height(); py++) {
    for (int px = 0; px < image.width(); px++) {
      image(px, py).r = pow(image(px, py).r, 1.0 / 2.2);
      image(px, py).g = pow(image(px, py).g, 1.0 / 2.2);
      image(px, py).b = pow(image(px, py).b, 1.0 / 2.2);
    }
  }
}

void RayTraceImageProcessing::hardSaveDirectLightning() {
  FILE *fichier;
  fichier = fopen((path + filename + ".taf").c_str(), "w");
  if (fichier == NULL) {
    fprintf(stderr, "Erreur à l'ouvertur du fichier\n");
  } else {
    Color color;
    for (int py = 0; py < image.height(); py++) {
      for (int px = 0; px < image.width(); px++) {
        color = image(px, py);
        int r = (int) std::min(std::floor(color.r * 255.f), 255.f);
        int g = (int) std::min(std::floor(color.g * 255.f), 255.f);
        int b = (int) std::min(std::floor(color.b * 255.f), 255.f);
        fprintf((fichier), "%d %d %d ", r, g, b);
        //printf("%d %d %d ", r, g, b);
      }
      fprintf((fichier), "\n");
    }
  }
}

void RayTraceImageProcessing::hardLoadDirectLightning() {
  FILE *fichier;
  fichier = fopen((path + filename + ".taf").c_str(), "r");
  if (fichier == NULL) {
    fprintf(stderr, "Erreur à l'ouvertur du fichier\n");
  } else {
    Color color;
    for (int py = 0; py < image.height(); py++) {
      for (int px = 0; px < image.width(); px++) {
        //char *r, *g, *b, *a;
        unsigned int r, g, b;
        fscanf((fichier), "%d %d %d ", &r, &g, &b);
        //printf("%s %s %s %s ", r, g, b, a);
        image(px, py) = Color(((float) r) / 255.f,
                              ((float) g) / 255.f,
                              ((float) b) / 255.f, 1);
      }
      //printf("\n");
      fscanf((fichier), "\n");
    }
  }
}
