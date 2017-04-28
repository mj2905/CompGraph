#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string.h>
#include <sstream>
#include "flexigrid.h"
#include "rng.h"
#include "turtle.h"

using namespace glm;

/**
 * @brief The Algae class
 */
class Algae {

    private:
       Flexigrid grid;
       Turtle turtle;
       vector<GLuint> leftIndex, rightIndex, indices;
       vector<vec3> leftPoint, rightPoint, direction,triangleNormals;
       vector<GLfloat> vertices, normals;
       vector<char> branches;
       int index_ = 0;
       float init_width = 0.08f;
       float init_length = 5*init_width;
       RNG generator;
       int rand;

    public:
       /**
        * @brief Init
        * @param depth The maximum depth of the tree
        * @param axiom The original axiom (ie: starting letter) of the tree, between A and B
        * @param origin The starting point of the tree
        * @param texture_id The id of the texture
        */
       void Init(GLuint depth, char axiom, vec3 origin, GLuint texture_id){
            generator.Seed(1);
            rand = generator.rand();


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


            turtle.Init(depth, axiom);
            generateAlgae();

            createNormals();

            if(3*triangleNormals.size()!= vertices.size()){
                cout << "Sizes different. Normals should be: " << vertices.size() << " but was " << triangleNormals.size() << endl;
            }

            vector<GLfloat> n;
            for(size_t i =0; i < triangleNormals.size(); ++i){
                n.push_back(triangleNormals.at(i).x);
                n.push_back(triangleNormals.at(i).y);
                n.push_back(triangleNormals.at(i).z);
            }

            grid.Init(vertices, indices, n, texture_id);

       }

       /**
        * @brief pushToVertices
        * @param point
        */
       void pushToVertices(vec3 point){
           vertices.push_back(point.x);
           vertices.push_back(point.y);
           vertices.push_back(point.z);
       }


       /**
        * @brief smallerThanWidth, returns true if the width of the parrallelepiped is smaller than the current width
        * @param dir The direction of the next points (orthogonal to the width of parrallelepiped)
        * @param p1 The left point
        * @param p2 The right point
        * @return true if width of parallelepiped <= width of parent branch
        */
       bool smallerThanWidth(vec3 dir, vec3 p1, vec3 p2){
           vec3 p2top1 = p1 - p2;
           float norm = turtle.norm(p2top1, vec3(0.0f,0.0f,0.0f));
           float angle = acos(normalize(dot(p2top1, dir)));
           float width = sin(angle)*norm;
           return width <= init_width;

       }


       /**
        * @brief updateDirection, from a direction and two letters update the direction depending on a set of rules
        * @param direction The original direction
        * @param parentType Parent letter
        * @param targetType Child letter
        * @return The updated direction as a vec3
        */
       vec3 updateDirection(vec3 direction, char parentType, char targetType){
            vec3 rotation, newDir;
            rand = generator.rand();

            if(parentType == 'A'){
                if(targetType == 'B'){
                    if(rand < 50){
                        rotation = vec3(0.0f,0.0f,25.0f);
                    }else{
                        rotation = vec3(0.0f,-15.0f, 20.0f);
                    }
                } else if(targetType == 'A') {
                    if(rand < 50){
                        rotation = vec3(0.0f, 0.0f,-25.0f);
                    }else{
                        rotation = vec3(0.0f,10.0f,-20.0f);
                    }
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


       void normalAndPushIDs(int id1, int id2, int id3){
           indices.push_back(id1);
           indices.push_back(id2);
           indices.push_back(id3);
       }

       void printTurtle(){
           turtle.printTree();
       }

       void createCubeVolume(vec3 leftBasePoint, vec3 rightBasePoint, vec3 leftUpPoint,
                             int leftBaseID, int rightBaseID, int leftUpID, int rightUpID){

            vec3 b, d, o, newBasePoint, newUpPoint;
            b = leftBasePoint - rightBasePoint;
            newBasePoint = leftBasePoint;
            newUpPoint = leftUpPoint;
            float length = turtle.norm(leftBasePoint, rightBasePoint);
            o = leftUpPoint - leftBasePoint;
            d = length*normalize(cross(b, o));

            int newBaseIndex, newUpIndex, currLeftIndex, currUpIndex;
            currLeftIndex = leftBaseID;
            currUpIndex = leftUpID;

            for(int i =0; i < 2; ++i){
                newBasePoint = newBasePoint+d;
                newUpPoint = newUpPoint +d;
                newBaseIndex = ++index_ ;
                newUpIndex = ++index_ ;
                pushToVertices(newBasePoint);
                pushToVertices(newUpPoint);
                normalAndPushIDs(newBaseIndex, currLeftIndex, currUpIndex);
                normalAndPushIDs(currUpIndex, newBaseIndex, newUpIndex);
                currLeftIndex = newBaseIndex;
                currUpIndex = newUpIndex;
                d = length*normalize(cross(d, o));
            }

            normalAndPushIDs(rightBaseID, currLeftIndex, currUpIndex);
            normalAndPushIDs(currUpIndex, rightBaseID, rightUpID);
       }


       /**
        * @brief popDirection, pop the stack of direction and returns its last element
        * @return Last element of direction after a pop
        */
       vec3 popDirection(){
           direction.pop_back();
           return direction.back();
       }

       /**
        * @brief popLeftPoint, pop the stack of leftPoint and returns its last element
        * @return Last element of leftPoint after a pop
        */
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

       /**
        * @brief updateTriangleIndicesAndIndexes, updates the vertices and indices with a new point
        * @param newPoint, the up point
        * @param dlid, down left point-id
        * @param drid, down right point-id
        * @param upid, up point id
        */
       void updateTriangleIndicesAndIndexes(vec3 newPoint, int dlid, int drid, int upid){
           pushToVertices(newPoint);
           normalAndPushIDs(dlid, drid, upid);
       }

       /**
        * @brief updateIndicesAndIndexes
        * @param ulpoint
        * @param urpoint
        * @param dlid
        * @param drid
        * @param ulid
        * @param urid
        */
       void updateIndicesAndIndexes(vec3 ulpoint, vec3 urpoint,
                                    int dlid, int drid, int ulid, int urid){
               pushToVertices(ulpoint);
               pushToVertices(urpoint);
               normalAndPushIDs(dlid,drid,ulid);
               normalAndPushIDs(ulid, drid, urid);
       }


       /**
        * @brief generateAlgae
        * The method parses the tree that is a string.
        * It is purely iterative, and manages a stack to keep the states of the branches from one iteration to another.
        * Its role is to update the indices and vertices vectors, to create the algae.
        */
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
           for(size_t i = 0; i < turtle.treeLength(); i++){
               char str = turtle.charAt(i);
               if(str == ']'){
                   dir0 = popDirection();
                   leftp0 = popLeftPoint();
                   rightp0 = popRightPoint();
                   lefti0 = popLeftIndex();
                   righti0 = popRightIndex();
                   branches.pop_back();
               } else if(str == 'A' || str == 'B'){
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
                            createCubeVolume(leftp1, rightp1, leftp0, lefti1, righti1, lefti0, righti0);

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
                            createCubeVolume(leftp1, rightp1, leftp0, lefti1, righti1, lefti0, righti0);
                        }

                    }
               } else if(str == '['){
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

       vec3 getPointFromID(int id){
           return vec3(vertices.at(id*3), vertices.at(id*3+1), vertices.at(id*3+2));
       }

       void createNormals(){
           for(size_t vertex = 0; vertex < vertices.size()/3; ++vertex){
               vector<vec3> vertexNormal; // accumulates all the normals for one vertex
               for(size_t triangle = 0; triangle < indices.size(); triangle+=3){
                   int i1 = indices.at(triangle);
                   int i2 = indices.at(triangle +1);
                   int i3 = indices.at(triangle +2);
                   if(vertex == i1 || vertex == i2 || vertex == i3){
                       // bingo! compute the normal of this triangle and add
                       // it to the array of normals for this vertex
                       vec3 p1 = getPointFromID(i1);
                       vec3 p2 = getPointFromID(i2);
                       vec3 p3 = getPointFromID(i3);
                       vec3 n = cross(p1-p3, p1-p2);
                       vertexNormal.push_back(n);
                   }
               }

               // Once all normals have been computed, we will simply sum
               // them up for now and normalize. Note that the correct approach
               // would be by computing their respective angles first and then
               // using a weighted sum.
               vec3 normalf = vec3(0.0f,0.0f,0.0f);
               for(size_t normal = 0; normal < vertexNormal.size(); ++normal){
                   normalf += vertexNormal.at(normal);
               }
               normalf = normalize(normalf);
               triangleNormals.push_back(normalf);// by hypothesis the normals should be pushed in the same order as the points
               vertexNormal.clear();
           }
       }


       void Draw(const glm::mat4 &model = IDENTITY_MATRIX,
                 const glm::mat4 &view = IDENTITY_MATRIX,
                 const glm::mat4 &projection = IDENTITY_MATRIX){
           grid.Draw(model, view, projection);
       }

       void Cleanup(){
           leftPoint.clear();
           turtle.Cleanup();
           rightPoint.clear();
           leftIndex.clear();
           rightIndex.clear();
           direction.clear();
           branches.clear();
           grid.Cleanup();
           indices.clear();
           vertices.clear();
           triangleNormals.clear();

       }

};
