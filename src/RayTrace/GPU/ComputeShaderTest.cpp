#include <algorithm>

#include "app_time.h"

#include "mat.h"
#include "mesh.h"
#include "wavefront.h"

#include "program.h"
#include "uniforms.h"

#include "orbiter.h"

#include "../Utils/OpenGLUtils.h"

// Objectif : trouver le minimum d'un
// Creer une texture avec la première ligne = tout les chiffres en entrer
// Les colonnes représentent les étapes des computes (donc 2 fois moins de lignes que de colonnes)
struct computeShaderTest : public AppTime {
  // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
  computeShaderTest() : AppTime(1024, 640, 4, 3) {
    nbEntier = 10;
    entiers = new int[nbEntier];
    textureData = new unsigned int[nbEntier * nbEntier];
    for (unsigned int i = 0; i < nbEntier; ++i) {
      for (unsigned int j = 0; j < nbEntier; j++) {
        textureData[nbEntier * i + j] = 0;
      }
      entiers[i] = i;
      textureData[i] = i;
    }
    /*
    std::cout<<"Value before compute\n";
    for (int i = 0; i < nbEntier; ++i) {
      for (int j = 0; j < nbEntier; ++j) {
        printf("%d ", textureData[i*nbEntier+j]);
      }
      std::cout<<std::endl;
    }*/

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
    // Creation du lien entre la texture dans la shader et dans la memoire du GPU
    textureBinding = 0; // la valeur du binding (dans le shader)
    glBindImageTexture(
        textureBinding, // id binding
        texture,        // id texture
        0,              // niveau de la mipmap de la texture
        GL_TRUE,        // false = texture decouper en plusiers couche
        0,              // quelle(s) couche(s) on bind
        GL_READ_WRITE,  // access (lecture, ecriture ou les deux
        GL_R32UI        // format des elements de la texture
    );
    glTexImage2D(
        GL_TEXTURE_2D, // Texture type
        0, // Niveau de la mipmap que l'on cree
        GL_R32UI, // Definis le composant de la texture (r, rg, rgb, rgba, ...)
        nbEntier, // Largeur de la texture
        nbEntier, // hauteur de la texture
        0, // Doit etre 0
        GL_RED_INTEGER, // Format de donnee dans les pixels
        GL_UNSIGNED_INT, // Type de donnee dans les pixels
        textureData // Donnee a mettre dans la texture
    );
    // On supprime les autres niveaux de la mipmap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    // Recuperation de l'uniform de la texture
    textureLocation = glGetUniformLocation(m_program, "imageMin");
    glUniform1i(textureLocation, textureBinding);

    // Préparation a l'execution du shader
    GLint threads[3] = {};
    // Récupération du nombre de groupe dans le shader
    glGetProgramiv(m_program, GL_COMPUTE_WORK_GROUP_SIZE, threads);
    printf("Taille des groupes :(%d, %d, %d).\n", threads[0], threads[1], threads[2]);
    fflush(stdout);

    // N : nombre de threads a faire
    int N = nbEntier/2;
    int groups = N / threads[0];
    if (N % threads[0] > 0) {
      // Un groupe supplémentaire, si N n'est pas un multiple de threads[0]
      // Permet aussi de lancer au moins un thread
      groups = groups + 1;
    }
    // Execution du compute shader
    glDispatchCompute(groups, 1, 1);
    // On attends la fin de l'execution des threads du compute shader
    glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);

    glGetTexImage(
        GL_TEXTURE_2D, // Texture type
        0, // Niveau de la mipmap que l'on cree
        GL_RED_INTEGER, // Format de donnee dans les pixels
        GL_UNSIGNED_INT, // Type de donnee dans les pixels
        textureData // place where to but data
    );

    std::cout<<"Value of texture after compute\n";
    for (unsigned int i = 0; i < nbEntier; ++i) {
      for (unsigned int j = 0; j < nbEntier; ++j) {
        printf("%d ", textureData[i*nbEntier+j]);
      }
      std::cout<<std::endl;
    }


    return 0;
  }

  int quit() {
    // TODO free the two pointers and
    delete []entiers;
    entiers = nullptr;
    delete []textureData;
    textureData = nullptr;
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
  unsigned int *textureData;
  // L'identifiant du compute shader
  GLuint m_program;
  // L'identifiant de la texture
  GLuint texture;
  // L'identifiant de la texture dans le shader
  GLuint textureBinding;
  //
  GLuint textureLocation;
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