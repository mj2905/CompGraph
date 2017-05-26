#pragma once
#include "icg_helper.h"

class Shadow {

private:
    GLuint vertex_array_id_;                // vertex array object
    GLuint vertex_buffer_object_position_;  // memory buffer for positions
    GLuint vertex_buffer_object_index_;     // memory buffer for indices
    GLuint program_id_;                     // GLSL shader program ID

    GLuint shadow_id_;

    GLuint num_indices_;                    // number of vertices to render
    GLuint M_id_;                           // model matrix ID
    GLuint V_id_;                           // view matrix ID
    GLuint P_id_;                           // proj matrix ID

public:
    void Init(GLuint shadow_texture) {
        // compile the shaders.
        program_id_ = icg_helper::LoadShaders("shadow_vshader.glsl",
                                              "shadow_fshader.glsl");
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
            glUniform1i(glGetUniformLocation(program_id_, "shadow"), 0 /*GL_TEXTURE0*/);
            shadow_id_ = shadow_texture;
        }

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
        glDeleteTextures(1, &shadow_id_);
    }

    void Draw(const glm::mat4 &model = IDENTITY_MATRIX,
              const glm::mat4 &view = IDENTITY_MATRIX,
              const glm::mat4 &projection = IDENTITY_MATRIX) {

        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);

        // bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, shadow_id_);

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
