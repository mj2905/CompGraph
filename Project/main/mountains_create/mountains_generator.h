#pragma once
#include "../constants.h"
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>
#include <array>

class MountainsGenerator {

    private:
        GLuint vertex_array_id_;                // vertex array object
        GLuint vertex_buffer_object_position_;  // memory buffer for positions
        GLuint vertex_buffer_object_index_;     // memory buffer for indices
        GLuint program_id_;                     // GLSL shader program ID

        GLuint texture_id_BL_;                  // texture ID bottom left
        GLuint texture_id_BR_;                  // texture ID bottom right
        GLuint texture_id_UL_;                  // texture ID upper left
        GLuint texture_id_UR_;                  // texture ID upper right

        GLuint num_indices_;                    // number of vertices to render

    public:
        void Init() {
            // compile the shaders.
            program_id_ = icg_helper::LoadShaders("mountains_g_vshader.glsl",
                                                  "mountains_g_fshader.glsl");
            if(!program_id_) {
                exit(EXIT_FAILURE);
            }

            glUseProgram(program_id_);

            // vertex one vertex array
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);

            // vertex coordinates and indices
            {
                std::vector<GLfloat> vertices;
                std::vector<GLuint> indices;
                // makes a triangle grid with dimension 100x100.
                // always two subsequent entries in 'vertices' form a 2D vertex position.
                int grid_dim = 1024;

                // the given code below are the vertices for a simple quad.
                // your grid should have the same dimension as that quad, i.e.,
                // reach from [-1, -1] to [1, 1].

                float middle = (grid_dim-1)/2.0f;


                for(size_t y = 0; y < grid_dim; ++y) {
                    for(size_t x = 0; x < grid_dim; ++x) {

                        //values bet -1 and 1
                        float left = (x - middle)/middle;
                        float down = (y - middle)/middle;

                        vertices.push_back(left); vertices.push_back(down);

                        if(x < grid_dim-1 and y < grid_dim-1) {
                            indices.push_back(x + (grid_dim)*y);
                            indices.push_back(x+1 + (grid_dim)*y);
                            indices.push_back(x+1 + (grid_dim)*(y+1));
                            indices.push_back(x + (grid_dim)*y);
                            indices.push_back(x+1 + (grid_dim)*(y+1));
                            indices.push_back(x + (grid_dim)*(y+1));
                        }
                    }
                }

                num_indices_ = indices.size();

                // position buffer
                glGenBuffers(1, &vertex_buffer_object_position_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_position_);
                glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
                             &vertices[0], GL_STATIC_DRAW);

                // vertex indices
                glGenBuffers(1, &vertex_buffer_object_index_);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_index_);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
                             &indices[0], GL_STATIC_DRAW);

                // position shader attribute
                GLuint loc_position = glGetAttribLocation(program_id_, "position");
                glEnableVertexAttribArray(loc_position);
                glVertexAttribPointer(loc_position, 2, GL_FLOAT, DONT_NORMALIZE,
                                      ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            }

            // load/Assign textures
            {
                glUniform1i(glGetUniformLocation(program_id_, "texBL"), 0 /*GL_TEXTURE0*/);
                glUniform1i(glGetUniformLocation(program_id_, "texBR"), 1 /*GL_TEXTURE1*/);
                glUniform1i(glGetUniformLocation(program_id_, "texUL"), 2 /*GL_TEXTURE2*/);
                glUniform1i(glGetUniformLocation(program_id_, "texUR"), 3 /*GL_TEXTURE3*/);
            }

            // to avoid the current object being polluted
            glBindVertexArray(0);
            glUseProgram(0);
        }

        //method to automatically change the textures, after the multitiles has computed them
        void changeTexture(const array<GLuint, 4>& ids) {
            texture_id_BL_ = ids[0];
            texture_id_BR_ = ids[1];
            texture_id_UL_ = ids[2];
            texture_id_UR_ = ids[3];
        }

        void Cleanup() {
            glBindVertexArray(0);
            glUseProgram(0);
            glDeleteBuffers(1, &vertex_buffer_object_position_);
            glDeleteBuffers(1, &vertex_buffer_object_index_);
            glDeleteVertexArrays(1, &vertex_array_id_);
            glDeleteProgram(program_id_);
            glDeleteTextures(1, &texture_id_BL_);
            glDeleteTextures(1, &texture_id_BR_);
            glDeleteTextures(1, &texture_id_UL_);
            glDeleteTextures(1, &texture_id_UR_);
        }

        void Draw(float offsetX, float offsetY) {

            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            // bind textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_id_BL_);

            // bind textures
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture_id_BR_);

            // bind textures
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, texture_id_UL_);

            // bind textures
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, texture_id_UR_);


            glm::vec2 offset = glm::vec2(offsetX, offsetY);

            glUniform2fv(glGetUniformLocation(program_id_, "offset"), 1, glm::value_ptr(offset));

            // draw
            // TODO 5: for debugging it can be helpful to draw only the wireframe.
            // You can do that by uncommenting the next line.
            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            // TODO 5: depending on how you set up your vertex index buffer, you
            // might have to change GL_TRIANGLE_STRIP to GL_TRIANGLES.
            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements(GL_TRIANGLES, num_indices_, GL_UNSIGNED_INT, 0);

            glBindVertexArray(0);
            glUseProgram(0);
        }
};
