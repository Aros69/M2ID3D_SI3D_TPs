#include "image_hdr.h"
#include "image_io.h"

#include "RayTraceImageProcessing.h"

RayTraceImageProcessing::RayTraceImageProcessing(const char *meshPath,
                                                 const char *orbiterPath)
// creer l'image resultat
    : image(1024, 640) {
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
  auto cpu_start = std::chrono::high_resolution_clock::now();

  // parcourir tous les pixels de l'image
  // en parallele avec openMP, un thread par bloc de 16 lignes
//#pragma omp parallel for schedule(dynamic, 1)
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
  write_image(image, "data/Result/TempRender.png");
  write_image_hdr(image, "data/Result/TempRender.hdr");
}

void RayTraceImageProcessing::computePixel(int px, int py,
                                           std::default_random_engine rng,
                                           std::uniform_real_distribution<float> u01) {
  /*std::cout << "Pixel : (" << px << ", " << py << ") pour une image de taille : "
                << image.width() << "x" << image.height() << std::endl;*/
  // recupere les transformations view, projection et viewport pour generer les rayons
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
    color = exercice5DirectLightning(hit, ray, rng, u01);
  }
  image(px, py) = Color(color, 1);
}

Color RayTraceImageProcessing::exercice2Material(Hit hitInfo, Ray usedRay) {
  const Material &material = mesh.triangle_material(hitInfo.triangle_id);
  const TriangleData &triangle = mesh.triangle(hitInfo.triangle_id);
  Vector pn = normal(hitInfo, triangle);
  float cos_theta = std::max(0.f, dot(pn, normalize(-usedRay.d)));
  if (material.emission.power() > 0) {
    return material.emission * material.diffuse * cos_theta;
  }
  else { return material.diffuse * cos_theta; }
}

Color RayTraceImageProcessing::exercice5DirectLightning(Hit hitInfo,
                                                        Ray usedRay,
                                                        std::default_random_engine rng,
                                                        std::uniform_real_distribution<float> u01) {
  Color color = Black();
  // recuperer le triangle
  const TriangleData &triangle = mesh.triangle(hitInfo.triangle_id);
  // et sa matiere
  const Material &material = mesh.triangle_material(hitInfo.triangle_id);


  // point d'intersection
  Point p = point(hitInfo, usedRay);
  // normale interpolee du triangle au point d'intersection
  Vector pn = normal(hitInfo, triangle);
  // retourne la normale pour faire face a la camera / origine du rayon...
  if (dot(pn, usedRay.d) > 0) { pn = -pn; }

  // Pour toute les sources, crer un/des rayon(s) du point de l'intersection : p à la sources
  // Si l'intersection touche le point source alors on l'éclair sinon non
  int nbRay = 10;
  int nbSource = sources->sources.size();
  for (auto s : sources->sources) {

    Point origin = p;
    for (int i = 0; i < nbRay; ++i) {
      float r1 = u01(rng);
      float r2 = u01(rng);
      float temp = sqrt(r1);
      float alpha = 1 - temp;
      float beta = (1 - r2) * temp;
      float lambda = r2 * temp;

      Point extremite(Point(s.a * alpha + s.b * beta + s.c * lambda));
      // Un des point du triangle selectionne empiriquement
      //Point extremite(Point(s.a));
      double epsilon = 0.000001;
      /*origin = origin + epsilon * pn;
      extremite = extremite + epsilon * (extremite - origin);*/
      Ray rayon(origin + pn * epsilon, extremite + epsilon * (extremite - origin));
      if (Hit hit1 = bvh->intersect(rayon)) {
        const TriangleData &triangle1 = mesh.triangle(hit1.triangle_id);
        const Material &material1 = mesh.triangle_material(hit1.triangle_id);
        if (material1.emission.power() > 0) {
          //std::cout<<"On passe par la"<<std::endl;
          float cos_theta = std::max(0.f, dot(pn, normalize(-usedRay.d)));
          Vector psN = normal(hit1, triangle1);
          if (dot(psN, rayon.d) > 0) { psN = -psN; }
          float cos_theta_sk = std::max(0.f, dot(psN, normalize(-rayon.d)));
          //std::cout<<cos_theta_sk<<std::endl;

// Formule lumière : Lr = Le(p) + Sum(Le(s)*fr(sk,p,o)*(cosTeta*costTeta_sk)/dist^2(sk,p)*1/pdf(sk))
// pdf(sk) densite de proba de l'aire sur la source (si on prends 10 points uniformement sur la source, proba = 1/10)
// Vu que c'est pas forcément uniforme, pdf peux varier
          color = color +
                  // Lumière emise par le point touche : Le(s)
                  material1.emission *
                  // fr(sk,p,o) couleur du materiaux ici cas mixte ???
                  calcBrbf(0.5, 0.5, cos_theta_sk) * material.diffuse *
                  // Old brbf
                  //(1.f / float(M_PI) * material.diffuse) *
                  // (cosTeta*costTeta_sk)
                  ((cos_theta * cos_theta_sk) /
                   // dist^2(sk,p)
                   distance2(origin, extremite)) *
                  // 1/pdf(sk) : répartition uniforme donc proba uniforme
                  1.0 / nbRay;
        }
      }
    }

  }
  color = color / (float) (nbRay * nbSource);
  color = color + material.emission;

  // accumuler la couleur de l'echantillon
  float cos_theta = std::max(0.f, dot(pn, normalize(-usedRay.d)));
  color = color + 1.f / float(M_PI) * material.diffuse * cos_theta;
  return color;
}