#pragma once
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string.h>
#include <sstream>
#include "../quad/quad.h"

// This code is based on L systems described here: http://www.naturewizard.at/tutorial04.html

using namespace glm;
/*
struct QuadLists {
        vector<Quad> quads;
        // pass material properties to the shaders
};*/

class Algae {

    private:
       string tree;
       GLuint depth_;
       vector<glm::vec3> states;
       vector<char> branches;
       vector<Quad> quads;
       vector<glm::mat4> transfos;

    public:
       void Init(GLuint depth, vec3 origin){
            tree = "A";
            depth_ = depth;
            states.push_back(origin);

            initTree();
       }



       string substituteString(char a){
           if(a == 'A'){
               return "[A->+AB]";
           }
           else if(a == 'B'){
               return "[B->+A]";
           }else {
               stringstream ss;
               ss << a;
               string s;
               ss >> s;
               return s;
           }
       }

       void firstExpand(){
           tree.replace(0,1, substituteString(tree.at(0)));
       }

       void expand(){

           for(size_t i = 1; i < tree.size(); ++i){
               char ch = tree.at(i);
               char chcontr = tree.at(i-1);
               if(chcontr == '+' || chcontr == ']'){ // 1st cond expands A->AB as A->[A->AB]B. 2nd takes care of ]B
                   string s = substituteString(ch);
                   tree.replace(i, 1, s);
                   i+= s.size()-1;
               }
           }
       }

       vec3 generateQuads(vec3 originDepth, char parentType, char targetType){
           vec4 dest =  vec4(originDepth,1.0); // vec3(originDepth.x, originDepth.y, originDepth.z + 0.25)
           mat4 t = scale(mat4(1.0f), vec3(0.01f,0.05f,0));
           mat4 transf = t*IDENTITY_MATRIX;
           if(parentType == 'A'){
               if(targetType == 'B'){
                    transf = rotate(mat4(1.0f),(float) (45.0*3.14159/180.0), vec3(0.0f,0.0f,1.0f))*transf;
               } else if(targetType == 'A'){
                    transf = rotate(mat4(1.0f), (float) ((-45.0)*3.14159/180.0), vec3(0.0f,0.0f,1.0f))*transf;
               }
               // here we'll rotate our dear dest vector

           } else if(parentType == 'B'){
               if(targetType == 'A'){ // only possible case for now
                    // we do nothing
               }
           }
           transf = transf*translate(mat4(1.0f), vec3(0.25f,0.25f,0.0f));
           Quad tmp_quad;
           tmp_quad.Init();
           quads.push_back(tmp_quad);
           dest = transf*dest;
           transfos.push_back(transf);

           return vec3(dest);
       }


       void generateAlgae(){
           glm::vec3 so = states.back();
           glm::vec3 s1;
           char lo = 'n';
           char l1 = 'n';
           cout << branches.size() << endl;
           for(size_t i = 1; i < tree.length(); i++){
               char str = tree.at(i);
               if(str == ']'){
                   states.pop_back();
                   branches.pop_back();
                   so = states.back();
               } else if(str == 'A' || tree.at(i) == 'B'){
                    lo = str;
                    if(states.size() > 0 && branches.size() > 0){
                        states.pop_back();
                        s1 = states.back();
                        l1 = branches.back();
                        so = generateQuads(s1, lo, l1);
                        states.push_back(so);
                    }
               } else if(str == '>'){
                   branches.push_back(lo);
                   states.push_back(so);
               }
           }

       }

       void initTree(){
           firstExpand();
           for(size_t i = 0; i < depth_; ++i){
                expand();
            }
           generateAlgae();
       }

       void printTree(){
           cout << tree << endl;
       }

       void Draw(){
           /*vector<Quad> v;
           Quad q1, q2, q3;
           q1.Init();
           q2.Init();
           q3.Init();
           v.push_back(q1);
           v.push_back(q2);
           vector<mat4> t;
           t.push_back(rotate(mat4(1.0f),(float)(45.0*3.14159/180.0), vec3(0.0f,0.0f,1.0f))*scale(mat4(1.0f), vec3(0.01f,0.5f,0)));
           t.push_back(rotate(mat4(1.0f),(float)((-45.0)*3.14159/180.0), vec3(0.0f,0.0f,1.0f))*scale(mat4(1.0f), vec3(0.01f,0.5f,0)));
           v.at(0).Draw(t.at(0));
           v.at(1).Draw(t.at(1));*/
/*
           for(size_t i =0; i < v.size(); ++i){
               v.at(i).Draw(t.at(i));
           }*/


           for(size_t i = 0; i < quads.size(); ++i){
               quads.at(i).Draw(transfos.at(i));
           }
       }

       void Cleanup(){
           for(size_t i = 0; i < quads.size(); ++i){
               quads.at(i).Cleanup();
           }
       }

};
