// This code comes from here: http://stackoverflow.com/questions/5988686/creating-a-3d-sphere-in-opengl-using-visual-c/5989676#5989676

#define _USE_MATH_DEFINES
#include "icg_helper.h"
#include "glm/gtc/type_ptr.hpp"
#include <GL/gl.h>
#include <GL/glu.h>
#include <vector>
#include <cmath>



class SolidSphere
{
protected:
    std::vector<GLfloat> vertices;
    GLuint vertex_array_id_, vertex_buffer_object_index, vertex_buffer_object_position;
    std::vector<GLfloat> normals;
    std::vector<GLfloat> texcoords;
    std::vector<GLushort> indices;
    GLuint program_id_;
    GLuint MVP_id_;
    GLuint numb;

public:
    void Init(float radius, unsigned int rings, unsigned int sectors)
    {
        program_id_ = icg_helper::LoadShaders("solidsphere_vshader.glsl",
                                              "solidsphere_fshader.glsl");

        if(!program_id_){
            exit(EXIT_FAILURE);
        }

        glUseProgram(program_id_);

        glGenVertexArrays(1,&vertex_array_id_);
        glBindVertexArray(vertex_array_id_);

        float const R = 1./(float)(rings-1);
        float const S = 1./(float)(sectors-1);
        int r, s;

       /* vertices.resize(rings * sectors * 3);
        normals.resize(rings * sectors * 3);
        texcoords.resize(rings * sectors * 2);
        std::vector<GLfloat>::iterator v = vertices.begin();
       // std::vector<GLfloat>::iterator n = normals.begin();
        std::vector<GLfloat>::iterator t = texcoords.begin();
        /*for(r = 0; r < rings; r++) for(s = 0; s < sectors; s++) {
                float const y = sin( -M_PI_2 + M_PI * r * R );
                float const x = cos(2*M_PI * s * S) * sin( M_PI * r * R );
                float const z = sin(2*M_PI * s * S) * sin( M_PI * r * R );

                *t++ = s*S;
                *t++ = r*R;

                *v++ = x * radius;
                *v++ = y * radius;
                *v++ = z * radius;

         /*       *n++ = x;
                *n++ = y;
                *n++ = z;*/
        //}

        /*indices.resize(rings * sectors * 4);
        std::vector<GLushort>::iterator i = indices.begin();
        for(r = 0; r < rings-1; r++) for(s = 0; s < sectors-1; s++) {
                *i++ = r * sectors + s;
                *i++ = r * sectors + (s+1);
                *i++ = (r+1) * sectors + (s+1);
                *i++ = (r+1) * sectors + s;
        }*/

        const GLfloat position[] = {-1.0f, -1.0f,  1.0f, // left, bottom, front
                                    1.0f, -1.0f,  1.0f,  // right, bottom, front
                                    1.0f,  1.0f,  1.0f,  // right, top, front
                                    -1.0f,  1.0f,  1.0f, // left, top, front
                                    -1.0f, -1.0f, -1.0f, // left, bottom, back
                                    1.0f, -1.0f, -1.0f,  // right, bottom, back
                                    1.0f,  1.0f, -1.0f,  // right, top, back
                                    -1.0f,  1.0f, -1.0f};// left, top, back

        glGenBuffers(1, &vertex_buffer_object_position);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_position);
        glBufferData(GL_ARRAY_BUFFER, sizeof(position), position,GL_STATIC_DRAW);

        const GLuint index[] = {0, 1, 2,  // front face triangle 1
                                0, 2, 3,  // front face triangle 2
                                1, 5, 6,  // right face triangle 1
                                1, 6, 2,  // right face triangle 2
                                5, 4, 7,  // back face triangle 1
                                5, 7, 6,  // back face triangle 2
                                4, 0, 3,  // left face triangle 1
                                4, 3, 7,  // left face triangle 2
                                3, 2, 6,  // top face triangle 1
                                3, 6, 7,  // top face triangle 2
                                1, 0, 4,  // bottom face triangle 1
                                1, 4, 5}; // bottom face triangle 2

        //
        GLuint loc_position = glGetAttribLocation(program_id_, "position");
        glEnableVertexAttribArray(loc_position); // Enable it
        glVertexAttribPointer(loc_position, 3, GL_FLOAT, DONT_NORMALIZE,
                              ZERO_STRIDE, ZERO_BUFFER_OFFSET);

        numb = sizeof(index)/sizeof(GLuint);

        // index buffer

        glGenBuffers(1, &vertex_buffer_object_index);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_index);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

        // other uniforms
        MVP_id_ = glGetUniformLocation(program_id_, "MVP");

        // to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void Draw(glm::mat4 projection=IDENTITY_MATRIX,
              glm::mat4 view=IDENTITY_MATRIX,
              glm::mat4 model=IDENTITY_MATRIX)
    {
        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);

        // setup MVP
        glm::mat4 MVP = projection*view*model;
        glUniformMatrix4fv(MVP_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(MVP));

        // draw
        glDrawElements(GL_TRIANGLES, numb, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        glUseProgram(0);
        /*glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glTranslatef(x,y,z);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
        glNormalPointer(GL_FLOAT, 0, &normals[0]);
        glTexCoordPointer(2, GL_FLOAT, 0, &texcoords[0]);
        glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_SHORT, &indices[0]);
        glPopMatrix();*/
    }
};
