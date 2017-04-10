#pragma once
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Quad {

    private:
        GLuint vertex_array_id_; // vertex array object
        GLuint program_id_; // GLSL shader program ID
        GLuint vertex_buffer_object_; // memory buffer
        GLuint texture_id_; // texture ID
        GLuint MVP_id_; // MVP matrix

    public:


        vec3 vectorAdd(vec3 a, vec3 b, bool add){
            if(add){
                return vec3(a.x+b.x, a.y+b.y, a.z+b.z);
            }else{
                return vec3(a.x - b.x, a.y-b.y, a.z-b.z);
            }
        }

        void Init(vec3 a, vec3 b, float width) {
            // compile the shaders
            program_id_ = icg_helper::LoadShaders("quad_vshader.glsl",
                                                  "quad_fshader.glsl");

            if(!program_id_) {
                exit(EXIT_FAILURE);
            }

            glUseProgram(program_id_);

            // vertex one vertex Array
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);

            // vertex coordinates
            {
                vec3 diag = vec3(b.x - a.x, b.y-a.y, b.z-a.z);
                vec3 n = width*normalize(cross(diag, vec3(0.0f,0.0f,1.0f)));
                vec3 p1,p2,p3,p4;
                p1 = vectorAdd(a, n, false);
                p2 = vectorAdd(a, n, true);
                p3 = vectorAdd(b,n, false);
                p4 = vectorAdd(b, n, true);
                const GLfloat vertex_point[] = { p1.x, p1.y, p1.z,
                                                 p2.x, p2.y, p2.z,
                                                 p3.x, p3.y, p3.z,
                                                 p4.x, p4.y, p4.z};
                // buffer
                glGenBuffers(1, &vertex_buffer_object_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_point),
                             vertex_point, GL_STATIC_DRAW);

                // attribute
                GLuint vertex_point_id = glGetAttribLocation(program_id_, "vpoint");
                glEnableVertexAttribArray(vertex_point_id);
                glVertexAttribPointer(vertex_point_id, 3, GL_FLOAT, DONT_NORMALIZE,
                                      ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            }
        }


        void Cleanup() {
            glBindVertexArray(0);
            glUseProgram(0);
            glDeleteBuffers(1, &vertex_buffer_object_);
            glDeleteProgram(program_id_);
            glDeleteVertexArrays(1, &vertex_array_id_);
        }

        void Draw(const glm::mat4 &model = IDENTITY_MATRIX,
                  const glm::mat4 &view = IDENTITY_MATRIX,
                  const glm::mat4 &projection = IDENTITY_MATRIX) {
            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            // setup MVP
            glm::mat4 MVP = projection*view*model;
            glUniformMatrix4fv(MVP_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(MVP));

            // draw
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            glBindVertexArray(0);
            glUseProgram(0);
        }
};
