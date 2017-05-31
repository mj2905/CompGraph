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
    vector<GLuint> leftIndex, rightIndex, indices, bLeftIndex, bRightIndex;
    vector<vec3> leftPoint, rightPoint, direction,triangleNormals, bLeftPoint, bRightPoint;
    vector<GLfloat> vertices_, normals;
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
    void Init(GLuint depth, char axiom, vec3 origin, GLuint texture_id, int index){
        generator.Seed(1);
        rand = generator.rand();

        this->index_ = index;


        vec3 originLeft = vec3(origin.x - init_width, origin.y, origin.z);
        vec3 originRight = vec3(origin.x + init_width, origin.y, origin.z);
        vec3 originBLeft = vec3(origin.x - init_width, origin.y, origin.z - init_width);
        vec3 originBRight = vec3(origin.x + init_width, origin.y, origin.z - init_width);

        leftPoint.push_back(originLeft);
        rightPoint.push_back(originRight);
        bLeftPoint.push_back(originBLeft);
        bRightPoint.push_back(originBRight);

        pushToVertices(originLeft);
        pushToVertices(originBLeft);
        pushToVertices(originBRight);
        pushToVertices(originRight);

        direction.push_back(vec3(0.0f,1.0f,0.0f));
        leftIndex.push_back(index_);
        bLeftIndex.push_back(++index_);
        bRightIndex.push_back(++index_);
        rightIndex.push_back(++index_);


        turtle.Init(depth, axiom);
        generateAlgae();

        createNormals();

        if(3*triangleNormals.size()!= vertices_.size()){
            cout << "Sizes different. Normals should be: " << vertices_.size() << " but was " << triangleNormals.size() << endl;
        }

        vector<GLfloat> n;
        for(size_t i =0; i < triangleNormals.size(); ++i){
            n.push_back(triangleNormals.at(i).x);
            n.push_back(triangleNormals.at(i).y);
            n.push_back(triangleNormals.at(i).z);
        }

        grid.Init(vertices_, indices, n, texture_id);

    }

    /**
        * @brief pushToVertices
        * @param point
        */
    void pushToVertices(vec3 point){
        vertices_.push_back(point.x);
        vertices_.push_back(point.y);
        vertices_.push_back(point.z);
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
        * @brief updateIndicesAndIndexes
        * @param ulpoint
        * @param urpoint
        * @param dlid
        * @param drid
        * @param ulid
        * @param urid
        */
    void updateIndicesAndIndexes(int dlid, int drid, int ulid, int urid){
        normalAndPushIDs(dlid,drid,ulid);
        normalAndPushIDs(ulid, drid, urid);
    }

    void customCreateCubeVolume(vec3 leftp1, vec3 rightp1, vec3 leftp0, vec3 rightp0,
                                vec3 bleftp1,vec3  brightp1,vec3  bleftp0, vec3 brightp0,
                                int lefti1,int  righti1,int  lefti0,int  righti0,
                                int blefti1,int  brighti1,int  blefti0,int  brighti0){

        pushToVertices(leftp0);
        pushToVertices(bleftp0);
        pushToVertices(brightp0);
        pushToVertices(rightp0);
        updateIndicesAndIndexes(lefti1, righti1, lefti0, righti0);
        updateIndicesAndIndexes(blefti1, lefti1, blefti0, lefti0);
        updateIndicesAndIndexes(brighti1, blefti1, brighti0, blefti0);
        updateIndicesAndIndexes(righti1, brighti1, righti0, brighti0);
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


    vec3 popBLeftPoint(){
        bLeftPoint.pop_back();
        return bLeftPoint.back();
    }

    vec3 popRightPoint(){
        rightPoint.pop_back();
        return rightPoint.back();
    }

    vec3 popBRightPoint(){
        bRightPoint.pop_back();
        return bRightPoint.back();
    }

    int popLeftIndex(){
        leftIndex.pop_back();
        return leftIndex.back();
    }


    int popBLeftIndex(){
        bLeftIndex.pop_back();
        return bLeftIndex.back();
    }

    int popRightIndex(){
        rightIndex.pop_back();
        return rightIndex.back();
    }


    int popBRightIndex(){
        bRightIndex.pop_back();
        return bRightIndex.back();
    }

    /**
        * @brief updateTriangleIndicesAndIndexes, updates the vertices and indices with a new point
        * @param newPoint, the up point
        * @param dlid, down left point-id
        * @param drid, down right point-id
        * @param upid, up point id
        */
    void updateTriangleIndicesAndIndexes(int dlid, int drid, int upid){
        normalAndPushIDs(dlid, drid, upid);
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
                dir0 = popDirection();

                leftp0 = popLeftPoint();
                bleftp0 = popBLeftPoint();
                rightp0 = popRightPoint();
                brightp0 = popBRightPoint();
                lefti0 = popLeftIndex();
                blefti0 = popBLeftIndex();
                righti0 = popRightIndex();
                brighti0 = popBRightIndex();

                branches.pop_back();
            } else if(str == 'A' || str == 'B'){
                lo = str;
                if(direction.size() > 0 && branches.size() > 0 && leftPoint.size() > 0 && rightPoint.size() > 0 && leftIndex.size()>0 && rightIndex.size()>0){
                    l1 = branches.back();

                    dir1 = popDirection();
                    dir0 = updateDirection(dir1, l1, lo);

                    direction.push_back(dir0);
                    leftp1 = popLeftPoint();
                    bleftp1 = popBLeftPoint();
                    rightp1 =  popRightPoint();
                    brightp1 =  popBRightPoint();
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

                        lefti1 = popLeftIndex();
                        lefti0 = ++index_ ;
                        leftIndex.push_back(lefti0);
                        blefti1 = popBLeftIndex();
                        blefti0 = ++index_ ;
                        bLeftIndex.push_back(blefti0);

                        brighti1 = popBRightIndex();
                        brighti0 = ++index_ ;
                        bRightIndex.push_back(brighti0);
                        righti1 = popRightIndex();
                        righti0 = ++index_ ;
                        rightIndex.push_back(righti0);

                        pushToVertices(leftp0);
                        pushToVertices(bleftp0);
                        pushToVertices(brightp0);
                        pushToVertices(rightp0);
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
                        lefti1 = popLeftIndex();
                        blefti1 = popBLeftIndex();
                        righti1 = popRightIndex();
                        brighti1 = popBRightIndex();


                        lefti0 = ++ index_;
                        blefti0 = ++ index_;
                        brighti0 = ++ index_;
                        righti0 = ++ index_;

                        leftIndex.push_back(lefti0);
                        bLeftIndex.push_back(blefti0);

                        rightIndex.push_back(righti0);
                        bRightIndex.push_back(brighti0);

                        leftp0 = updatePoint(dir0, newLeftPoint);
                        bleftp0 = updatePoint(dir0, newBLeftPoint);
                        rightp0 = updatePoint(dir0, rightp1);
                        brightp0 = updatePoint(dir0, brightp1);
                        leftPoint.push_back(leftp0);
                        bLeftPoint.push_back(bleftp0);
                        rightPoint.push_back(rightp0);
                        bRightPoint.push_back(brightp0);

                        customCreateCubeVolume(newLeftPoint, rightp1, leftp0, rightp0,
                                               newBLeftPoint, brightp1, bleftp0, brightp0,
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
                        lefti1 = popLeftIndex();
                        blefti1 = popBLeftIndex();
                        righti1 = popRightIndex();
                        brighti1 = popBRightIndex();

                        lefti0 = ++ index_;
                        blefti0 = ++ index_;
                        brighti0 = ++ index_;
                        righti0 = ++ index_;

                        leftIndex.push_back(lefti0);
                        bLeftIndex.push_back(blefti0);

                        rightIndex.push_back(righti0);
                        bRightIndex.push_back(brighti0);

                        leftp0 = updatePoint(dir0, leftp1);
                        bleftp0 = updatePoint(dir0, newBLeftPoint);
                        rightp0 = updatePoint(dir0, rightp1);
                        brightp0 = updatePoint(dir0, newBRightPoint);
                        leftPoint.push_back(leftp0);
                        bLeftPoint.push_back(bleftp0);
                        rightPoint.push_back(rightp0);
                        bRightPoint.push_back(brightp0);

                        customCreateCubeVolume(leftp1, rightp1, leftp0, rightp0,
                                               newBLeftPoint, newBRightPoint, bleftp0, brightp0,
                                               lefti1, righti1, lefti0, righti0,
                                               bLeftId, bRightId, blefti0, brighti0);


                        updateTriangleIndicesAndIndexes(blefti1, lefti1, bLeftId);
                        updateTriangleIndicesAndIndexes(brighti1, righti1, bRightId);
                        updateIndicesAndIndexes(brighti1, blefti1, bLeftId, bRightId);
                    }

                }
            } else if(str == '['){
                direction.push_back(dir0);
                leftPoint.push_back(leftp0);
                bLeftPoint.push_back(bleftp0);
                rightPoint.push_back(rightp0);
                bRightPoint.push_back(brightp0);
                leftIndex.push_back(lefti0);
                bLeftIndex.push_back(blefti0);
                rightIndex.push_back(righti0);
                bRightIndex.push_back(brighti0);
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

    vec3 getPointFromID(int id){
        return vec3(vertices_.at(id*3), vertices_.at(id*3+1), vertices_.at(id*3+2));
    }

    void createNormals(){
        for(size_t vertex = 0; vertex < vertices_.size()/3; ++vertex){
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
            float total = 0.0f;
            float angle = 0.0f;
            for(size_t normal = 0; normal < vertexNormal.size(); ++normal){
                for(size_t k = normal+1; k < vertexNormal.size() && k != normal; ++k ){
                    vec3 v1 = vertexNormal.at(normal);
                    vec3 v2 = vertexNormal.at(k);
                    angle = acos(dot(v1, v2));
                    normalf += angle*(v1+v2);
                    total += angle;

                }
            }
            normalf = normalf/total;
            triangleNormals.push_back(normalf);// by hypothesis the normals should be pushed in the same order as the points
            vertexNormal.clear();
        }
    }


    void Draw(const glm::mat4 &model = IDENTITY_MATRIX,
              const glm::mat4 &view = IDENTITY_MATRIX,
              const glm::mat4 &projection = IDENTITY_MATRIX){
        grid.Draw(model, view, projection);
    }

    vector<GLuint> getIndicesArray(){
        return indices;
    }

    int getIndex(){
        return index_;
    }

    vector<GLfloat> getVertices(){
        return vertices_;
    }

    vector<GLfloat> getNormals(){
        return normals;
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
        vertices_.clear();
        triangleNormals.clear();

    }

};
