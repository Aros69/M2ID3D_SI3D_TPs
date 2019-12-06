
#include <cfloat>
#include <random>
#include <chrono>

#include "vec.h"
#include "mesh.h"
#include "wavefront.h"
#include "orbiter.h"

#include "image.h"
#include "image_io.h"
#include "image_hdr.h"
#include "o_surface_bezier.c"
struct Ray
{
    Point o;
    float pad;
    Vector d;
    float tmax;

    Ray() : o(), d(), tmax(0) {}
    Ray(const Point &_o, const Point &_e) : o(_o), d(Vector(_o, _e)), tmax(1) {}
    Ray(const Point &_o, const Vector &_d) : o(_o), d(_d), tmax(FLT_MAX) {}
};

/** TODO
 * Renvoie les coordonnees u, v a partir d'une surface s et d'un rayon ray
 * Renvoie Faux s'il n y'a pas d'intersection
*/  
bool getIntersection(struct surface_bezier s, Ray r, double &u, double &v)
{
    return false;
}

int main(const int argc, const char **argv)
{
    const char *mesh_filename = "data/RayTracingData/cornell.obj";
    const char *orbiter_filename = "data/RayTracingData/cornell_orbiter.txt";

    if (argc > 1)
        mesh_filename = argv[1];
    if (argc > 2)
        orbiter_filename = argv[2];

    printf("%s: '%s' '%s'\n", argv[0], mesh_filename, orbiter_filename);

    // creer l'image resultat
    Image image(1024, 640);

    // charger un objet
    Mesh mesh = read_mesh(mesh_filename);
    if (mesh.triangle_count() == 0)
        // erreur de chargement, pas de triangles
        return 1;

    /**TODO
     *  Recupere la surface de bezier a partir du fichier obj
    */ 
    struct surface_bezier s;

    // charger la camera
    Orbiter camera;
    if (camera.read_orbiter(orbiter_filename))
        // erreur, pas de camera
        return 1;
    // recupere les transformations view, projection et viewport pour generer les rayons
    Transform model = Identity();
    Transform view = camera.view();
    Transform projection = camera.projection(image.width(), image.height(), 45);
    Transform viewport = Viewport(image.width(), image.height());

    int samples = 1;
    auto cpu_start = std::chrono::high_resolution_clock::now();

    double eps = 0.01;
    // Lumiere
    vec3 lumiere(0.0, 5.0, 0.0);
    // parcourir tous les pixels de l'image
    // en parallele avec openMP, un thread par bloc de 16 lignes
#pragma omp parallel for schedule(dynamic, 1)
    for (int py = 0; py < image.height(); py++)
    {
        // nombres aleatoires, version c++11
        std::random_device seed;
        // un generateur par thread... pas de synchronisation
        std::default_random_engine rng(seed());
        // nombres aleatoires entre 0 et 1
        std::uniform_real_distribution<float> u01(0.f, 1.f);

        for (int px = 0; px < image.width(); px++)
        {
            Color color = Black();

            // generer le rayon pour le pixel (x, y)
            float x = px + u01(rng);
            float y = py + u01(rng);

            Point o = camera.position();                                                            // origine dans l'image
            Point e = (view.inverse() * projection.inverse() * viewport.inverse())(Point(x, y, 1)); // extremite dans l'image

            Ray ray(o + eps * (e - o), e);
            double u,v;
            // calculer les intersection
            if (getIntersection(s, ray, u, v))
            {
                // Calculer la normal du point en (u,v)
                Quadruplet normQuad = getNormal(&s, u, v);
                Vector pn = Vector(normQuad.x, normQuad.y, normQuad.z); // normal calculé a partir de la surface
                // retourne la normale pour faire face a la camera / origine du rayon...
                // Utile ?
                if (dot(pn, ray.d) > 0)
                    pn = -pn;

                // accumuler la couleur de l'echantillon
                float cos_theta = std::max(0.f, dot(pn, normalize(-ray.d)));
                color = 1.f/float(M_PI) * White() * cos_theta;
            }
            image(px, py) = color;
        }
    }

    auto cpu_stop = std::chrono::high_resolution_clock::now();
    int cpu_time = std::chrono::duration_cast<std::chrono::milliseconds>(cpu_stop - cpu_start).count();
    printf("cpu  %ds %03dms\n", int(cpu_time / 1000), int(cpu_time % 1000));

    // enregistrer l'image resultat
    write_image(image, "render.png");
    write_image_hdr(image, "render.hdr");
    return 0;
}
