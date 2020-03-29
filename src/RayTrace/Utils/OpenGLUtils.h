#ifndef SI3D_TP_OPENGLUTILS_H
#define SI3D_TP_OPENGLUTILS_H

#include "vec.h"

// cf tuto_storage
namespace glsl
{
  template < typename T >
  struct alignas(8) gvec2
  {
    alignas(4) T x, y;

    gvec2( ) {}
    gvec2( const vec2& v ) : x(v.x), y(v.y) {}
  };

  typedef gvec2<float> vec2;
  typedef gvec2<int> ivec2;
  typedef gvec2<unsigned int> uvec2;
  typedef gvec2<int> bvec2;

  template < typename T >
  struct alignas(16) gvec3
  {
    alignas(4) T x, y, z;

    gvec3( ) {}
    gvec3( const vec3& v ) : x(v.x), y(v.y), z(v.z) {}
    gvec3( const Point& v ) : x(v.x), y(v.y), z(v.z) {}
    gvec3( const Vector& v ) : x(v.x), y(v.y), z(v.z) {}
  };

  typedef gvec3<float> vec3;
  typedef gvec3<int> ivec3;
  typedef gvec3<unsigned int> uvec3;
  typedef gvec3<int> bvec3;

  template < typename T >
  struct alignas(16) gvec4
  {
    alignas(4) T x, y, z, w;

    gvec4( ) {}
    gvec4( const vec4& v ) : x(v.x), y(v.y), z(v.z), w(v.w) {}
  };

  typedef gvec4<float> vec4;
  typedef gvec4<int> ivec4;
  typedef gvec4<unsigned int> uvec4;
  typedef gvec4<int> bvec4;
}

void printComputeShadersInfo() {
  GLint threads_max = 0;
  glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &threads_max);
  printf("Threads Max = %d\n", threads_max);

  // Les computes shaders s'ordonnance en 3D (utile en fonction du type de calcul : Images, Grilles...)
  GLint groups_max[3] = {};
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &groups_max[0]);
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &groups_max[1]);
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &groups_max[2]);
  printf("Groupe Max : %d, %d, %d\n", groups_max[0], groups_max[1], groups_max[2]);
}

struct triangle {
  glsl::vec3 a;
  glsl::vec3 ab;
  glsl::vec3 ac;
};

#endif //SI3D_TP_OPENGLUTILS_H
