#pragma once
#include "../constants.h"
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>
#include <array>

class Water {

    private:
        GLuint vertex_array_id_;                // vertex array object
        GLuint vertex_buffer_object_position_;  // memory buffer for positions
        GLuint vertex_buffer_object_index_;     // memory buffer for indices
        GLuint program_id_;                     // GLSL shader program ID

        GLuint texture_id_;
        GLuint reflect_id_;
        GLuint normal_id_;
        GLuint dudv_id_;

        GLuint num_indices_;                    // number of vertices to render
        GLuint M_id_;                         // model matrix ID
        GLuint V_id_;                         // view matrix ID
        GLuint P_id_;                         // proj matrix ID

        GLuint light_pos_id;
        LightSource light;
        glm::vec3 light_pos;


    public:

        void loadImage(string filename, string uniformName, int textureNb, GLuint& id) {
            // load texture
            {
                int width;
                int height;
                int nb_component;
                // set stb_image to have the same coordinates as OpenGL
                stbi_set_flip_vertically_on_load(1);
                unsigned char* image = stbi_load(filename.c_str(), &width,
                                                 &height, &nb_component, 0);

                if(image == nullptr) {
                    throw(string("Failed to load texture " + filename));
                }

                glGenTextures(1, &id);
                glBindTexture(GL_TEXTURE_2D, id);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, -1);


                if(nb_component == 3) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                                 GL_RGB, GL_UNSIGNED_BYTE, image);
                } else if(nb_component == 4) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                                 GL_RGBA, GL_UNSIGNED_BYTE, image);
                }

                glGenerateMipmap(GL_TEXTURE_2D);

                GLuint tex_id = glGetUniformLocation(program_id_, uniformName.c_str());
                glUniform1i(tex_id, textureNb);

                // cleanup
                glBindTexture(GL_TEXTURE_2D, 0);
                stbi_image_free(image);
            }
        }

        void Init(GLuint terrain_texture, GLuint reflect_texture, LightSource &light, size_t grid_dim = 512) {
            // compile the shaders.
            program_id_ = icg_helper::LoadShaders("water_vshader.glsl",
                                                  "water_fshader.glsl");
            this->light = light;
            light_pos = glm::vec3(light.getPosition().x, light.getPosition().y, light.getPosition().z);
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
                glUniform1i(glGetUniformLocation(program_id_, "tex"), 0 /*GL_TEXTURE0*/);
                texture_id_ = terrain_texture;
            }

            // load/Assign textures
            {
                glUniform1i(glGetUniformLocation(program_id_, "ref"), 1 /*GL_TEXTURE1*/);
                reflect_id_ = reflect_texture;
            }

            // load/Assign textures
            {
                //loadImage("normal_map_1.jpg", "normal_map", 2, normal_id_);
                loadImage("normal.jpg", "normal_map", 2, normal_id_);
            }

            // load/Assign textures
            {
                //loadImage("waterdudv_tiled.jpg", "dudv", 4, dudv_id_);
                loadImage("waterdudv.jpg", "dudv", 3, dudv_id_);
            }

            //light_pos = glm::vec3(12.0f, 3, 9.0f);
            //glm::vec3 light_pos = glm::vec3(-1.0, 3, 0.0f);
            //light_pos = glm::vec3(2.5,1,2.5);
            //light_pos = glm::vec3(0.0f, 1, -1);
            //light_pos = glm::vec3(0.0f, 1, -2);

            glm::vec3 La = glm::vec3(1.0f, 1.0f, 0.9f);
            glm::vec3 Ld = glm::vec3(1.0f, 0.9f, 0.75f);
            glm::vec3 Ls = glm::vec3(1.0f, 0.9f, 0.75f);

            light_pos_id = glGetUniformLocation(program_id_, "light_pos");

            GLuint La_id = glGetUniformLocation(program_id_, "La");
            GLuint Ld_id = glGetUniformLocation(program_id_, "Ld");
            GLuint Ls_id = glGetUniformLocation(program_id_, "Ls");

            glm::vec3 ka = glm::vec3(0.1f, 0.1f, 0.1f);
            glm::vec3 kd = glm::vec3(0.3f, 0.3f, 0.3f);
            glm::vec3 ks = glm::vec3(0.7, 0.7, 0.7);
            float alpha = 50.0f;

            GLuint ka_id = glGetUniformLocation(program_id_, "ka");
            GLuint kd_id = glGetUniformLocation(program_id_, "kd");
            GLuint ks_id = glGetUniformLocation(program_id_, "ks");
            GLuint alpha_id = glGetUniformLocation(program_id_, "alpha");

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

            GLuint fog_threshold_id = glGetUniformLocation(program_id_, "fog_threshold");
            glUniform1f(fog_threshold_id, FOG_THRESHOLD);

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
            glDeleteTextures(1, &texture_id_);
            glDeleteTextures(1, &normal_id_);
            glDeleteTextures(1, &reflect_id_);
            glDeleteTextures(1, &dudv_id_);
            glDeleteProgram(program_id_);
        }

        void Draw(float offsetX, float offsetY,
                  const glm::mat4 &model = IDENTITY_MATRIX,
                  const glm::mat4 &view = IDENTITY_MATRIX,
                  const glm::mat4 &projection = IDENTITY_MATRIX) {
            glEnable(GL_BLEND);
            glEnable(GL_CLIP_PLANE0);
            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // bind textures
            glActiveTexture(GL_TEXTURE0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
            glBindTexture(GL_TEXTURE_2D, texture_id_);

            // bind textures
            glActiveTexture(GL_TEXTURE1);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
            glBindTexture(GL_TEXTURE_2D, reflect_id_);

            // bind textures
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, normal_id_);

            // bind textures
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, dudv_id_);

            glm::vec2 offset = glm::vec2(offsetX, offsetY);

            glUniform2fv(glGetUniformLocation(program_id_, "offset"), 1, glm::value_ptr(offset));

            // setup MVP
            glUniformMatrix4fv(M_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(model));
            glUniformMatrix4fv(V_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(view));
            glUniformMatrix4fv(P_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(projection));

            glUniform1f(glGetUniformLocation(program_id_, "time"), glfwGetTime());

            glm::vec3 rot_light_pos =  glm::mat3(glm::rotate(IDENTITY_MATRIX, (float)glfwGetTime()/100-1, glm::vec3(0,1,0))) * light_pos;

            glUniform3fv(light_pos_id, 1, glm::value_ptr(rot_light_pos));

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

            glDisable(GL_CLIP_PLANE0);
            glDisable(GL_BLEND);
        }
};
