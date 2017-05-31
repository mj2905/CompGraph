#pragma once
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>
#include <vector>

struct Light {
        glm::vec3 La = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 Ld = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 Ls = glm::vec3(1.0f, 1.0f, 1.0f);

        glm::vec3 light_pos = glm::vec3(0.0f, 0.0f, 2.0f);

        // pass light properties to the shader
        void Setup(GLuint program_id) {
            glUseProgram(program_id);

            // given in camera space
            GLuint light_pos_id = glGetUniformLocation(program_id, "light_pos");

            GLuint La_id = glGetUniformLocation(program_id, "La");
            GLuint Ld_id = glGetUniformLocation(program_id, "Ld");
            GLuint Ls_id = glGetUniformLocation(program_id, "Ls");

            glUniform3fv(light_pos_id, ONE, glm::value_ptr(light_pos));
            glUniform3fv(La_id, ONE, glm::value_ptr(La));
            glUniform3fv(Ld_id, ONE, glm::value_ptr(Ld));
            glUniform3fv(Ls_id, ONE, glm::value_ptr(Ls));
        }
};

struct Material {
        glm::vec3 ka = glm::vec3(0.9f, 0.05f, 0.05f);
        glm::vec3 kd = glm::vec3(0.9f, 0.5f, 0.5f);
        glm::vec3 ks = glm::vec3(0.7f, 0.7f, 0.7f);
        float alpha = 60.0f;

        // pass material properties to the shaders
        void Setup(GLuint program_id) {
            glUseProgram(program_id);

            GLuint ka_id = glGetUniformLocation(program_id, "ka");
            GLuint kd_id = glGetUniformLocation(program_id, "kd");
            GLuint ks_id = glGetUniformLocation(program_id, "ks");
            GLuint alpha_id = glGetUniformLocation(program_id, "alpha");

            glUniform3fv(ka_id, ONE, glm::value_ptr(ka));
            glUniform3fv(kd_id, ONE, glm::value_ptr(kd));
            glUniform3fv(ks_id, ONE, glm::value_ptr(ks));
            glUniform1f(alpha_id, alpha);
        }
};


class Flexigrid: public Material, public Light {

    private:
        GLuint vertex_array_id_;                // vertex array object
        GLuint vertex_buffer_object_position_;  // memory buffer for positions
        GLuint vertex_buffer_object_index_;     // memory buffer for indices
        GLuint vertex_normal_buffer_object_;    // memory buffer for normals
        GLuint program_id_;                     // GLSL shader program ID
        GLuint num_indices_;                    // number of vertices to render
        GLuint MVP_id_;                         // model, view, proj matrix ID
        GLuint texture_id_;
        constexpr static size_t PERM_SIZE = 512;
        //We have here the permuattion array, doubled, as we don't want to make each time the duplication job (huge factor of lags)
        const int perm[PERM_SIZE] = { 151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,
        142,8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,219,
        203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,
        74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,
        105,92,41,55,46,245,40,244,102,143,54, 65,25,63,161,1,216,80,73,209,76,132,
        187,208, 89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,
        64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,
        47,16,58,17,182,189,28,42,223,183,170,213,119,248,152, 2,44,154,163, 70,221,
        153,101,155,167, 43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185,
        112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,
        235,249,14,239,107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,
        127, 4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,
        156,180,151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,
        142,8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,219,
        203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,
        74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,
        105,92,41,55,46,245,40,244,102,143,54, 65,25,63,161,1,216,80,73,209,76,132,
        187,208, 89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,
        64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,
        47,16,58,17,182,189,28,42,223,183,170,213,119,248,152, 2,44,154,163, 70,221,
        153,101,155,167, 43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185,
        112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,
        235,249,14,239,107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,
        127, 4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,
        156,180
        };


    public:
        void BindShader(GLuint program_id) {
            glUseProgram(program_id);
            glBindVertexArray(vertex_array_id_);

            // vertex attribute id for positions
            GLint vertex_point_id = glGetAttribLocation(program_id, "vpoint");
            if (vertex_point_id >= 0) {
                glEnableVertexAttribArray(vertex_point_id);

                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_position_);
                glVertexAttribPointer(vertex_point_id, 3 /*vec3*/, GL_FLOAT,
                                      DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            }

            // vertex attribute id for normals
            GLint vertex_normal_id = glGetAttribLocation(program_id, "vnormal");
            if (vertex_normal_id >= 0) {
                glEnableVertexAttribArray(vertex_normal_id);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_normal_buffer_object_);
                glVertexAttribPointer(vertex_normal_id, 3 /*vec3*/, GL_FLOAT,
                                      DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            }

        }

        void UnbindShader(GLuint program_id) {
            GLint vertex_point_id = glGetAttribLocation(program_id, "vpoint");
            if (vertex_point_id >= 0) {
                glDisableVertexAttribArray(vertex_point_id);
            }

            GLint vertex_normal_id = glGetAttribLocation(program_id, "vnormal");
            if (vertex_normal_id >= 0) {
                glDisableVertexAttribArray(vertex_normal_id);
            }

            glUseProgram(0);
            glBindVertexArray(0);
        }



        void Init(vector<GLfloat> vertices, vector<GLuint> indices, vector<GLfloat> normals, GLuint texture) {
            // compile the shaders.
            program_id_ = icg_helper::LoadShaders("flexigrid_vshader.glsl",
                                                  "flexigrid_fshader.glsl");
            if(!program_id_) {
                exit(EXIT_FAILURE);
            }

            glUseProgram(program_id_);

            // vertex one vertex array
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);

            texture_id_ = texture;

            // vertex coordinates and indices
            {

                num_indices_ = indices.size();

                // position buffer
                glGenBuffers(1, &vertex_buffer_object_position_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_position_);
                glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
                             &vertices[0], GL_STATIC_DRAW);

                // normal buffer
                glGenBuffers(ONE, &vertex_normal_buffer_object_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_normal_buffer_object_);
                glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat),
                             &normals[0], GL_STATIC_DRAW);

                cout << vertices.size() << endl;
                cout << normals.size() << endl;


                // vertex indices/*
                glGenBuffers(1, &vertex_buffer_object_index_);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_index_);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
                             &indices[0], GL_STATIC_DRAW);

                // position shader attribute
                GLuint loc_position = glGetAttribLocation(program_id_, "position");
                glEnableVertexAttribArray(loc_position);
                glVertexAttribPointer(loc_position, 3, GL_FLOAT, DONT_NORMALIZE,
                                      ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            }

            // other uniforms
            MVP_id_ = glGetUniformLocation(program_id_, "MVP");


            glUniform1iv(glGetUniformLocation(program_id_, "p"), PERM_SIZE, perm);

            glBindTexture(GL_TEXTURE_2D, texture_id_);
            GLuint tex_id = glGetUniformLocation(program_id_, "tex");
            glUniform1i(tex_id, 0);
            glBindTexture(GL_TEXTURE_2D, 0);

            // to avoid the current object being polluted
            glBindVertexArray(0);
            glUseProgram(0);
        }

        void Cleanup() {
            glBindVertexArray(0);
            glUseProgram(0);
            glDeleteBuffers(1, &vertex_buffer_object_position_);
            glDeleteBuffers(1, &vertex_buffer_object_index_);
            glDeleteBuffers(1, &vertex_normal_buffer_object_);
            glDeleteVertexArrays(1, &vertex_array_id_);
            glDeleteTextures(1, &texture_id_);
            glDeleteProgram(program_id_);
        }

        void Draw(const glm::mat4 &model = IDENTITY_MATRIX,
                  const glm::mat4 &view = IDENTITY_MATRIX,
                  const glm::mat4 &projection = IDENTITY_MATRIX) {
            BindShader(program_id_);

            Material::Setup(program_id_);
            Light::Setup(program_id_);



            float scale = 0.01;
            glm::mat4 M = model;
            //M = glm::translate(M, glm::vec3(0.0f, 0.0f, 0.5f));
           // M = glm::rotate(M, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
            M = glm::scale(M, glm::vec3(scale));
            // setup MVP
            glm::mat4 MVP = projection*view*model;
            glUniformMatrix4fv(MVP_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(MVP));

/*
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_id_);*/


            // set up spot light if needed
            GLint spot_dir_id = glGetUniformLocation(program_id_, "spot_dir");
            if (spot_dir_id >=0) {
                glm::vec3 spot_dir = light_pos;
                glUniform3fv(spot_dir_id, ONE, glm::value_ptr(spot_dir));
            }
            // setup matrix stack
            GLint model_id = glGetUniformLocation(program_id_,
                                                  "model");
            glUniformMatrix4fv(model_id, ONE, DONT_TRANSPOSE, glm::value_ptr(M));
            GLint view_id = glGetUniformLocation(program_id_,
                                                 "view");
            glUniformMatrix4fv(view_id, ONE, DONT_TRANSPOSE, glm::value_ptr(view));
            GLint projection_id = glGetUniformLocation(program_id_,
                                                       "projection");
            glUniformMatrix4fv(projection_id, ONE, DONT_TRANSPOSE,
                               glm::value_ptr(projection));
            glDrawElements(GL_TRIANGLES, /*#vertices*/ num_indices_,
                    GL_UNSIGNED_INT, ZERO_BUFFER_OFFSET);
            UnbindShader(program_id_);

            glBindVertexArray(0);
            glUseProgram(0);
        }
};


