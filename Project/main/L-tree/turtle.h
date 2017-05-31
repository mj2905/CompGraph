#pragma once

#include <string.h>
#include <sstream>

using namespace glm;

/**
 * @brief The Turtle class
 */
class Turtle {

    private:
       string tree;
       GLuint depth_;
       char axiom;

    public:
       /**
        * @brief Init
        * @param depth The maximum depth of the tree
        * @param axiom The original axiom (ie: starting letter) of the tree, between A and B
        */
       void Init(GLuint depth, char axiom){
            axiom = axiom;
            stringstream ss;
            ss << axiom;
            ss >> tree;

            depth_ = depth;
            initTree();
       }


       static float sqr(float x){
           return x*x;
       }


       float norm(vec3 a, vec3 b){
           return sqrt(sqr(a.x - b.x) + sqr(a.y - b.y) + sqr(a.z - b.z));
       }

       int treeLength(){
           return tree.size();
       }

       char charAt(int i){
           return tree.at(i);
       }


       void printTree(){
           cout << tree << endl;
       }

       /**
        * @brief substituteString, according to a set of rules replaces a character with its children
        * @param a the character to substitute
        * @return the substituted string
        */
       string substituteString(char a){
           if(a == 'A'){
               return "A[AB]";
           }
           else if(a == 'B'){
               return "B[A]";
           }else if(a == 'C'){
               return "C[ABC]";
           }else {
               stringstream ss;
               ss << a;
               string s;
               ss >> s;
               return s;
           }
       }

       /**
        * @brief firstExpand, it is the first expansion of the tree, done this way to avoid some bugs with the usual expansion
        */
       void firstExpand(){
           tree.replace(0,1, substituteString(tree.at(0)));
       }

       /**
        * @brief expand, rest of the expansion of the tree
        */
       void expand(){
           for(size_t i = 1; i < tree.size()-1; ++i){
               char ch = tree.at(i);
               //char chcontr = tree.at(i-1);
               char chmor = tree.at(i+1);
               if(chmor != '['){
                   string s = substituteString(ch);
                   tree.replace(i, 1, s);
                   i+= s.size()-1;
               }
           }
       }


       /**
        * @brief initTree
        * Puts everything together: expands the tree from the axiom and then generates the algae
        */
       void initTree(){
           firstExpand();
           for(size_t i = 0; i < depth_; ++i){
                expand();
            }
       }

       void Cleanup(){
           tree.clear();
       }

};
