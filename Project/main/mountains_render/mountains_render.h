#pragma once
#include "../constants.h"
#include "../globals.h"
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>
#include <array>

class MountainsRender {

    private:
        GLuint vertex_array_id_;                // vertex array object
        GLuint vertex_buffer_object_position_;  // memory buffer for positions
        GLuint vertex_buffer_object_index_;     // memory buffer for indices
        GLuint program_id_;                     // GLSL shader program ID

        GLuint texture_id_;
        GLuint grass_id_;
        GLuint rock_id_;
        GLuint snow_id_;
        GLuint sand_id_;

        GLuint light_pos_id;
        glm::vec3 light_pos;

        //shadow
        GLuint shadow_vp_id;
        GLuint shadow_mvp_id;
        GLuint shadowmap_id;

        GLuint num_indices_;                    // number of vertices to render
        GLuint M_id_;                           // model matrix ID
        GLuint V_id_;                           // view matrix ID
        GLuint P_id_;                           // proj matrix ID

    public:
        void Init(GLuint mountain_texture, size_t grid_dim = 512) {
            // compile the shaders.
            program_id_ = icg_helper::LoadShaders("mountains_r_vshader.glsl",
                                                  "mountains_r_fshader.glsl");
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
                texture_id_ = mountain_texture;
            }

            // lights and shading
            {
                light_pos = glm::vec3(0.0f, 1, -1);

                glm::vec3 Ld = glm::vec3(1.0f, 1.0f, 0.8f);

                light_pos_id = glGetUniformLocation(program_id_, "light_pos");

                GLuint Ld_id = glGetUniformLocation(program_id_, "Ld");

                glm::vec3 kd = glm::vec3(0.3f);

                GLuint kd_id = glGetUniformLocation(program_id_, "kd");

                glUniform3fv(light_pos_id, 1, glm::value_ptr(light_pos));
                glUniform3fv(Ld_id, 1, glm::value_ptr(Ld));
                glUniform3fv(kd_id, ONE, glm::value_ptr(kd));


                GLuint fog_threshold_id = glGetUniformLocation(program_id_, "fog_threshold");
                glUniform1f(fog_threshold_id, FOG_THRESHOLD);
            }

            loadImage("grass.jpg", "grass", 1, grass_id_);
            loadImage("rock.jpg", "rock", 2, rock_id_);
            loadImage("snow.jpg", "snow", 3, snow_id_);
            loadImage("sand.jpg", "sand", 4, sand_id_);

            // other uniforms
            shadow_vp_id = glGetUniformLocation(program_id_, "shadow_vp");
            shadow_mvp_id = glGetUniformLocation(program_id_, "shadow_mvp");
            shadowmap_id = shadowmap.get_shadowmap_id();
            glUniform1i(glGetUniformLocation(program_id_, "shadowmap"), 5);

            M_id_ = glGetUniformLocation(program_id_, "model");
            V_id_ = glGetUniformLocation(program_id_, "view");
            P_id_ = glGetUniformLocation(program_id_, "projection");

            // to avoid the current object being polluted
            glBindVertexArray(0);
            glUseProgram(0);
        }

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

        void Cleanup() {
            glBindVertexArray(0);
            glUseProgram(0);
            glDeleteBuffers(1, &vertex_buffer_object_position_);
            glDeleteBuffers(1, &vertex_buffer_object_index_);
            glDeleteVertexArrays(1, &vertex_array_id_);
            glDeleteProgram(program_id_);
            glDeleteTextures(1, &texture_id_);
            glDeleteTextures(1, &grass_id_);
            glDeleteTextures(1, &rock_id_);
            glDeleteTextures(1, &snow_id_);
            glDeleteTextures(1, &sand_id_);
        }

        void Draw(float offsetX, float offsetY, bool underwaterclip,
                  const glm::mat4 &model = IDENTITY_MATRIX,
                  const glm::mat4 &view = IDENTITY_MATRIX,
                  const glm::mat4 &projection = IDENTITY_MATRIX) {

                    glEnable(GL_CLIP_PLANE0);

                    if (!in_shadowmap) {
                      glUseProgram(program_id_);
                    }
                      glBindVertexArray(vertex_array_id_);


                      // bind textures
                      glActiveTexture(GL_TEXTURE0);
                      glBindTexture(GL_TEXTURE_2D, texture_id_);

                      // bind textures
                      glActiveTexture(GL_TEXTURE1);
                      glBindTexture(GL_TEXTURE_2D, grass_id_);

                      // bind textures
                      glActiveTexture(GL_TEXTURE2);
                      glBindTexture(GL_TEXTURE_2D, rock_id_);

                      // bind textures
                      glActiveTexture(GL_TEXTURE3);
                      glBindTexture(GL_TEXTURE_2D, snow_id_);

                      // bind textures
                      glActiveTexture(GL_TEXTURE4);
                      glBindTexture(GL_TEXTURE_2D, sand_id_);

                      // bind shadowmap
                      glActiveTexture(GL_TEXTURE5);
                      glBindTexture(GL_TEXTURE_2D, shadowmap_id);




                      glm::vec2 offset = glm::vec2(offsetX, offsetY);

                      glUniform2fv(glGetUniformLocation(program_id_, "offset"), 1, glm::value_ptr(offset));
                      glUniform1i(glGetUniformLocation(program_id_, "clip"), underwaterclip);

                      glm::vec3 rot_light_pos =  glm::mat3(glm::rotate(IDENTITY_MATRIX, (float)glfwGetTime()/100-1, glm::vec3(0,1,0))) * light_pos;

                      glUniform3fv(light_pos_id, 1, glm::value_ptr(rot_light_pos));

                      // setup MVP
                      glUniformMatrix4fv(M_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(model));
                      glUniformMatrix4fv(V_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(view));
                      glUniformMatrix4fv(P_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(projection));


                      glm::mat4 MVP = projection * view * model;

                      glm::mat4 biasMatrix(
          0.5, 0.0, 0.0, 0.0,
          0.0, 0.5, 0.0, 0.0,
          0.0, 0.0, 0.5, 0.0,
          0.5, 0.5, 0.5, 1.0
          );
          glm::mat4 depthBiasMVP = biasMatrix * MVP;

                      glm::mat4 shadowVP = shadow_projection * shadow_view;

                      glUniformMatrix4fv(shadow_vp_id, ONE, DONT_TRANSPOSE, glm::value_ptr(shadowVP));
                      glUniformMatrix4fv(shadow_mvp_id, ONE, DONT_TRANSPOSE, glm::value_ptr(depthBiasMVP));

                      // draw
                      // TODO 5: for debugging it can be helpful to draw only the wireframe.
                      // You can do that by uncommenting the next line.
                      //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                      // TODO 5: depending on how you set up your vertex index buffer, you
                      // might have to change GL_TRIANGLE_STRIP to GL_TRIANGLES.
                      //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                      glDrawElements(GL_TRIANGLES, num_indices_, GL_UNSIGNED_INT, 0);

                      glBindVertexArray(0);

                      if (!in_shadowmap) {
                        glUseProgram(0);
                    }
                      glDisable(GL_CLIP_PLANE0);





        }
};
