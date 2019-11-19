
#include <algorithm>

#include "app_time.h"

#include "mat.h"
#include "mesh.h"
#include "wavefront.h"

#include "program.h"
#include "uniforms.h"

#include "orbiter.h"

#include "OpenGLUtils.h"

struct RT : public AppTime
{
  // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
  RT( const char *filename ) : AppTime(1024, 640, 4,3)
  {
    m_mesh= read_mesh(filename);
  }

  int init( )
  {
    if(m_mesh == Mesh::error())
      return -1;

    Point pmin, pmax;
    m_mesh.bounds(pmin, pmax);
    m_camera.lookat(pmin, pmax);

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, m_buffer);

    //
    struct triangle
    {
      glsl::vec3 a;
      glsl::vec3 ab;
      glsl::vec3 ac;
    };

    std::vector<triangle> data;
    data.reserve(m_mesh.triangle_count());
    for(int i= 0; i < m_mesh.triangle_count(); i++)
    {
      TriangleData t= m_mesh.triangle(i);
      data.push_back( { Point(t.a), Point(t.b) - Point(t.a), Point(t.c) - Point(t.a) } );
    }

    // alloue le buffer
    // alloue au moins 1024 triangles, cf le shader
    if(data.size() < 1024)
      data.resize(1024);
    glBufferData(GL_UNIFORM_BUFFER, data.size() * sizeof(triangle), data.data(), GL_STATIC_READ);

    //
    m_program= read_program("data/RayTracingData/raytrace.glsl");
    program_print_errors(m_program);

    // associe l'uniform buffer a l'entree 0
    GLint index= glGetUniformBlockIndex(m_program, "triangleData");
    glUniformBlockBinding(m_program, index, 0);

    return 0;
  }

  int quit( )
  {
    return 0;
  }

  int render( )
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(key_state('f'))
    {
      clear_key_state('f');
      Point pmin, pmax;
      m_mesh.bounds(pmin, pmax);
      m_camera.lookat(pmin, pmax);
    }

    // deplace la camera
    int mx, my;
    unsigned int mb= SDL_GetRelativeMouseState(&mx, &my);
    if(mb & SDL_BUTTON(1))              // le bouton gauche est enfonce
      m_camera.rotation(mx, my);
    else if(mb & SDL_BUTTON(3))         // le bouton droit est enfonce
      m_camera.move(mx);
    else if(mb & SDL_BUTTON(2))         // le bouton du milieu est enfonce
      m_camera.translation((float) mx / (float) window_width(), (float) my / (float) window_height());

    Transform m;
    Transform v= m_camera.view();
    Transform p= m_camera.projection(window_width(), window_height(), 45);
    Transform mvp= p * v * m;

    // config pipeline
    glBindVertexArray(m_vao);
    glUseProgram(m_program);

    // uniform buffer 0
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_buffer);
    program_uniform(m_program, "triangle_count", std::min((int) m_mesh.triangle_count(), 1024) );

    program_uniform(m_program, "mvpInvMatrix", mvp.inverse());
    glDrawArrays(GL_TRIANGLES, 0, 3);

    return 1;
  }

protected:
  Mesh m_mesh;
  Orbiter m_camera;

  GLuint m_program;
  GLuint m_vao;
  GLuint m_buffer;
};


int main( int argc, char **argv )
{
  const char *filename= "data/RayTracingData/cornell.obj";
  if(argc > 1)
    filename= argv[1];

  RT app(filename);
  app.run();

  return 0;
}
