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


#endif //SI3D_TP_OPENGLUTILS_H
