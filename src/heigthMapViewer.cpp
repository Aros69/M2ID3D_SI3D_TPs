#include <algorithm>
#include <image_io.h>

#include "app_time.h"

#include "mat.h"
#include "mesh.h"
#include "draw.h"
#include "wavefront.h"

#include "program.h"
#include "uniforms.h"

#include "orbiter.h"


// Objectif : afficher une heigthMap
struct heigthMapViewer : public AppTime {
  // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
  heigthMapViewer() : AppTime(1024, 640, 4, 3) {
    heightMap = read_image(path);
  }

  int init() {
    // Camera part default
    camera = Orbiter();

    printf("Image size (%d, %d)\n", heightMap.width(), heightMap.height());
    fflush(stdout);

    // Creation de mesh du terrain
    map = Mesh(GL_TRIANGLE_STRIP);
    map.color(Color(1, 0, 0));
    map.vertex(-0.5, -0.5, 0);

    map.color(Color(0, 1, 0));
    map.vertex(0.5,  0.5, 0);

    map.color(Color(0, 0, 1));
    map.vertex(-0.5,  0.5, 0);    /*for (int i=0; i < heightMap.width(); ++i) {
      for(int j=0;j<heightMap.height()-1;++j){
        map.color(White());
        map.vertex(i/10.0,j/10.0,heightMap.sample(i,j).r/10.0);
        map.color(White());
        map.vertex(i/10.0,(j+1)/10.0,heightMap.sample(i,j+1).r/10.0);
      }
    }*/
    return 0;
  }

  int quit() {
    return 0;
  }

  int render() {
    // etape 2 : dessiner l'objet avec opengl

    // on commence par effacer la fenetre avant de dessiner quelquechose
    glClear(GL_COLOR_BUFFER_BIT);

    // on dessine le triangle du point de vue de la camera
    draw(map, camera);

    return 1;   // on continue, renvoyer 0 pour sortir de l'application
  }

protected:
  // Chemin vers l'image
  const char *path = "/home/robin/Master/MJV/TPs/data/res.png";
  Image heightMap;
  Mesh map;
  Orbiter camera;
};


int main(int argc, char **argv) {

  heigthMapViewer app;
  app.run();

  return 0;
}
