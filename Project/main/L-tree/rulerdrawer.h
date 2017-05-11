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
#include "stack.h"
#include "plant.h"

using namespace glm;

template <class Plant> class RulerDrawer {
private:
    Flexigrid grid;
    Turtle turtle;
    Stack<GLuint> leftIndex, rightIndex, indices, bLeftIndex, bRightIndex;
    Stack<vec3> leftPoint, rightPoint, direction, triangleNormals, bLeftPoint, bRightPoint;
    Stack<GLfloat> vertices, normals;
    Stack<char> branches;
    int index_ = 0;
    float init_width = 0.08f;
    float init_length = 5*init_width;

public:

    void pushAsSquare(vec3 oL, vec3 oR, vec3 oBL, vec3 oBR){
        pushToVertices(oL);
        pushToVertices(oBL);
        pushToVertices(oBR);
        pushToVertices(oR);
    }

    void pushSquareStack(vec3 l, vec3 r, vec3 bl, vec3 br){
        leftPoint.push_back(l);
        rightPoint.push_back(r);
        bLeftPoint.push_back(bl);
        bRightPoint.push_back(br);
    }

    void pushSquareIndex(int il, int ir, int ibl, int ibr){
        leftIndex.push_back(il);
        bLeftIndex.push_back(ir);
        bRightIndex.push_back(ibl);
        rightIndex.push_back(ibr);
    }

    void Init(vec3 origin){
        vec3 originLeft = vec3(origin.x - init_width, origin.y, origin.z);
        vec3 originRight = vec3(origin.x + init_width, origin.y, origin.z);
        vec3 originBLeft = vec3(origin.x - init_width, origin.y, origin.z - init_width);
        vec3 originBRight = vec3(origin.x + init_width, origin.y, origin.z - init_width);

        pushSquareStack(originLeft, originRight, originBLeft, originBRight);

        pushAsSquare(originLeft,originRight, originBLeft, originBRight);

        direction.push_back(vec3(0.0f,1.0f,0.0f));

        pushSquareIndex(index_, ++index_, ++index_, ++index_);

        /*turtle.Init(depth, axiom);
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
        grid.Init(vertices, indices, n, texture_id);*/
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

    // CHANGE ACCORDING TO THE RULE OF THE CLASS

    vec3 updateDirection(vec3 direction, char parentType, char targetType){
        /*vec3 rotation, newDir;
        rand = generator.rand();

        if(parentType == 'A'){
            if(targetType == 'B'){
                if(rand < 50){
                    rotation = vec3(5.0f,50.0f,25.0f);
                }else{
                    rotation = vec3(5.4f,-50.0f, 20.0f);
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
        return newDir;*/
        return vec3(0.0f,0.0f,0.0f);
    }

    vec3 updatePoint(vec3 direction, vec3 originalPoint){
        return direction+originalPoint;
    }


    void pushTriIDs(int id1, int id2, int id3){
        indices.push_back(id1);
        indices.push_back(id2);
        indices.push_back(id3);
    }

    void printTurtle(){
        turtle.printTree();
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
    void updateIndicesAndIndexes(int dlid, int drid, int ulid, int urid){
        pushTriIDs(dlid,drid,ulid);
        pushTriIDs(ulid, drid, urid);
    }

    void createBranch(vec3 leftp0, vec3 rightp0, vec3  bleftp0, vec3 brightp0,
                      int lefti1,int  righti1,int  lefti0,int  righti0,
                      int blefti1,int  brighti1,int  blefti0,int  brighti0){

        pushSquareStack(leftp0,rightp0,bleftp0, brightp0);
        updateIndicesAndIndexes(lefti1, righti1, lefti0, righti0);
        updateIndicesAndIndexes(blefti1, lefti1, blefti0, lefti0);
        updateIndicesAndIndexes(brighti1, blefti1, brighti0, blefti0);
        updateIndicesAndIndexes(righti1, brighti1, righti0, brighti0);
    }

    /**
        * @brief updateTriangleIndicesAndIndexes, updates the vertices and indices with a new point
        * @param newPoint, the up point
        * @param dlid, down left point-id
        * @param drid, down right point-id
        * @param upid, up point id
        */
    void updateTriangleIndicesAndIndexes(int dlid, int drid, int upid){
        pushTriIDs(dlid, drid, upid);
    }

    vec3 getPointFromID(int id){
        return vec3(vertices.at(id*3), vertices.at(id*3+1), vertices.at(id*3+2));
    }

    void assignIndexAndPoint(class B *i0,class B *i1, Stack<class B> *s){
        (*i1) = (*s).popStack();
        (*i0) = ++index_;
        (*s).push_back((*i0));
    }

    void generateAlgae(){
        vec3 dir0,dir1;
        dir0 = direction.back();

        vec3 leftp0, leftp1, bleftp0, bleftp1;
        leftp0 = leftPoint.back();
        bleftp0 = bLeftPoint.back();
        vec3 rightp0, rightp1, brightp0, brightp1;
        rightp0 = rightPoint.back();
        brightp0 = bRightPoint.back();

        int lefti0, lefti1, blefti0, blefti1;
        lefti0 = leftIndex.back();
        blefti0 = bLeftIndex.back();

        int righti0, righti1, brighti0, brighti1;
        righti0 = rightIndex.back();
        brighti0 = bRightIndex.back();

        char lo = 'n';
        char l1 = 'n';
        for(size_t i = 0; i < turtle.treeLength(); i++){
            char str = turtle.charAt(i);
            if(str == ']'){
                dir0 = direction.popStack();

                leftp0 = leftPoint.popStack();
                bleftp0 = bLeftPoint.popStack();
                rightp0 = rightPoint.popStack();
                brightp0 = bRightPoint.popStack();
                lefti0 = leftIndex.popStack();
                blefti0 = bLeftIndex.popStack();
                righti0 = rightIndex.popStack();
                brighti0 = bRightIndex.popStack();

                branches.popStack();
            } else if(str == 'A' || str == 'B'){
                lo = str;
                if(direction.size() > 0 && branches.size() > 0 && leftPoint.size() > 0 && rightPoint.size() > 0 && leftIndex.size()>0 && rightIndex.size()>0){
                    l1 = branches.back();

                    dir1 = direction.popStack();
                    dir0 = updateDirection(dir1, l1, lo);

                    direction.push_back(dir0);
                    leftp1 = leftPoint.popStack();
                    bleftp1 = bLeftPoint.popStack();
                    rightp1 = rightPoint.popStack();
                    brightp1 = bRightPoint.popStack();
                    if(!smallerThanWidth(dir0, leftp1, rightp1) &&
                            !smallerThanWidth(dir0, bleftp1, brightp1)){
                        leftp0 = updatePoint(dir0, leftp1);
                        leftPoint.push_back(leftp0);
                        bleftp0 = updatePoint(dir0, bleftp1);
                        bLeftPoint.push_back(bleftp0);

                        rightp0 = updatePoint(dir0, rightp1);
                        rightPoint.push_back(rightp0);
                        brightp0 = updatePoint(dir0, brightp1);
                        bRightPoint.push_back(brightp0);


                        assignIndexAndPoint(&lefti0,&lefti1, &leftIndex);
                        assignIndexAndPoint(&blefti0, &blefti1, &bLeftIndex);
                        assignIndexAndPoint(&brighti0, &brighti1, &bRightIndex);
                        assignIndexAndPoint(&righti0, &righti1, &rightIndex);

                        pushAsSquare(leftp0,rightp0, bleftp0, brightp0);
                        updateIndicesAndIndexes(lefti1, righti1, lefti0, righti0);
                        updateIndicesAndIndexes(blefti1, lefti1, blefti0, lefti0);
                        updateIndicesAndIndexes(brighti1, blefti1, brighti0, blefti0);
                        updateIndicesAndIndexes(righti1, brighti1, righti0, brighti0);


                    }else if(smallerThanWidth(dir0, leftp1, rightp1)) {
                        vec3 dirbase = leftp1 - rightp1;
                        vec3 axis = normalize(cross(dir0, dirbase));
                        vec3 transdir = init_width*normalize(cross(axis,dir0));
                        vec3 newLeftPoint = updatePoint(rightp1,transdir);
                        vec3 newBLeftPoint = updatePoint(brightp1, transdir);
                        int leftId = ++index_;
                        pushToVertices(newLeftPoint);
                        int bLeftId = ++index_;
                        pushToVertices(newBLeftPoint);
                        lefti1 = leftIndex.popStack();
                        blefti1 = bLeftIndex.popStack();
                        righti1 = rightIndex.popStack();
                        brighti1 = bRightIndex.popStack();


                        lefti0 = ++ index_;
                        blefti0 = ++ index_;
                        brighti0 = ++ index_;
                        righti0 = ++ index_;

                        pushSquareIndex(leftIndex, rightIndex, bLeftIndex, bRightIndex);

                        leftp0 = updatePoint(dir0, newLeftPoint);
                        bleftp0 = updatePoint(dir0, newBLeftPoint);
                        rightp0 = updatePoint(dir0, rightp1);
                        brightp0 = updatePoint(dir0, brightp1);

                        pushSquareStack(leftp0, rightp0, bleftp0, brightp0);


                        createBranch(leftp0, rightp0,bleftp0, brightp0,
                                     leftId, righti1, lefti0, righti0,
                                     bLeftId, brighti1, blefti0, brighti0);


                        updateTriangleIndicesAndIndexes(lefti1, righti1, leftId);
                        updateTriangleIndicesAndIndexes(blefti1, brighti1, bLeftId);
                        updateIndicesAndIndexes(blefti1, lefti1, bLeftId, leftId);

                    }else if(smallerThanWidth(dir0, leftp1, bleftp1)){
                        // this time is left and bleft
                        vec3 dirbase = bleftp1 - leftp1;
                        vec3 axis = normalize(cross(dir0, dirbase));
                        vec3 transdir = init_width*normalize(cross(axis,dir0));
                        vec3 newBLeftPoint = updatePoint(leftp1,transdir);
                        vec3 newBRightPoint = updatePoint(rightp1, transdir);
                        int bLeftId = ++index_;
                        pushToVertices(newBLeftPoint);
                        int bRightId = ++index_;
                        pushToVertices(newBRightPoint);
                        lefti1 = leftIndex.popStack();
                        blefti1 = bLeftIndex.popStack();
                        righti1 = rightIndex.popStack();
                        brighti1 = bRightIndex.popStack();

                        lefti0 = ++ index_;
                        blefti0 = ++ index_;
                        brighti0 = ++ index_;
                        righti0 = ++ index_;

                        pushSquareIndex(lefti0,righti0, blefti0, brighti0);

                        leftp0 = updatePoint(dir0, leftp1);
                        bleftp0 = updatePoint(dir0, newBLeftPoint);
                        rightp0 = updatePoint(dir0, rightp1);
                        brightp0 = updatePoint(dir0, newBRightPoint);

                        pushSquareStack(leftp0, rightp0, bleftp0, brightp0);

                        createBranch(leftp0, rightp0,bleftp0, brightp0,
                                     lefti1, righti1, lefti0, righti0,
                                     bLeftId, bRightId, blefti0, brighti0);


                        updateTriangleIndicesAndIndexes(blefti1, lefti1, bLeftId);
                        updateTriangleIndicesAndIndexes(brighti1, righti1, bRightId);
                        updateIndicesAndIndexes(brighti1, blefti1, bLeftId, bRightId);
                    }

                }
            } else if(str == '['){
                direction.push_back(dir0);
                pushSquareStack(leftp0, rightp0, bleftp0, brightp0);
                pushSquareIndex(lefti0, righti0, blefti0, brighti0);
                branches.push_back(lo);
            }
        }

        leftPoint.clear();
        bLeftPoint.clear();
        rightPoint.clear();
        bRightPoint.clear();
        leftIndex.clear();
        bLeftIndex.clear();
        rightIndex.clear();
        bRightIndex.clear();
        direction.clear();
        branches.clear();

    }


};
