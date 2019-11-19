//! \file tuto7.cpp reprise de tuto6.cpp mais en derivant App::init(), App::quit() et bien sur App::render().
#include <list>

#include "wavefront.h"
#include "texture.h"

#include "program.h"
#include "uniforms.h"

#include "orbiter.h"
#include "draw.h"
#include "app_time.h"


// utilitaire : renvoie la chaine de caracteres pour un type glsl.
const char *glsl_string( const GLenum type )
{
    switch(type)
    {
        case GL_BOOL:
            return "bool";
        case GL_UNSIGNED_INT:
            return "uint";
        case GL_INT:
            return "int";
        case GL_FLOAT:
            return "float";
        case GL_FLOAT_VEC2:
            return "vec2";
        case GL_FLOAT_VEC3:
            return "vec3";
        case GL_FLOAT_VEC4:
            return "vec4";
        case GL_FLOAT_MAT3:
            return "mat3";
        case GL_FLOAT_MAT4:
            return "mat4";

        default:
            return "";
    }
}

int print_uniform(const GLuint program) {
    if (program == 0) {
        printf("[error] program 0, no buffers...\n");
        return -1;
    }

    // recupere le nombre d'uniform buffers
    GLint buffer_count = 0;
    glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &buffer_count);

    for (int i = 0; i < buffer_count; i++) {
        // recupere le nom du block et son indice
        char bname[1024] = {0};
        glGetActiveUniformBlockName(program, i, sizeof(bname), nullptr, bname);

        GLint binding = 0;
        glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_BINDING, &binding);

        printf("  uniform '%s' binding %d\n", bname, binding);

        GLint variable_count = 0;
        glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS,
                                  &variable_count);

        std::vector<GLint> variables(variable_count);
        glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES,
                                  variables.data());
        for (int k = 0; k < variable_count; k++) {
            // recupere chaque info... une variable a la fois,
            GLint glsl_type = 0;
            glGetActiveUniformsiv(program, 1, (GLuint *) &variables[k], GL_UNIFORM_TYPE,
                                  &glsl_type);
            GLint offset = 0;
            glGetActiveUniformsiv(program, 1, (GLuint *) &variables[k], GL_UNIFORM_OFFSET,
                                  &offset);

            GLint array_size = 0;
            glGetActiveUniformsiv(program, 1, (GLuint *) &variables[k], GL_UNIFORM_SIZE,
                                  &array_size);
            GLint array_stride = 0;
            glGetActiveUniformsiv(program, 1, (GLuint *) &variables[k],
                                  GL_UNIFORM_ARRAY_STRIDE, &array_stride);

            GLint matrix_stride = 0;
            glGetActiveUniformsiv(program, 1, (GLuint *) &variables[k],
                                  GL_UNIFORM_MATRIX_STRIDE, &matrix_stride);
            GLint matrix_row_major = 0;
            glGetActiveUniformsiv(program, 1, (GLuint *) &variables[k],
                                  GL_UNIFORM_IS_ROW_MAJOR, &matrix_row_major);

            // nom de la variable
            char vname[1024] = {0};
            glGetActiveUniformName(program, variables[k], sizeof(vname), nullptr, vname);

            printf("    '%s %s': offset %d", glsl_string(glsl_type), vname, offset);
            if (array_size > 1)
                printf(", array size %d", array_size);

            printf(", stride %d", array_stride);

            // organisation des matrices
            if (glsl_type == GL_FLOAT_MAT4 || glsl_type == GL_FLOAT_MAT3)
                printf(", %s, matrix stride %d",
                       matrix_row_major ? "row major" : "column major", matrix_stride);

            printf("\n");
        }

        GLint buffer_size = 0;
        glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_DATA_SIZE, &buffer_size);
        printf("  buffer size %d\n\n", buffer_size);
    }

    return 0;
}

typedef alignas(16) float a16float;
struct MaterialGlsl{
    Color difuse;
    Color specular;
    Color emission;
    a16float ns;
};

struct RobotMaterialsGlsl {
    MaterialGlsl mats[3];
};

struct RobotGLBuffers {
    GLuint vao;
    GLuint vertex_buffer;
    GLuint materialUniform;
    int vertex_count;
    int vertex_size;

    RobotGLBuffers() : vao(0), vertex_buffer(0), vertex_count(0) {}

    void create(const Mesh mesh[23], GLuint m_program) {
        if (!mesh[0].vertex_buffer_size()) return;

        // TODO Use those variable instead of callin the functions
        vertex_count = mesh[0].vertex_count();
        vertex_size = mesh[0].vertex_buffer_size();

        // cree et initialise le buffer: conserve la positions des sommets
        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER,
                // Taille du buffer
                     vertex_size * 23 +
                     mesh[0].triangle_count()*3 * sizeof(int),
                     nullptr,
                     GL_DYNAMIC_DRAW);

        // On met les mesh dans le buffer
        for (int i = 0; i < 23; ++i) {
            glBufferSubData(GL_ARRAY_BUFFER,
                    // byte de départ dans le buffer
                            i * vertex_size,
                            vertex_size,
                            mesh[i].vertex_buffer());
        }

        // Création du tableau d'index des matières par vertex
        int *indexMaterials = new int[mesh[0].triangle_count()*3];
        //fprintf(stderr, "Nb materials %d.\n", mesh[0].mesh_material_count());
        for (int i = 0; i < mesh[0].triangle_count(); ++i) {
            indexMaterials[3*i] = mesh[0].materials()[i];
            indexMaterials[3*i+1] = mesh[0].materials()[i];
            indexMaterials[3*i+2] = mesh[0].materials()[i];
        }
        glBufferSubData(GL_ARRAY_BUFFER,
                // byte de départ dans le buffer
                        23 * vertex_size,
                        mesh[0].triangle_count()*3 * sizeof(int),
                        indexMaterials);
        // Destruction d'element plus utile
        delete[]indexMaterials;

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(3);

        // Attribution des index de matériaux dans le buffer pour chaque sommet
        glVertexAttribIPointer(3, 1,
                // size et type, position est un vec3 dans le vertex shader
                               GL_INT,
                // stride 0, les valeurs sont les unes a la suite des autres
                               0,
                // offset = 0, les valeurs sont au debut du buffer
                               (const GLvoid *) (23 * vertex_size));

        // Création du tableau uniform de materiaux
        // Récupération de l'identifiant du groupe d'uniforms materials
        materialUniform = glGetUniformBlockIndex(m_program, "mat");

        // recupere le nombre d'uniform buffers
        GLint buffer_count= 0;
        glGetProgramiv(m_program, GL_ACTIVE_UNIFORM_BLOCKS, &buffer_count);

        std::cout<<buffer_count<<std::endl;

        // Association de l'indice 0 a materials
        glUniformBlockBinding(m_program, materialUniform, 0);

        // Création du buffer pour les uniforms
        GLuint bufferMaterial;
        // Création et Initalisation des datas materials
        RobotMaterialsGlsl mats;
        for (int i = 0; i < mesh[0].mesh_material_count(); ++i) {
            mats.mats->difuse = mesh[0].mesh_materials()[i].diffuse;
            mats.mats->specular = mesh[0].mesh_materials()[i].specular;
            mats.mats->emission = mesh[0].mesh_materials()[i].emission;
            mats.mats->ns = mesh[0].mesh_materials()[i].ns;
        }

        glGenBuffers(1, &bufferMaterial);
        glBindBuffer(GL_UNIFORM_BUFFER, bufferMaterial);
        glBufferData(GL_UNIFORM_BUFFER,
                // Taille du buffer
                     sizeof(RobotMaterialsGlsl),
                     &mats,
                     GL_STATIC_DRAW);

        // Selection d'un buffer existant pour affecter les valeurs a tous les
        // uniforms
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, bufferMaterial);
    }

    // TODO 1) Mettre 23 fois le tableau d'index de matière dans le buffer
    // TODO 2) Mettre le tableau d'index dans un uniform buffer (attention a l'alignement)

    void release() {
        glDeleteBuffers(1, &vertex_buffer);
        glDeleteVertexArrays(1, &vao);
    }
};

class TP : public AppTime {
public:
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    TP() : AppTime(1024, 640) {}

    // creation des objets de l'application
    int init() {

        // Loading glsl shader
        m_program = 0;
        m_program = read_program("src/shader/robot.glsl");
        program_print_errors(m_program);

        // Initiating Meshes
        Mesh mesh[23];
        for (int i = 1; i <= 23; i++) {
            std::string str;
            if (i <= 9) {
                str = "data/Robot/run/Robot_00000" + std::to_string(i) + ".obj";
            } else {
                str = "data/Robot/run/Robot_0000" + std::to_string(i) + ".obj";
            }
            mesh[i - 1] = read_mesh(str.c_str());
            if (!mesh[i - 1].vertex_count()) return -1;
        }
        m_objet.create(mesh, m_program);

        for (int i = 0; i < 23; i++) {
            mesh[i].release();
        }

        Point pmin(-15, -15, -15), pmax(15, 15, 15);
        //mesh.bounds(pmin, pmax);
        m_camera.lookat(pmin, pmax);


        // etat openGL par defaut
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);        // couleur par defaut de la fenetre

        glClearDepth(1.f); // profondeur par defaut
        // ztest, conserver l'intersection la plus proche de la camera
        glDepthFunc(GL_LESS);
        glEnable(GL_DEPTH_TEST);                    // activer le ztest

        return 0;   // ras, pas d'erreur
    }

    // destruction des objets de l'application
    int quit() {
        m_objet.release();

        return 0;
    }

    // dessiner une nouvelle image
    int render() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // deplace la camera
        int mx, my;
        unsigned int mb = SDL_GetRelativeMouseState(&mx, &my);
        if (mb & SDL_BUTTON(1))              // le bouton gauche est enfonce
            m_camera.rotation(mx, my);
        else if (mb & SDL_BUTTON(3))         // le bouton droit est enfonce
            m_camera.move(mx);
        else if (mb & SDL_BUTTON(2)) {         // le bouton du milieu est enfonce
            m_camera.translation((float) mx / (float) window_width(),
                                 (float) my / (float) window_height());
        }

        glUseProgram(m_program);

        int nbFrame = 23;
        float time = global_time();
        //print_uniform(m_program);


        for (int i = 0; i < 10; ++i) {
            for (int j = 0; j < 10; ++j) {
                Transform model = /*Scale(0.1, 0.1, 0.1) * */
                        RotationY(45) * Translation(4 * i, -5, 4 * -j);
                Transform view = m_camera.view();
                Transform projection = m_camera.projection(window_width(),
                                                           window_height(), 45);
                Transform mvp = projection * view * model;

                int location = glGetUniformLocation(m_program, "mvpMatrix");
                glUniformMatrix4fv(location, 1, GL_TRUE, mvp.buffer());

                location = glGetUniformLocation(m_program, "mesh_color");
                glUniform4f(location, abs(sin(time / 1000)), (float) i / 10,
                            (float) j / 10, 1);

                int frame = int((time / 100)) % nbFrame;
                int nbLoop = int((time / 100)) / nbFrame;
                float deltaT = (time / 100) - (frame + nbFrame * nbLoop);

                location = glGetUniformLocation(m_program, "deltaT");
                glUniform1f(location, deltaT);

                location = glGetUniformLocation(m_program, "deltaT");
                glUniform1f(location, deltaT);

                glBindVertexArray(m_objet.vao);

                glVertexAttribPointer(0,
                                      3,
                        // size et type, position est un vec3 dans le vertex shader
                                      GL_FLOAT,
                        // pas de normalisation des valeurs
                                      GL_FALSE,
                        // stride 0, les valeurs sont les unes a la suite des autres
                                      0,
                        // offset = 0, les valeurs sont au debut du buffer
                                      (const GLvoid *) (frame * m_objet.vertex_size));

                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0,
                                      (const GLvoid *) ((frame + 1) % nbFrame *
                                                        m_objet.vertex_size));

                // dessiner les triangles de l'objet
                glDrawArrays(GL_TRIANGLES, 0, m_objet.vertex_count);
            }
        }

        return 1;
    }

protected:
    RobotGLBuffers m_objet;
    GLuint m_program;
    Orbiter m_camera;
};


int main(int argc, char **argv) {
    // il ne reste plus qu'a creer un objet application et la lancer 
    TP tp;
    tp.run();

    return 0;
}
