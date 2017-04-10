#pragma once
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string.h>
#include <sstream>
#include "../quad/quad.h"

// This code is based on L systems described here: http://www.naturewizard.at/tutorial04.html

using namespace glm;

class Algae {

    private:
       string tree;
       GLuint depth_;
       vector<char> branches;
       vector<Quad> quads;
       vector<glm::mat4> transfos, rotations;
       vector<int> indices, pointers;

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


       // Adding randomness choices to this rotation later on
       mat4 rotationRule(char parentType, char targetType){
           mat4 rotation;
           if(parentType == 'A'){
               if(targetType == 'B'){
                    rotation = rotate(mat4(1.0f), (float)((45.0)*M_PI/180.0),vec3(0.0f,0.0f,1.0f));
               } else{
                   rotation = rotate(mat4(1.0f), (float)((-45.0)*3.14159/180.0),vec3(0.0f,0.0f,1.0f));
               }
           } else if(parentType == 'B'){
               if(targetType == 'A'){
                   rotation = IDENTITY_MATRIX;
               }
           } else {
               rotation = rotate(mat4(1.0f), (float)((45.0)*M_PI/180.0),vec3(0.0f,0.0f,1.0f));
           }
           return rotation;
       }

       mat4 translationRule(char parentType, char targetType, bool amplify){
           mat4 translation;
           float xf = 0.07f;
           float yf = 0.165f;

           if(amplify){
               xf *=2;
           }

           if(parentType == 'A'){
               if(targetType == 'B'){
                    translation = translate(mat4(1.0f), vec3(-xf,0.0f,0.0f));
               } else{
                   translation = translate(mat4(1.0f), vec3(xf,0.0f,0.0f));
               }
           } else {
                translation = IDENTITY_MATRIX;
           }

           return translation;
       }

       /*
        * Now the translation has a problem of chaining again
        *
        * */

       /*
        *
        *
        *
        * */
       int generateQuads(int transfoIndex, char parentType, char targetType){
           Quad tmp_quad;
           mat4 rotation, translation, transup;
           tmp_quad.Init();
           quads.push_back(tmp_quad);

           rotation = rotationRule(parentType, targetType);

           transup = translate(mat4(1.0f), vec3(0.0f,0.165f, 0.0f));
           mat4 m = transfos.at(transfoIndex);

           rotations.push_back(rotation);
           transfos.push_back(translation*m*transup);
           int index = transfos.size()-1;
           pointers.push_back(index);
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
                        so = generateQuads(s1, l1, lo);
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
           printTree();
           generateAlgae();
       }

       // Uses hypothesis that the transf. of a quad is at the same indice
       void Draw(){
           mat4 t = IDENTITY_MATRIX;
           for(size_t i = 0; i < quads.size(); ++i){
               int index = pointers.at(i);
               quads.at(i).Draw(transfos.at(index)*rotations.at(i)*scale(mat4(1.0f), vec3(0.01f,0.1f,0)));
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
