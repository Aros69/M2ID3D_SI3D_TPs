#include <algorithm>

#include "app_time.h"

#include "mat.h"
#include "mesh.h"
#include "wavefront.h"

#include "program.h"
#include "uniforms.h"

#include "orbiter.h"

#include "../Utils/OpenGLUtils.h"

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

// Objectif : trouver le minimum d'un
// Creer une texture avec la première ligne = tout les chiffres en entrer
// Les colonnes représentent les étapes des computes (donc 2 fois moins de lignes que de colonnes)
struct computeShaderTest : public AppTime {
  // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
  computeShaderTest() : AppTime(1024, 640, 4, 3) {
    nbEntier = 10;
    entiers = new int[nbEntier];
    for (int i = 0; i < 10; ++i) { entiers[i] = i; }
  }

  int init() {
    // Chargement du shader
    m_program = read_program("src/RayTrace/GPU/min.glsl");
    program_print_errors(m_program);
    printComputeShadersInfo();

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);


    // Embalage de la texture dans un frame buffer
    glGenTextures(1, &texture);
    // Definition du type de texture (ici texture 2D)
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
        GL_TEXTURE_2D, // Texture type
        0, // Niveau de la mipmap que l'on cree
        GL_R32UI, // Definis le composant de la texture (r, rg, rgb, rgba, ...)
        nbEntier, // Largeur de la texture
        nbEntier, // hauteur de la texture
        0, // Doit etre 0
        GL_RED_INTEGER, // Format de donnee dans les pixels
        GL_UNSIGNED_INT, // Type de donnee dans les pixels
        nullptr // Donnee a mettre dans la texture
    );
    // On supprime les autres niveaux de la mipmap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    // Old (bad ?) version of texture initialisation, create image not texture
    /*textureBinding = 0; // la valeur du binding (dans le shader)
    glBindImageTexture(
        textureBinding, // id binding
        texture,        // id texture
        0,              // niveau de la mipmap de la texture
        GL_TRUE,        // false = texture decouper en plusiers couche
        0,              // quelle(s) couche(s) on bind
        GL_READ_WRITE,  // access (lecture, ecriture ou les deux
        GL_R16UI        // format des elements de la texture
        );*/

    // Recuperation de l'uniform de la texture
    textureLocation = glGetUniformLocation(m_program, "imageMin");
    glUniform1i(textureLocation, textureBinding);

    // Si les résultats sont écrits dans une (storage) texture et que le cpu
    // va relire ces données avec glGetTexImage(),
    // il faut utiliser glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT),

    // TODO Trouver comment initialiser le valeurs d'une texture


    // Préparation a l'execution du shader
    GLint threads[3] = {};
    // Récupération du nombre de groupe dans le shader
    glGetProgramiv(m_program, GL_COMPUTE_WORK_GROUP_SIZE, threads);
    printf("Taille des groupes :(%d, %d, %d)\n.", threads[0], threads[1], threads[2]);
    fflush(stdout);

    int N = nbEntier;
    int groups = N / threads[0];
    if (N % threads[0] > 0) {
      // Un groupe supplémentaire, si N n'est pas un multiple de threads[0]
      // Permet aussi de lancer au moins un thread
      groups = groups + 1;
    }
    // Execution du compute shader
    glDispatchCompute(groups, 1, 1);


    // TODO Comment and understand this function
    /*glGetTexImage(
        target,
        level,
        format,
        type,
        pixels
        );*/

    return 0;
  }

  int quit() {
    return 0;
  }

  int render() {
    /*
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // config pipeline
    glBindVertexArray(m_vao);
    glUseProgram(m_program);

    // uniform buffer 0
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_buffer);
    program_uniform(m_program, "triangle_count",
                    std::min((int) m_mesh.triangle_count(), 1024));

    program_uniform(m_program, "mvpInvMatrix", mvp.inverse());
    glDrawArrays(GL_TRIANGLES, 0, 3);
    */

    return 1;
  }

protected:
  // Nombre d'entier dans le tableau
  unsigned int nbEntier;
  // Le tableau d'entier que l'on va tester
  int *entiers;
  // L'identifiant du compute shader
  GLuint m_program;
  // L'identifiant de la texture
  GLuint texture;
  // L'identifiant de la texture dans le shader
  GLuint textureBinding;
  //
  GLuint textureLocation;

  GLuint m_vao;
};


int main(int argc, char **argv) {

  computeShaderTest app;
  app.run();

  return 0;
}

/*
#include <GL/glew.h>

void computeShaderMin() {
  // application
  // TODO define N and n ?
  int N; // ????????????
  int n; // ????????????
  int groups = (N + 1024) / 1024;
  glDispatchCompute(groups, 1, 1);   // premiere iteration

    //la syntaxe correcte, pour fixer les valeurs des uniforms est la meme que d'habitude, cf
    //glUniform1i( glUniformLocation(program, "N"), N);
    //glUniform1i( glUniformLocation(program, "n"), 0);

// iterations suivantes
  for (int n = N / 2; n > 1; n = n / 2) {
    // attendre que les résultats soient écrient
    // Si les résultats sont écrits dans une (storage) texture et que le cpu
    // va relire ces données avec glGetTexImage(),
    // il faut utiliser glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT),
    // Read : https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glMemoryBarrier.xhtml
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    int groups = (n + 1024) / 1024;
    glDispatchCompute(groups, 1, 1);
  }
}
*/