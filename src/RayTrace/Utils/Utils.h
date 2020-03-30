#pragma once

#ifndef GKIT2LIGHTCMAKE_UTILS_H
#define GKIT2LIGHTCMAKE_UTILS_H

#include <cfloat>
#include <cassert>
#include <cmath>
#include <random>

#include "vec.h"
#include "mesh.h"

struct Ray {
  Point o;
  float pad;
  Vector d;
  float tmax;

  Ray() : o(), d(), tmax(0) {}

  Ray(const Point &_o, const Point &_e) : o(_o), d(Vector(_o, _e)), tmax(1) {}

  Ray(const Point &_o, const Vector &_d) : o(_o), d(_d), tmax(FLT_MAX) {}
};


// intersection rayon / triangle.
struct Hit {
  int triangle_id;
  float t;
  float u, v;

  Hit() : triangle_id(-1), t(0), u(0), v(0) {}       // pas d'intersection
  Hit(const int _id, const float _t, const float _u, const float _v) : triangle_id(_id),
                                                                       t(_t), u(_u),
                                                                       v(_v) {}

  operator bool() const {
    return (triangle_id != -1);
  }      // renvoie vrai si l'intersection est initialisee...
};

// renvoie la normale interpolee d'un triangle.
Vector normal(const Hit &hit, const TriangleData &triangle);

// renvoie le point d'intersection sur le triangle.
Point point(const Hit &hit, const TriangleData &triangle);

// renvoie le point d'intersection sur le rayon
Point point(const Hit &hit, const Ray &ray);


// triangle "intersectable".
struct Triangle {
  Point p;
  Vector e1, e2;
  int id;

  Triangle(const Point &_a, const Point &_b, const Point &_c, const int _id) :
      p(_a), e1(Vector(_a, _b)), e2(Vector(_a, _c)), id(_id) {}

  /* calcule l'intersection ray/triangle
      cf "fast, minimum storage ray-triangle intersection"
      http://www.graphics.cornell.edu/pubs/1997/MT97.pdf

      renvoie faux s'il n'y a pas d'intersection valide (une intersection peut exister mais peut ne pas se trouver dans l'intervalle [0 htmax] du rayon.)
      renvoie vrai + les coordonnees barycentriques (u, v) du point d'intersection + sa position le long du rayon (t).
      convention barycentrique : p(u, v)= (1 - u - v) * a + u * b + v * c
  */
  Hit intersect(const Ray &ray, const float htmax) const {
    Vector pvec = cross(ray.d, e2);
    float det = dot(e1, pvec);

    float inv_det = 1 / det;
    Vector tvec(p, ray.o);

    float u = dot(tvec, pvec) * inv_det;
    if (u < 0 || u > 1) return Hit();

    Vector qvec = cross(tvec, e1);
    float v = dot(ray.d, qvec) * inv_det;
    if (v < 0 || u + v > 1) return Hit();

    float t = dot(e2, qvec) * inv_det;
    if (t > htmax || t < 0) return Hit();

    return Hit(id, t, u, v);           // p(u, v)= (1 - u - v) * a + u * b + v * c
  }
};


// ensemble de triangles.
// a remplacer par une vraie structure acceleratrice, un bvh, par exemple
struct BVH {
  std::vector<Triangle> triangles;

  BVH() = default;

  BVH(const Mesh &mesh) { build(mesh); }

  void build(const Mesh &mesh) {
    triangles.clear();
    triangles.reserve(mesh.triangle_count());
    for (int id = 0; id < mesh.triangle_count(); id++) {
      TriangleData data = mesh.triangle(id);
      triangles.push_back(Triangle(data.a, data.b, data.c, id));
    }

    printf("%d triangles\n", int(triangles.size()));
    assert(triangles.size());
  }

  Hit intersect(const Ray &ray) const {
    Hit hit;
    float tmax = ray.tmax;
    for (int id = 0; id < int(triangles.size()); id++)
      // ne renvoie vrai que si l'intersection existe dans l'intervalle [0 tmax]
      if (Hit h = triangles[id].intersect(ray, tmax)) {
        hit = h;
        tmax = h.t;
      }

    return hit;
  }

  bool visible(const Ray &ray) const {
    for (int id = 0; id < int(triangles.size()); id++)
      if (triangles[id].intersect(ray, ray.tmax))
        return false;

    return true;
  }
};


struct Source {
  Point a, b, c;
  Color emission;

  Source() {}

  Source(const TriangleData &data, const Color &color) : a(data.a), b(data.b),
                                                         c(data.c), emission(color) {}
};

struct Sources {
  std::vector<Source> sources;

  Sources(const Mesh &mesh) : sources() {
    build(mesh);

    printf("%d sources\n", int(sources.size()));
    assert(sources.size());
  }

  void build(const Mesh &mesh) {
    sources.clear();
    for (int id = 0; id < mesh.triangle_count(); id++) {
      const TriangleData &data = mesh.triangle(id);
      const Material &material = mesh.triangle_material(id);
      if (material.emission.power() > 0)
        sources.push_back(Source(data, material.emission));
    }
  }
};


// utilitaires
// construit un repere ortho tbn, a partir d'un seul vecteur, la normale d'un point d'intersection, par exemple.
// permet de transformer un vecteur / une direction dans le repere du monde.

// cf "generating a consistently oriented tangent space"
// http://people.compute.dtu.dk/jerf/papers/abstracts/onb.html
// cf "Building an Orthonormal Basis, Revisited", Pixar, 2017
// http://jcgt.org/published/0006/01/01/
struct World {
  World(const Vector &_n) : n(_n) {
    float sign = std::copysign(1.0f, n.z);
    float a = -1.0f / (sign + n.z);
    float d = n.x * n.y * a;
    t = Vector(1.0f + sign * n.x * n.x * a, sign * d, -sign * n.x);
    b = Vector(d, sign + n.y * n.y * a, -n.y);
  }

  // transforme le vecteur du repere local vers le repere du monde
  Vector operator( )(const Vector &local) const {
    return local.x * t + local.y * b + local.z * n;
  }

  // transforme le vecteur du repere du monde vers le repere local
  Vector inverse(const Vector &global) const {
    return Vector(dot(global, t), dot(global, b), dot(global, n));
  }

  Vector t;
  Vector b;
  Vector n;
};

Point squareRootParametrization(Source source);
Point square2TriangleParametrization(Source source);

double calcBrbf(double kFactor, double mFactor, double cosTeta);

#endif //GKIT2LIGHTCMAKE_UTILS_H


// CMakeFiles/tuto_ray.exe.dir/src/RayTrace/RayTraceImageProcessing.cpp.o
// CMakeFiles/tuto_ray.exe.dir/src/RayTrace/tuto_ray.cpp.o:tuto_ray.cpp