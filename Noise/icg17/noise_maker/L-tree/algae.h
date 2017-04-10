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
       vector<glm::vec3> states;

    public:
       void Init(GLuint depth, char c){
            stringstream ss;
            ss << c;
            ss >> tree;
            depth_ = depth;
            cout << "boop" << endl;
            states.push_back(vec3(0.0f,0.0f,0.0f));
            cout << "beep" << endl;
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
           }

           return rotation;
       }

       mat4 translationRule(char parentType, char targetType){
           mat4 translation;
           float xf = 0.07f;
           float yf = 0.165f;

           if(parentType == 'A'){
               if(targetType == 'B'){
                    translation = translate(mat4(1.0f), vec3(-xf,yf,0.0f));
               } else{
                   translation = translate(mat4(1.0f), vec3(xf,yf,0.0f));
               }
           } else {
                translation = translate(mat4(1.0f), vec3(0.0f,yf,0.0f));
           }

           return translation;
       }

       vec3 generateQuads(vec3 originPoint, char parentType, char targetType){
           Quad tmp_quad;
           mat4 rotation, translation;

           rotation = rotationRule(parentType, targetType);
           translation = translationRule(parentType, targetType);

           vec4 dest = translation*rotation*vec4(originPoint,1.0f);
           tmp_quad.Init(originPoint, vec3(dest),0.01f);
           quads.push_back(tmp_quad);
           return vec3(dest);
       }

       // The algorithm on paper should work. Does it?
       void generateAlgae(){
           vec3 so,s1;
           so= states.back();
           char lo = 'n';
           char l1 = 'n';
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
                        so = generateQuads(s1, l1, lo);
                        states.push_back(so);
                    }
               } else if(str == '>'){
                   branches.push_back(lo);
                   states.push_back(so);
               }
           }
           branches.clear();
           states.clear();

       }

       // Verified: Parses + Generate the quads & transfos
       void initTree(){
           firstExpand();
           for(size_t i = 0; i < depth_; ++i){
                expand();
            }
           generateAlgae();
       }


       void Draw(){
           for(size_t i = 0; i < quads.size(); ++i){
               quads.at(i).Draw(IDENTITY_MATRIX);
           }
       }

       void Cleanup(){
           for(size_t i = 0; i < quads.size(); ++i){
               quads.at(i).Cleanup();
           }
           states.clear();
           branches.clear();
           quads.clear();
           tree.clear();
       }

};
