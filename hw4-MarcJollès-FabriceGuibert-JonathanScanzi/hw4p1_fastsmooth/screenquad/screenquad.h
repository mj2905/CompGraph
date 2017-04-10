#pragma once
#include "icg_helper.h"
#define TEX_1 "tex"
#define TEX_2 "tex2"

#include <vector>


class ScreenQuad {

    private:
        GLuint vertex_array_id_, program_id_, vertex_buffer_object_; // vertex array object, GLSL shader program ID, memory buffer
        float screenquad_width_, screenquad_height_;
        int size;
        float standard_dev_ = 2.0;
        vector<float> G;

        GLuint text1_id_, tex2_id_;

    public:

        void Init(float screenquad_width, float screenquad_height,
                  GLuint texture, GLuint texture2) {

            // set screenquad size
            this->screenquad_width_ = screenquad_width;
            this->screenquad_height_ = screenquad_height;

            // compile the shaders
            program_id_ = icg_helper::LoadShaders("screenquad_vshader.glsl",
                                                  "screenquad_fshader.glsl");
            if(!program_id_) {
                exit(EXIT_FAILURE);
            }

            glUseProgram(program_id_);

            // vertex one vertex Array
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);

            // vertex coordinates
            {
                const GLfloat vertex_point[] = { /*V1*/ -1.0f, -1.0f, 0.0f,
                                                 /*V2*/ +1.0f, -1.0f, 0.0f,
                                                 /*V3*/ -1.0f, +1.0f, 0.0f,
                                                 /*V4*/ +1.0f, +1.0f, 0.0f};
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

            // texture coordinates
            {
                const GLfloat vertex_texture_coordinates[] = { /*V1*/ 0.0f, 0.0f,
                                                               /*V2*/ 1.0f, 0.0f,
                                                               /*V3*/ 0.0f, 1.0f,
                                                               /*V4*/ 1.0f, 1.0f};

                // buffer
                glGenBuffers(1, &vertex_buffer_object_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_texture_coordinates),
                             vertex_texture_coordinates, GL_STATIC_DRAW);

                // attribute
                GLuint vertex_texture_coord_id = glGetAttribLocation(program_id_,
                                                                     "vtexcoord");
                glEnableVertexAttribArray(vertex_texture_coord_id);
                glVertexAttribPointer(vertex_texture_coord_id, 2, GL_FLOAT,
                                      DONT_NORMALIZE, ZERO_STRIDE,
                                      ZERO_BUFFER_OFFSET);
            }

            // load/Assign texture1
            text1_id_ = texture;
            glBindTexture(GL_TEXTURE_2D, texture);
            GLuint tmp = glGetUniformLocation(program_id_, "tex");
            glUniform1i(tmp, 0);

            // load/Assign texture2
            tex2_id_ = texture2;
            glBindTexture(GL_TEXTURE_2D, texture2);
            GLuint tmp2 = glGetUniformLocation(program_id_, "tex2");
            glUniform1i(tmp2, 1);

            // to avoid the current object being polluted
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindVertexArray(0);
            glUseProgram(0);
        }


        void Cleanup() {
            glBindVertexArray(0);
            glUseProgram(0);
            glDeleteBuffers(1, &vertex_buffer_object_);
            glDeleteProgram(program_id_);
            glDeleteVertexArrays(1, &vertex_array_id_);
            glDeleteTextures(1, &text1_id_);
            glDeleteTextures(1, &tex2_id_);
        }

        void UpdateSize(int screenquad_width, int screenquad_height) {
            this->screenquad_width_ = screenquad_width;
            this->screenquad_height_ = screenquad_height;
        }

        void update(float c){
            standard_dev_ = standard_dev_ + c;
        }

        void Draw(int p) {
            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);


            // Create and assign Gaussian Kernel
            int size = 1 + 6 * int(ceil(standard_dev_));

            G.clear();
            for(int i=-size; i<=size; i++){
                G.push_back(exp(-i*i/(2.0*standard_dev_*standard_dev_*standard_dev_*standard_dev_)));
            }

            glUniform1fv(glGetUniformLocation(program_id_, "G"), G.size(), G.data());
            glUniform1i(glGetUniformLocation(program_id_, "size"), G.size());
            // window size uniforms
            glUniform1f(glGetUniformLocation(program_id_, "tex_width"),
                        this->screenquad_width_);
            glUniform1f(glGetUniformLocation(program_id_, "tex_height"),
                        this->screenquad_height_);


            glUniform1i(glGetUniformLocation(program_id_, "SIZE_OPT"),
                        (this->G.size()-1)/2);
            glUniform1fv(glGetUniformLocation(program_id_, "G"), G.size(), G.data());

            // Here, we bind both textures, so that they exist and can be read and written to.
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, text1_id_);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, tex2_id_);

            // Finally, the draw.
            glDrawArrays(GL_TRIANGLE_STRIP, 0,4);

            glBindVertexArray(0);
            glUseProgram(0);
        }
};
