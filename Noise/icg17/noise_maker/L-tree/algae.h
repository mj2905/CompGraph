#pragma once
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string.h>
#include <sstream>

// This code is based on L systems described here: http://www.naturewizard.at/tutorial04.html

class Algae {

    private:
       string tree;
       GLuint depth_;
       vector<vec3> states;

    public:
       void Init(GLuint depth, vec3 origin){
            tree = "B";
            depth_ = depth;
            states.push_back(origin);
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

       void drawMethod(vec3 parentDepth, vec3 targetDepth, char parentType, char targetType){
            /* Pour t'expliquer, il y a deux règles de grammaire comme tu l'auras lu plus haut
             * A -> AB et B->A. Dans le deuxième cas.
             * Pour le 2e cas, ça veut dire que parentType ='B' et targetType = 'A'
             * Les profondeurs respectives seront donc (x_B, y_B, z_B) et (x_B, y_B, z_B+1) par exemple.
             * En gros la deuxième coordonnée passée sert vraiment à conserver la "hauteur" mais pas le reste.
             * Le but notamment ici, c'est de calculer en fonction de la lettre parente et celle enfant.
             * Pour B -> A, on a un truc en ligne, donc rien à changer sur la coordonnée fille pour le dessin.
             *
             * Mais dans le cas de A->AB, on fait DEUX appels à cette méthode (l'algo qui appelle drawMethod est itératif,
             * c'est pour ça). Or, là on veut (par simplicité) un truc en fourche pour les deux "filles".
             * Donc la coordonnée du 2e A va être par exemple décallée de 0.5 sur la gauche en X, le B de 0.5 sur la droite.
             * Pour dessiner un quad, il faut quatre points, je ne t'apprends rien.
             * Ici, tu as donc "deux points" par quad; ils représentent les milieux. En déduire les points autour n'est pas très compliqué.
             * Note qu'il existe aussi une méthode en OpenGL pour dessiner des lignes. Toutefois, j'ai estimé que ça n'était pas
             * préférable ici et que nous voudrions un peu...D'épaisseur, disons.
             *
             *
             * */
       }

       // This algae only has branches
       void drawBranch(vec3 origin, vec3 destination){

       }


       void beginTree(){
           firstExpand();
           for(size_t i = 0; i < depth_; ++i){
                expand();
            }
       }

       void printTree(){
           cout << tree << endl;
       }

};
