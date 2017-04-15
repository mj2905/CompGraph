#pragma once
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string.h>
#include <sstream>
#include "flexigrid.h"


// This code is based on L systems described here: http://www.naturewizard.at/tutorial04.html

using namespace glm;


class Algae {

    private:
    /*
     * CHANGE TO THE FOLLOWING:
     * CREATE 4 POINTS
     * ADD THEM TO THE BUFFER & INDICES
     * THE 2 UPPER POINTS SERVE AS BASIS FOR NEXT DRAW (must be stacked!)
     * THE WIDTH & HEIGHT ARE ALSO A PARAMETER CONTROLLED THROUGH THE RECURSION
     * IDEA: UNIT VECTOR THAT SHOWS DIRECTION (also stacked!)
     * AFFECTED THROUGH ROTATION
     * NO TRANSLATION AS SUCH; WE TRANSLATE BY THIS VECTOR*HEIGHT VECTOR FOR THE POINTS
     * */

       Flexigrid grid;
       string tree;
       GLuint depth_;
       vector<GLuint> leftIndex, rightIndex, indices;
       vector<vec3> leftPoint, rightPoint, direction;
       vector<GLfloat> vertices;
       vector<char> branches;
       int index_ = 0;
       float init_width = 0.08f;
       float init_length = 5*init_width;
       char axiom;

    public:
       void Init(GLuint depth, char c, vec3 origin, GLuint texture_id){
            axiom = c;
            stringstream ss;
            ss << c;
            ss >> tree;
            depth_ = depth;
            vec3 originLeft = vec3(origin.x - init_width, origin.y, origin.z);
            vec3 originRight = vec3(origin.x + init_width, origin.y, origin.z);

            leftPoint.push_back(originLeft);
            rightPoint.push_back(originRight);

            pushToVertices(originLeft);
            pushToVertices(originRight);

            direction.push_back(vec3(0.0f,1.0f,0.0f));
            leftIndex.push_back(index_);
            index_++;
            rightIndex.push_back(index_);
            initTree();
            grid.Init(vertices, indices, texture_id);
       }

       void pushToVertices(vec3 point){
           vertices.push_back(point.x);
           vertices.push_back(point.y);
           vertices.push_back(point.z);
       }

       float sqr(float x){
           return x*x;
       }

       bool smallerThanWidth(vec3 dir, vec3 p1, vec3 p2){
           vec3 p2top1 = p1 - p2;
           float norm = sqrt(sqr(p2top1.x) + sqr(p2top1.y) + sqr(p2top1.z));
           float angle = acos(normalize(dot(p2top1, dir)));
           float width = sin(angle)*norm;
           return width <= init_width;

       }

       void printTree(){
           cout << tree << endl;
       }

       // Verified: Parsing
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

       // Verified: Parsing
       void firstExpand(){
           tree.replace(0,1, substituteString(tree.at(0)));
       }

       // Verified: Parsing
       void expand(){
           for(size_t i = 1; i < tree.size()-1; ++i){
               char ch = tree.at(i);
               char chcontr = tree.at(i-1);
               char chmor = tree.at(i+1);
               if(chmor != '['){
                   string s = substituteString(ch);
                   tree.replace(i, 1, s);
                   i+= s.size()-1;
               }
           }
       }

       /*
        * This is where the update of the direction is done, depending on the rules defined for the turtle
        *
        * */
       vec3 updateDirection(vec3 direction, char parentType, char targetType){
            vec3 rotation, newDir;
            if(parentType == 'A'){
                if(targetType == 'B'){
                    rotation = vec3(0.0f,0.0f,25.0f);
                } else if(targetType == 'A') {
                    rotation = vec3(0.0f,0.0f,-25.0f);
                }
            } else {
                rotation = vec3(0.0f,0.0f,0.0f);
            }
            rotation*= M_PI/180.0;
            newDir = vec3(glm::toMat4(quat(rotation))*vec4(direction,1.0f));
            newDir = normalize(newDir);
            newDir *= init_length;
            return newDir;
       }

       vec3 updatePoint(vec3 direction, vec3 originalPoint){
           return direction+originalPoint;
       }

       vec3 popDirection(){
           direction.pop_back();
           return direction.back();
       }

       vec3 popLeftPoint(){
           leftPoint.pop_back();
           return leftPoint.back();
       }

       vec3 popRightPoint(){
           rightPoint.pop_back();
           return rightPoint.back();
       }

       int popLeftIndex(){
           leftIndex.pop_back();
           return leftIndex.back();
       }

       int popRightIndex(){
           rightIndex.pop_back();
           return rightIndex.back();
       }

       void updateTriangleIndicesAndIndexes(vec3 newPoint, int dlid, int drid, int upid){
           pushToVertices(newPoint);
           indices.push_back(dlid);
           indices.push_back(drid);
           indices.push_back(upid);
       }

       void updateIndicesAndIndexes(vec3 ulpoint, vec3 urpoint,
                                    int dlid, int drid, int ulid, int urid){

               pushToVertices(ulpoint);
               pushToVertices(urpoint);
               indices.push_back(dlid);
               indices.push_back(drid);
               indices.push_back(ulid);
               indices.push_back(ulid);
               indices.push_back(drid);
               indices.push_back(urid);
       }


       // Must stack: - left point & right point (@ leftPoint, rightPoint)
       //             - left index & right index (@ leftIndex, rightIndex)
       //             - direction (@direction)
       //             - parent (@ branches)
       // Must update: - indices
       //              - vertices
       void generateAlgae(){
           vec3 dir0,dir1;
           dir0 = direction.back();

           vec3 leftp0, leftp1;
           leftp0 = leftPoint.back();
           vec3 rightp0, rightp1;
           rightp0 = rightPoint.back();

           int lefti0, lefti1;
           lefti0 = leftIndex.back();

           int righti0, righti1;
           righti0 = rightIndex.back();

           char lo = 'n';
           char l1 = 'n';
           for(size_t i = 0; i < tree.length(); i++){
               char str = tree.at(i);
               if(str == ']'){
                   init_length *= 1.43;
                   dir0 = popDirection();
                   leftp0 = popLeftPoint();
                   rightp0 = popRightPoint();
                   lefti0 = popLeftIndex();
                   righti0 = popRightIndex();
                   branches.pop_back();
               } else if(str == 'A' || tree.at(i) == 'B'){
                    lo = str;
                    if(direction.size() > 0 && branches.size() > 0 && leftPoint.size() > 0 && rightPoint.size() > 0 && leftIndex.size()>0 && rightIndex.size()>0){
                        l1 = branches.back();

                        dir1 = popDirection();
                        dir0 = updateDirection(dir1, l1, lo);

                        direction.push_back(dir0);
                        leftp1 = popLeftPoint();
                        rightp1 =  popRightPoint();
                        if(!smallerThanWidth(dir0, leftp1, rightp1)){
                            leftp0 = updatePoint(dir0, leftp1);
                            leftPoint.push_back(leftp0);

                            rightp0 = updatePoint(dir0, rightp1);
                            rightPoint.push_back(rightp0);
                            lefti1 = popLeftIndex();
                            lefti0 = ++index_ ;
                            leftIndex.push_back(lefti0);
                            righti1 = popRightIndex();
                            righti0 = ++index_ ;
                            rightIndex.push_back(righti0);
                            updateIndicesAndIndexes(leftp0, rightp0, lefti1, righti1,lefti0, righti0);

                        } else {
                            vec3 dirbase = leftp1 - rightp1;
                            vec3 axis = normalize(cross(dir0, dirbase));
                            vec3 transdir = init_width*normalize(cross(axis,dir0));
                            vec3 newLeftPoint = updatePoint(rightp1,transdir);
                            int leftId = ++index_;
                            lefti1 = popLeftIndex();
                            righti1 = popRightIndex();
                            updateTriangleIndicesAndIndexes(newLeftPoint, lefti1, righti1, leftId);
                            lefti0 = ++ index_;
                            righti0 = ++ index_;
                            leftIndex.push_back(lefti0);
                            rightIndex.push_back(righti0);
                            leftp0 = updatePoint(dir0, newLeftPoint);
                            rightp0 = updatePoint(dir0, rightp1);
                            leftPoint.push_back(leftp0);
                            rightPoint.push_back(rightp0);
                            updateIndicesAndIndexes(leftp0, rightp0, lefti1, righti1, lefti0, righti0);

                            /* the code above creates a new point with correct width */
                            /* the code under still needs to be corrected*/
                        }

                    }
               } else if(str == '['){
                   init_length *=.7;
                   direction.push_back(dir0);
                   leftPoint.push_back(leftp0);
                   rightPoint.push_back(rightp0);
                   leftIndex.push_back(lefti0);
                   rightIndex.push_back(righti0);
                   branches.push_back(lo);
               }
           }

           leftPoint.clear();
           rightPoint.clear();
           leftIndex.clear();
           rightIndex.clear();
           direction.clear();
           branches.clear();

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
           grid.Draw(model, view, projection);
           // DRAW THE GRID
       }

       void Cleanup(){
           leftPoint.clear();
           rightPoint.clear();
           leftIndex.clear();
           rightIndex.clear();
           direction.clear();
           branches.clear();
           grid.Cleanup();
           tree.clear();
       }

};
