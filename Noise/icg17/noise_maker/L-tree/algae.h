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
       vector<glm::mat4> transfos, rotations;
       vector<int> indices;

    public:
       void Init(GLuint depth, char c){
            stringstream ss;
            ss << c;
            ss >> tree;
            depth_ = depth;
            transfos.push_back(IDENTITY_MATRIX);
            indices.push_back(0);
            initTree();
       }


       void printTree(){
           cout << tree << endl;
       }

       // Verified: Parsing
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

       // Verified: Parsing
       void firstExpand(){
           tree.replace(0,1, substituteString(tree.at(0)));
       }

       // Verified: Parsing
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

       // The drawing algorithm that chains transformations. Are they correctly ordered?
       // Hypothesis: problem comes from chained rotations that should not occur
       // Ex: Trans1*Rot1*Scale*Quad1
       //     Trans2*Rot2*Trans1*Rot1*Scale*Quad2 if quad2 is child of Quad1
       //  But what we might want here would actually be Trans2*Rot2*(-Rot1)*Trans1*Rot1*Scale*Quad2
       int generateQuads(int transfoIndex, char parentType, char targetType){
           Quad tmp_quad;
           mat4 rotation, translation;
           tmp_quad.Init();
           translation = translate(mat4(1.0f), vec3(0.0f,0.165f,0.0f));
           quads.push_back(tmp_quad);


           if(parentType == 'A'){
               if(targetType == 'B'){
                    rotation = rotate(mat4(1.0f), (float)((45.0)*M_PI/180.0),vec3(0.0f,0.0f,1.0f));
                    translation = translate(mat4(1.0f), vec3(-0.07f,0.165f,0.0f));
               } else{
                   rotation = rotate(mat4(1.0f), (float)((-45.0)*3.14159/180.0),vec3(0.0f,0.0f,1.0f));
                   translation = translate(mat4(1.0f), vec3(0.07f,0.165f,0.0f));
               }
           } else if(parentType == 'B'){
               if(targetType == 'A'){
                   rotation = rotate(mat4(1.0f), (float)((45.0)*M_PI/180.0),vec3(0.0f,0.0f,1.0f));
               }
           } else {
               rotation = rotate(mat4(1.0f), (float)((45.0)*M_PI/180.0),vec3(0.0f,0.0f,1.0f));
           }

           rotations.push_back(rotation);
           transfos.push_back(translation*transfos.at(transfoIndex));
           int index = transfos.size()-1;
           indices.push_back(index);
           return index;
       }

       // The algorithm on paper should work. Does it?
       void generateAlgae(){           
           int so = indices.back();
           int s1;
           char lo = 'n';
           char l1 = 'n';
           for(size_t i = 1; i < tree.length(); i++){
               char str = tree.at(i);
               if(str == ']'){
                   indices.pop_back();
                   branches.pop_back();
                   so = indices.back();
               } else if(str == 'A' || tree.at(i) == 'B'){
                    lo = str;
                    if(indices.size() > 0 && branches.size() > 0){
                        indices.pop_back();
                        s1 = indices.back();
                        l1 = branches.back();
                        cout << l1 << " - >" << lo << endl;
                        so = generateQuads(s1, l1, lo);
                        cout << so << endl;
                        indices.push_back(so);
                    }
               } else if(str == '>'){
                   branches.push_back(lo);
                   indices.push_back(so);
               }
           }
           branches.clear();
           indices.clear();

       }

       // Verified: Parses + Generate the quads & transfos
       void initTree(){
           firstExpand();
           for(size_t i = 0; i < depth_; ++i){
                expand();
            }
           generateAlgae();
       }

       // Uses hypothesis that the transf. of a quad is at the same indice
       void Draw(){
           mat4 t = IDENTITY_MATRIX;
           for(size_t i = 0; i < quads.size(); ++i){
               quads.at(i).Draw(transfos.at(i)*rotations.at(i)*scale(mat4(1.0f), vec3(0.01f,0.1f,0)));
           }
       }

       void Cleanup(){
           for(size_t i = 0; i < quads.size(); ++i){
               quads.at(i).Cleanup();
           }
           indices.clear();
           branches.clear();
           quads.clear();
           transfos.clear();
           tree.clear();
       }

};
