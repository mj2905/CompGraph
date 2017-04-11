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
 * TODO: Agrandir la largeur des algues -> y inclure la texture via perlin noise
 * Rendre les coordonnées associées à un rand aléatoire elles aussi, tant qu'à faire
 * Réduire la longueur de chaque branche
 * Tout dessiner en une structure 3D? Ou plutôt en 2D?
 *
 * */

class Algae {

    private:
       string tree;
       GLuint depth_;
       vector<char> branches;
       vector<Quad> quads;
       vector<glm::vec3> states;
       int rand_val;
       float init_width = 0.02f;

    public:
       void Init(GLuint depth, char c, vec3 origin){
            stringstream ss;
            ss << c;
            ss >> tree;
            depth_ = depth;
            srand(110);
            states.push_back(origin);
            initTree();
       }


       void printTree(){
           cout << tree << endl;
       }

       void callRand(){
           srand(rand_val);
           rand_val = rand()%110;
           rand_val = rand();
           cout << rand_val << endl;
           //rand_val = rand()%210;
           //rand_val = rand()%210;
           //rand_val = rand()%210;
           //rand_val = rand()%210;
       }

       // Verified: Parsing
       string substituteString(char a){           
           callRand();
           cout << rand_val << endl;
           int r = rand_val;
           if(a == 'A'){
               if(r > 90){ // r c (90,94]
                   return "[A->+ABC]";
               }else if(r <= 90 && r >40){ // r c (40,90]
                   return "[A->+AB]";
               } else if(r <=40 && r > 20){ // r c (20,  40]
                   return "[A->+B]";
               } else{ // r c [0, 20]
                   return "[A]";
               }
           }
           else if(a == 'B'){
               return "[B->+A]";
           }else if(a == 'C'){
               return "[C->+BA]";
           }else {
               stringstream ss;
               ss << a;
               string s;
               ss >> s;
               return s;
           }
           srand(10);
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


       /*
        * TODO: Add a rand function to create 3D plants
        *
        * */
       mat4 transformationRule(char parentType, char targetType){
           mat4 transformation;
           float xf = 0.08f;
           float yf = 0.15f;

           if(parentType == 'A'){
               if(targetType == 'B'){
                    transformation = translate(mat4(1.0f), vec3(-xf,yf,0.0f));
               } else if(targetType == 'C'){
                   transformation = translate(mat4(1.0f), vec3(-xf,yf,-xf));
               }

               else{
                   transformation = translate(mat4(1.0f), vec3(xf,yf,0.0f));
               }
           } else if(parentType == 'C'){
               if(targetType == 'A'){
                   transformation = translate(mat4(1.0f), vec3(0.0f, yf, xf));
               } else {
                   transformation = translate(mat4(1.0f), vec3(xf, yf, -xf));
               }

           }else{
                transformation = translate(mat4(1.0f), vec3(0.0f,yf,0.0f));
           }

           return transformation;
       }

       vec3 generateQuads(vec3 originPoint, char parentType, char targetType){
           Quad tmp_quad;
           vec4 dest = transformationRule(parentType, targetType)*vec4(originPoint,1.0f);
           tmp_quad.Init(originPoint, vec3(dest),init_width);
           quads.push_back(tmp_quad);
           return vec3(dest);
       }

       // The algorithm on paper should work.
       void generateAlgae(){
           vec3 so,s1;
           so= states.back();
           char lo = 'n';
           char l1 = 'n';
           for(size_t i = 1; i < tree.length(); i++){
               char str = tree.at(i);
               if(str == ']'){
                   init_width *=1.1;
                   states.pop_back();
                   branches.pop_back();
                   so = states.back();
               } else if(str == 'A' || str == 'B' || str == 'C'){
                    lo = str;
                    if(states.size() > 0 && branches.size() > 0){
                        init_width *= 0.9;
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

       void initTree(){
           firstExpand();
           for(size_t i = 0; i < depth_; ++i){
                expand();
            }
           generateAlgae();
       }


       void Draw(const glm::mat4 &model = IDENTITY_MATRIX,
                 const glm::mat4 &view = IDENTITY_MATRIX,
                 const glm::mat4 &projection = IDENTITY_MATRIX){
           for(size_t i = 0; i < quads.size(); ++i){
               quads.at(i).Draw(model,view,projection);
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
