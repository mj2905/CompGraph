#pragma once
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>

class Grid {

    private:
        GLuint vertex_array_id_;                // vertex array object
        GLuint vertex_buffer_object_position_;  // memory buffer for positions
        GLuint vertex_buffer_object_index_;     // memory buffer for indices
        GLuint program_id_;                     // GLSL shader program ID
        GLuint texture_id_;                     // texture ID
        GLuint interpolation_id_;                     // texture ID
        GLuint num_indices_;                    // number of vertices to render
        GLuint M_id_;                         // model matrix ID
        GLuint V_id_;                         // view matrix ID
        GLuint P_id_;                         // proj matrix ID

    public:
        void Init(GLuint texture) {
            // compile the shaders.
            program_id_ = icg_helper::LoadShaders("grid_vshader.glsl",
                                                  "grid_fshader.glsl");
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
                // TODO 5: make a triangle grid with dimension 100x100.
                // always two subsequent entries in 'vertices' form a 2D vertex position.
                int grid_dim = 512;

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
                this->texture_id_ = texture;
                glBindTexture(GL_TEXTURE_2D, texture_id_);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                GLuint tex_id = glGetUniformLocation(program_id_, "tex");
                glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);

                // cleanup
                glBindTexture(GL_TEXTURE_2D, 0);
            }

            // create 1D texture (colormap)
            {
                const int ColormapSize=5;
                GLfloat tex[3*ColormapSize] = {/*yellow*/    1.0, 1.0, 0.0,
                                                /*yellow*/    0.5, 1.0, 0.0,
                                               /*green*/ 0.0, 1.0, 0.0,
                                               /*darkgreen*/ 0, 0, 0,
                                               /*white*/  1.0, 1.0, 1.0};
                glGenTextures(1, &interpolation_id_);
                glBindTexture(GL_TEXTURE_1D, interpolation_id_);
                glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, ColormapSize, 0, GL_RGB, GL_FLOAT, tex);
                glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                GLuint tex_id = glGetUniformLocation(program_id_, "colormap");
                glUniform1i(tex_id, 1 /*GL_TEXTURE1*/);
                // check_error_gl();

                glBindTexture(GL_TEXTURE_1D, 0);
            }


            glm::vec3 light_pos = glm::vec3(1.0f, 1.0f, 2.0f);

            glm::vec3 La = glm::vec3(1.0f, 1.0f, 1.0f);
            glm::vec3 Ld = glm::vec3(1.0f, 1.0f, 1.0f);
            glm::vec3 Ls = glm::vec3(1.0f, 1.0f, 1.0f);

            GLuint light_pos_id = glGetUniformLocation(program_id_, "light_pos");

            GLuint La_id = glGetUniformLocation(program_id_, "La");
            GLuint Ld_id = glGetUniformLocation(program_id_, "Ld");
            GLuint Ls_id = glGetUniformLocation(program_id_, "Ls");

            glm::vec3 ka = glm::vec3(0.1f, 0.1f, 0.1f);
            glm::vec3 kd = glm::vec3(0.6f, 0.6f, 0.6f);
            glm::vec3 ks = glm::vec3(0.2f, 0.2f, 0.2f);
            float alpha = 60.0f;

            GLuint ka_id = glGetUniformLocation(program_id_, "ka");
            GLuint kd_id = glGetUniformLocation(program_id_, "kd");
            GLuint ks_id = glGetUniformLocation(program_id_, "ks");
            GLuint alpha_id = glGetUniformLocation(program_id_, "alpha");

            glUniform3fv(light_pos_id, 1, glm::value_ptr(light_pos));
            glUniform3fv(La_id, 1, glm::value_ptr(La));
            glUniform3fv(Ld_id, 1, glm::value_ptr(Ld));
            glUniform3fv(Ls_id, 1, glm::value_ptr(Ls));
            glUniform3fv(ka_id, ONE, glm::value_ptr(ka));
            glUniform3fv(kd_id, ONE, glm::value_ptr(kd));
            glUniform3fv(ks_id, ONE, glm::value_ptr(ks));
            glUniform1f(alpha_id, alpha);

            // other uniforms
            M_id_ = glGetUniformLocation(program_id_, "model");
            V_id_ = glGetUniformLocation(program_id_, "view");
            P_id_ = glGetUniformLocation(program_id_, "projection");

            // to avoid the current object being polluted
            glBindVertexArray(0);
            glUseProgram(0);
        }

        void Cleanup() {
            glBindVertexArray(0);
            glUseProgram(0);
            glDeleteBuffers(1, &vertex_buffer_object_position_);
            glDeleteBuffers(1, &vertex_buffer_object_index_);
            glDeleteVertexArrays(1, &vertex_array_id_);
            glDeleteProgram(program_id_);
            glDeleteTextures(1, &texture_id_);
        }

        void Draw(const glm::mat4 &model = IDENTITY_MATRIX,
                  const glm::mat4 &view = IDENTITY_MATRIX,
                  const glm::mat4 &projection = IDENTITY_MATRIX) {
            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            // bind textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_id_);

            // bind textures
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_1D, interpolation_id_);

            // setup MVP
            glUniformMatrix4fv(M_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(model));
            glUniformMatrix4fv(V_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(view));
            glUniformMatrix4fv(P_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(projection));

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
