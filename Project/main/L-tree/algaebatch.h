#pragma once

#include <vector>
#include "algae.h"
#include "flexigrid.h"
#include "../globals.h"


class AlgaeBatch{
private:
    GLuint texture_id_, program_id_, vertex_array_id_, vertex_buffer_object_position_;
    GLuint vertex_buffer_object_index_;
    vector<Algae> algaes;
    GLuint num_indices_;                    // number of vertices to render
    GLuint M_id_;                           // model matrix ID
    GLuint V_id_;                           // view matrix ID
    GLuint P_id_;
    Flexigrid grid;
    Terrain terrain;


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
    void Init(Terrain &terrain, GLuint texture, int algaeNumber,
              int window_width, int window_height){

        float p = 0;
        p = terrain.getCurrentHeight(0,0);

        Algae a;
        float x = -15;
        float y = 50;
        p = terrain.getCurrentHeight(x,y);
        cout << p << endl;
        a.Init(8, 'A', vec3(x,40,y), texture, 0);
        algaes.push_back(a);
    }

     void Draw(const glm::mat4 &model = IDENTITY_MATRIX,
               const glm::mat4 &view = IDENTITY_MATRIX,
               const glm::mat4 &projection = IDENTITY_MATRIX){
         for(auto a: algaes){
             a.Draw(model, view, projection);
         }
         //grid.Draw(model, view, projection);
     }

};
