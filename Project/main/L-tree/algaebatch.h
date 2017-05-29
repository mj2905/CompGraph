#pragma once
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "algae.h"
#include "flexigrid.h"

class AlgaeBatch{
private:
    GLuint texture_id_, program_id_, vertex_array_id_, vertex_buffer_object_position_;
    GLuint vertex_buffer_object_index_;
    //vector<Algae> algaes;
    GLuint num_indices_;                    // number of vertices to render
    GLuint M_id_;                           // model matrix ID
    GLuint V_id_;                           // view matrix ID
    GLuint P_id_;
    Flexigrid grid;


    const int size_perm[100] = {
        76,7,84,5,58,59,60,61,12,13,14,93,15,16,94,95,96,17,
        21,22,75,23,66,85,86,24,25,26,87,67,29,30,6,7,8,9,10,35,89,36,37,
        38,39,40,2,3,92,41,100,42,43,44,45,98,99,49,18,20,53,19,54,
        55,56,57,64,65,69,70,46,47,48,32,33,34,
        72,73,74,68,27,28,79,80,81,82,83,84,50,51,52,88,71,31,
        90,91,1,97,62,63,11
    };
    const char letter_perm[2] = {'A','B'};

public:
    void Init(GLuint texture, int algaeNumber){
        program_id_= icg_helper::LoadShaders("algaebatch_vshader.glsl", "algaebatch_fshader.glsl");

        if(!program_id_) {
            exit(EXIT_FAILURE);
        }

        glUseProgram(program_id_);

        glGenVertexArrays(1, &vertex_array_id_);
        glBindVertexArray(vertex_array_id_);

        glUniform1i(glGetUniformLocation(program_id_, "tex"), 0 /*GL_TEXTURE0*/);
        texture_id_ = texture;

        M_id_ = glGetUniformLocation(program_id_, "model");
        V_id_ = glGetUniformLocation(program_id_, "view");
        P_id_ = glGetUniformLocation(program_id_, "projection");

        vector<GLfloat> points;
        vector<GLfloat> normals;
        vector<GLuint> indices;

        int baseIndex = 0;

        for(size_t i = 0; i < algaeNumber; ++i){
            vec3 p = vec3(size_perm[(i+size_perm[(size_perm[(i+4)%100]+2*i)%100])%100],
                    size_perm[(i+size_perm[(size_perm[(i-4)%100]-2*i)%100])%100],
                    0);
            Algae a;
            a.Init((size_perm[i]%8),letter_perm[(i*i-1)%2],p,baseIndex);

            points.push_back(*(a.getVertices().data()));
            normals.push_back(*(a.getNormals().data()));
            indices.push_back(*(a.getIndicesArray().data()));
            baseIndex = a.getIndex();

            ++baseIndex;
            a.Cleanup(); // we basically don't need it anymore

        }

        grid.Init(points, indices, normals, texture_id_);

        // to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);

    }

     void Draw(const glm::mat4 &model = IDENTITY_MATRIX,
               const glm::mat4 &view = IDENTITY_MATRIX,
               const glm::mat4 &projection = IDENTITY_MATRIX){
         grid.Draw(model, view, projection);
     }

};
