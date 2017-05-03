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
    vector<GLuint> leftIndex, rightIndex, indices, botLeftIndex, botRightIndex;
    vector<vec3> leftPoint, rightPoint, direction,triangleNormals, botleftPoint, botRightPoint;
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
        vec3 originBotLeft = vec3(origin.x - init_width, origin.y, origin.z - init_width);
        vec3 originBotRight = vec3(origin.x + init_width, origin.y, origin.z - init_width);

        leftPoint.push_back(originLeft);
        rightPoint.push_back(originRight);
        botleftPoint.push_back(originBotLeft);
        botRightPoint.push_back(originBotRight);

        pushToVertices(originLeft);
        pushToVertices(originBotLeft);
        pushToVertices(originBotRight);
        pushToVertices(originRight);


        // left, bleft, bright, right
        direction.push_back(vec3(0.0f,1.0f,0.0f));
        leftIndex.push_back(index_);
        botLeftIndex.push_back(++index_);
        botRightIndex.push_back(++index_);
        rightIndex.push_back(++index_);


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


    vec3 popVec3(vector<vec3> *v){
        (*v).pop_back();
        return (*v).back();
    }


    int popVec3Int(vector<GLuint> *v){
        (*v).pop_back();
        return (*v).back();
    }


    vec3 popDirection(){
        return popVec3(&direction);
    }

    vec3 popLeftPoint(){
        return popVec3(&leftPoint);
    }

    vec3 popBotLeftPoint(){
        return popVec3(&botleftPoint);
    }

    vec3 popRightPoint(){
        return popVec3(&rightPoint);
    }

    vec3 popBotRightPoint(){
        return popVec3(&botRightPoint);
    }

    int popLeftIndex(){
        return popVec3Int(&leftIndex);
    }

    int popBotLeftIndex(){
        return popVec3Int(&botLeftIndex);
    }

    int popRightIndex(){
        return popVec3Int(&rightIndex);
    }
    int popBotRightIndex(){
        return popVec3Int(&botRightIndex);
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


    /*       void createCubeVolume(vec3 leftBasePoint, vec3 rightBasePoint, vec3 leftUpPoint,
                             int leftBaseID, int rightBaseID, int leftUpID, int rightUpID){

            vec3 b, d, o, newBasePoint, newUpPoint;
            int newBaseIndex, newUpIndex, currLeftIndex, currUpIndex;
            float length = init_width;//turtle.norm(leftBasePoint, rightBasePoint);

            b = leftBasePoint - rightBasePoint;
            newBasePoint = leftBasePoint;
            newUpPoint = leftUpPoint;


            o = leftUpPoint - leftBasePoint;
            d = length*normalize(cross(b, o));

            currLeftIndex = leftBaseID;
            currUpIndex = leftUpID;

            vec3 bp1, bp2;
            int bp1i, bp2i;

            {

            newBasePoint = newBasePoint+d;
            newUpPoint = newUpPoint +d;
            newBaseIndex = ++index_ ;
            newUpIndex = ++index_ ;
            bp1i = newBaseIndex;
            pushToVertices(newBasePoint);
            bp1 = newBasePoint;
            pushToVertices(newUpPoint);
            normalAndPushIDs(newBaseIndex, currLeftIndex, currUpIndex);
            normalAndPushIDs(currUpIndex, newBaseIndex, newUpIndex);
            currLeftIndex = newBaseIndex;
            currUpIndex = newUpIndex;
            d = length*normalize(cross(d, o));

            newBasePoint = newBasePoint+d;
            newUpPoint = newUpPoint +d;
            newBaseIndex = ++index_ ;
            newUpIndex = ++index_ ;
            pushToVertices(newBasePoint);
            bp2 = newBasePoint;
            pushToVertices(newUpPoint);
            bp2i = newBaseIndex;
            normalAndPushIDs(newBaseIndex, currLeftIndex, currUpIndex);
            normalAndPushIDs(currUpIndex, newBaseIndex, newUpIndex);
            currLeftIndex = newBaseIndex;
            currUpIndex = newUpIndex;
            d = length*normalize(cross(d, o));

            }

            normalAndPushIDs(rightBaseID, currLeftIndex, currUpIndex);
            normalAndPushIDs(currUpIndex, rightBaseID, rightUpID);

            vec3 abp1 = botleftPoint.back();
            vec3 abp2 = botRightPoint.back();
            if(!areConnected(&abp1, &bp1) && !areConnected(&abp2,&bp2)){
                updateTriangleIndicesAndIndexes(botLeftIndex.back(),leftBaseID,bp1i);
                updateIndicesAndIndexes(botRightIndex.back(), botLeftIndex.back(), bp2i, bp1i);
                updateTriangleIndicesAndIndexes(rightBaseID, botRightIndex.back(), bp2i);
            }
            if(!areConnected(&abp2, &bp2)){

            }

            botleftPoint.push_back(abp1);
            botRightPoint.push_back(abp2);
            botRightIndex.push_back(bp2i);
            botLeftIndex.push_back(bp1i);
       }*/

    float norm(vec3 *v){
        return sqrt(((*v).x*(*v).x)+((*v).y*(*v).y) + ((*v).z*(*v).z));
    }

    float scalarProjection(vec3 *a, vec3 *b){
        return (dot((*a), normalize((*b))));
    }

    bool projIsSmallerThan(vec3 *a, vec3 *b){
        if(dot((*a), (*b)) < 0){
            vec3 c = -(*b);
            return scalarProjection(a, &c) < norm(b);
        }
        return scalarProjection(a,b) < norm(b);
    }

    int compareProjPoints(vec3 *toCompare, vec3 *p1, vec3 *p2, vec3 *p3, vec3 *p4){
        vec3 p1p2 = (*p1) - (*p2);
        vec3 p1p3 = (*p1) - (*p3);
        vec3 p1p4 = (*p1) - (*p4);
        vec3 pap2 = (*toCompare) - (*p2);
        vec3 pap3 = (*toCompare) - (*p3);
        vec3 pap4 = (*toCompare) - (*p4);
        int count(0);
        if(projIsSmallerThan(&pap2, &p1p2)) ++count;
        if(projIsSmallerThan(&pap3,&p1p3)) ++count;
        if(projIsSmallerThan(&pap4, &p1p4)) ++count;

        return count;
    }

    void newPointFromDir(vec3 *from, vec3 *to, vec3 *direction, vec3 *res){
        vec3 v = (*to) - (*from);
        float projAxis = abs(scalarProjection(&v, direction));
        vec3 projVector = projAxis*normalize((*direction));
        vec3 dirVector = v + projVector;
        vec3 trans = init_width*normalize(dirVector);
        (*res) = (*from) + trans;
    }
/*
    void someMethod(vec3 *p1, vec3 *p2, vec3 *p3, vec3 *p4,
                    int *i1, int *i2, int *i3, int *i4,
                    vec3 pa, vec3 *direction,
                    vec3 *pb, vec3 *pc, vec3 *pd){
        vec3 pbBase2 = vec3(0.0,0.0,0.0);
        newPointFromDir(p1,p2,direction, &pbBase2);
        int pbBase2Id = ++ index_;
        pushToVertices(pbBase2);
        updateTriangleIndicesAndIndexes((*i2),(*i1),pbBase2Id);

        vec3 pbBase4 = vec3(0.0,0.0,0.0);
        newPointFromDir(p1,p4,direction, &pbBase4);
        int pbBase4Id = ++ index_;
        pushToVertices(pbBase4);
        updateTriangleIndicesAndIndexes((*i1),(*i4),pbBase4Id);

        vec3 pbBase3 = vec3(0.0,0.0,0.0);
        newPointFromDir(p1,p3,direction, &pbBase3);
        int pbBase3Id = ++ index_;
        pushToVertices(pbBase3);
        updateIndicesAndIndexes((*i3),(*i2),pbBase3Id,pbBase2Id);
        updateIndicesAndIndexes((*i4),(*i3),pbBase4Id,pbBase3Id);

        int pai = ++ index_;
        int pbi = ++index_;
        int pci= ++index_;
        int pdi = ++index_;

        leftIndex.push_back(pai);
        botLeftIndex.push_back(pbi);
        botRightIndex.push_back(pci);
        rightIndex.push_back(pdi);

        (*pb) = pbBase2 + (*direction);
        (*pc) = pbBase3 + (*direction);
        (*pd) = pbBase4 + (*direction);

        pushToVertices(pa);
        pushToVertices((*pb));
        pushToVertices((*pc));
        pushToVertices((*pd));

        leftPoint.push_back(pa);
        botleftPoint.push_back((*pb));
        botRightPoint.push_back((*pc));
        rightPoint.push_back((*pd));

        updateIndicesAndIndexes((*i1),pbBase4Id,pai,pdi);
        updateIndicesAndIndexes(pbBase2Id,(*i1),pbi, pai);
        updateIndicesAndIndexes(pbBase3Id, pbBase2Id,pci, pbi);
        updateIndicesAndIndexes(pbBase4Id, pbBase3Id, pdi, pci);

    }

    void someMethod2(vec3 *p1, vec3 *p2, vec3 *p3, vec3 *p4,
                     int *i1, int *i2, int *i3, int *i4,
                     vec3 pa, vec3 pd, vec3 *direction,
                     vec3 *pb, vec3 *pc){
        vec3 pbBase2 = vec3(0.0,0.0,0.0);
        newPointFromDir(p1,p2,direction, &pbBase2);
        int pbBase2Id = ++ index_;
        pushToVertices(pbBase2);
        updateTriangleIndicesAndIndexes((*i2),(*i1),pbBase2Id);


        vec3 pbBase3 = vec3(0.0,0.0,0.0);
        newPointFromDir(p1,p3,direction, &pbBase3);
        int pbBase3Id = ++ index_;
        pushToVertices(pbBase3);
        updateTriangleIndicesAndIndexes((*i4),(*i3), pbBase3Id);
        updateIndicesAndIndexes((*i3),(*i2),pbBase3Id,pbBase2Id);


        int pai = ++ index_;
        int pbi = ++index_;
        int pci= ++index_;
        int pdi = ++index_;

        (*pb) = pbBase2 + (*direction);
        (*pc) = pbBase3 + (*direction);

        leftIndex.push_back(pai);
        botLeftIndex.push_back(pbi);
        botRightIndex.push_back(pci);
        rightIndex.push_back(pdi);


        pushToVertices(pa);
        pushToVertices((*pb));
        pushToVertices((*pc));
        pushToVertices(pd);

        leftPoint.push_back(pa);
        botleftPoint.push_back((*pb));
        botRightPoint.push_back((*pc));
        rightPoint.push_back(pd);


        updateIndicesAndIndexes((*i1), (*i4),pai, pdi);
        updateIndicesAndIndexes(pbBase2Id,(*i1),pbi, pai);
        updateIndicesAndIndexes(pbBase3Id, pbBase2Id,pci, pbi);
        updateIndicesAndIndexes((*i4),pbBase3Id, pdi, pci);

    }

*/

    void drawNiceCube(vec3 *p1, vec3 *p2, vec3 *p3, vec3 *p4,
                      int i1, int i2, int i3, int i4, vec3 *direction){

        int pai = ++ index_;
        int pbi = ++index_;
        int pci= ++index_;
        int pdi = ++index_;

        leftIndex.push_back(pai);
        botLeftIndex.push_back(pbi);
        botRightIndex.push_back(pci);
        rightIndex.push_back(pdi);

        vec3 a = (*p1) + (*direction);
        vec3 b = (*p2) + (*direction);
        vec3 c = (*p3) + (*direction);
        vec3 d = (*p4) + (*direction);

        pushToVertices(a);
        pushToVertices(b);
        pushToVertices(c);
        pushToVertices(d);

        leftPoint.push_back(a);
        botleftPoint.push_back(b);
        botRightPoint.push_back(c);
        rightPoint.push_back(d);

        updateIndicesAndIndexes(i1,i4,pai, pdi);
        updateIndicesAndIndexes(i2,i1, pbi, pai);
        updateIndicesAndIndexes(i3,i2, pci, pbi);
        updateIndicesAndIndexes(i4,i3,pdi,pci);
    }



    // left, botleft, botright, right
    void createBranch(vec3 *p1, vec3 *p2, vec3 *p3, vec3 *p4,
                      int *i1, int *i2, int *i3, int *i4, vec3 *direction){
        // Here will be the main meat

        vec3 fromp1top2 = (*p2) - (*p1);
        vec3 fromp1top4 = (*p4) - (*p1);
        float dot1 = dot(fromp1top2, (*direction));
        float dot2 = dot(fromp1top4, (*direction));
        if(dot1 == 0 && dot2 == 0){
            // neutral / neutral = no more construct
            // we can directly create the next branch!
            drawNiceCube(p1,p2,p3,p4,(*i1),(*i2),(*i3),(*i4),direction);
        }else if(dot1 < 0 && dot2 == 0){
            // interior / neutral
            // we must create the vector for p2
            // the use it to create the new p3
            // draw the resulting triangles and square
            // finally the cube with those new values
            vec3 altp2 = vec3(0.0,0.0,0.0);
            newPointFromDir(p1,p2,direction, &altp2); // creates the new p2
            vec3 trans = altp2 - (*p1);
            vec3 altp3 = (*p4) + trans;

            int indaltp2 = ++index_;
            int indaltp3 = ++index_;
            pushToVertices(altp2);
            pushToVertices(altp3);
            updateTriangleIndicesAndIndexes((*i2), (*i1), indaltp2);
            updateIndicesAndIndexes((*i3), (*i2),indaltp3, indaltp2);
            updateTriangleIndicesAndIndexes((*i4),(*i3), indaltp3);

            drawNiceCube(p1,&altp2,&altp3,p4,(*i1),indaltp2, indaltp3, (*i4), direction);

        }else if(dot1 == 0 && dot2 <0){
            // neutral /interior
            // same, but with p3 and p4, not p2 and p3


            vec3 altp3 = vec3(0.0,0.0,0.0);
            newPointFromDir(p2,p3,direction, &altp3); // creates the new p2
            vec3 trans = altp3 - (*p2);
            vec3 altp4 = (*p1) + trans;

            int indaltp3 = ++index_;
            int indaltp4 = ++index_;
            pushToVertices(altp3);
            pushToVertices(altp4);
            updateTriangleIndicesAndIndexes((*i2), (*i3), indaltp3);
            updateIndicesAndIndexes((*i4), (*i3),indaltp4, indaltp3);
            updateTriangleIndicesAndIndexes((*i1),(*i4), indaltp4);

            drawNiceCube(p1,p2,&altp3,&altp4,(*i1),(*i2), indaltp3, indaltp4, direction);

        }else if(dot1 <0 && dot2 <0){
            // interior/interior
            vec3 altp2 = vec3(0.0,0.0,0.0);
            vec3 altp4 = vec3(0.0,0.0,0.0);
            vec3 altp3 = vec3(0.0,0.0,0.0);
            newPointFromDir(p1,p2,direction, &altp2);
            newPointFromDir(p1,p4, direction, &altp4);
            altp3 = altp2 + altp3 - (*p1);

            int indaltp2 = ++index_;
            int indaltp3 = ++index_;
            int indaltp4 = ++index_;

            pushToVertices(altp2);
            pushToVertices(altp3);
            pushToVertices(altp4);

            updateTriangleIndicesAndIndexes((*i2), (*i1), indaltp2);
            updateIndicesAndIndexes((*i3), (*i2), indaltp3, indaltp2);
            updateIndicesAndIndexes((*i4), (*i3), indaltp4, indaltp3);
            updateTriangleIndicesAndIndexes((*i1), (*i4), indaltp4);

            drawNiceCube(p1, &altp2, &altp3, &altp4, (*i1), indaltp2, indaltp3, indaltp4, direction);

        }else{
            // in all other cases, we must rotate counter clockwise to handle it from another point.
            //createBranch(p4,p1,p2,p3,i4,i1,i2,i3,direction);
            vec3 fromp3top4 = (*p4) - (*p3);
            vec3 fromp3top2 = (*p2) - (*p3);
            dot1 = dot(fromp3top4,(*direction));
            dot2 = dot(fromp3top2, (*direction));
            if(dot1 < 0 && dot2 == 0){
                // interior / neutral
                // we must create the vector for p4
                // the use it to create the new p1
                // draw the resulting triangles and square
                // finally the cube with those new values
                vec3 altp4 = vec3(0.0,0.0,0.0);
                newPointFromDir(p3,p4,direction, &altp4); // creates the new p2
                vec3 trans = altp4 - (*p3);
                vec3 altp1 = (*p1) + trans;

                int indaltp4 = ++index_;
                int indaltp1 = ++index_;
                pushToVertices(altp4);
                pushToVertices(altp1);
                updateTriangleIndicesAndIndexes((*i4), (*i3), indaltp4);
                updateIndicesAndIndexes((*i1), (*i4),indaltp1, indaltp4);
                updateTriangleIndicesAndIndexes((*i2),(*i1), indaltp1);

                drawNiceCube(&altp1,p2,p3,&altp4,indaltp1,(*i2), (*i3), indaltp4, direction);


            }else if(dot1 == 0 && dot2 <0){
                // neutral / interior

                vec3 altp1 = vec3(0.0,0.0,0.0);
                newPointFromDir(p3,p1,direction, &altp1); // creates the new p2
                vec3 trans = altp1 - (*p3);
                vec3 altp2 = (*p2) + trans;

                int indaltp1 = ++index_;
                int indaltp2 = ++index_;
                pushToVertices(altp1);
                pushToVertices(altp2);
                updateTriangleIndicesAndIndexes((*i4), (*i1), indaltp1);
                updateIndicesAndIndexes((*i2), (*i1),indaltp2, indaltp1);
                updateTriangleIndicesAndIndexes((*i3),(*i2), indaltp2);

                drawNiceCube(&altp1,&altp2,p3,p4,indaltp1,indaltp2, (*i3), (*i4), direction);

            }else{
                // interior / interior
                vec3 altp4 = vec3(0.0,0.0,0.0);
                vec3 altp1 = vec3(0.0,0.0,0.0);
                vec3 altp2 = vec3(0.0,0.0,0.0);
                newPointFromDir(p3,p4,direction, &altp4);
                newPointFromDir(p3,p2, direction, &altp2);
                altp1 = altp4 + altp2 - (*p3);

                int indaltp4 = ++index_;
                int indaltp1 = ++index_;
                int indaltp2 = ++index_;

                pushToVertices(altp4);
                pushToVertices(altp1);
                pushToVertices(altp2);
                updateTriangleIndicesAndIndexes((*i4), (*i3), indaltp4);
                updateIndicesAndIndexes((*i1), (*i4), indaltp1, indaltp4);
                updateIndicesAndIndexes((*i2), (*i1), indaltp2, indaltp1);
                updateTriangleIndicesAndIndexes((*i3), (*i2), indaltp2);

                drawNiceCube(&altp1, &altp2, p3, &altp4, indaltp1, indaltp2, (*i3), indaltp4, direction);
            }
        }

        {
        /*
            * Idea: First, the four points of the last cube (stacky stack ~ )
            * Then, take the points, apply the dir vector.*/
/*
        vec3 pa, pb, pc, pd;
        pa = (*p1) + (*direction);
        pb = (*p2) + (*direction);
        pc = (*p3) + (*direction);
        pd = (*p4) + (*direction);*/

        /*
            * The resulting vector is then projected onto a vector composed betw 2 points
            * If same sign & smaller than said vector = outer point
            * If sign different, flip vector of projection
            * If same sign & bigger than said vector = inner point*/
/*
        int paCount(0), pbCount(0), pcCount(0), pdCount(0);

        paCount = compareProjPoints(&pa, p1, p2, p3, p4);
        pbCount = compareProjPoints(&pb, p2, p1, p3, p4);
        pcCount = compareProjPoints(&pc, p3, p1,p2,p3);
        pdCount = compareProjPoints(&pd, p4, p1, p2,p3);*/

        /* * If there is one point that is "farther" than all others, case of one point
            * If there are two points that are "farthest" of others, but close to each other
            * case of two points
            * From here, we do as follow
            * If case of "one point", then we use three points as the basis. The one further
            * from it will be the only one changed and translated.*/
        //if(paCount == 0){
            // p1 will be the basis of our triangles.
            // p2 - p3 - p4
        /*    someMethod(p1, p2,p3,p4,i1,i2,i3,i4, pa, direction, &pb, &pc, &pd);
        }else if(pbCount == 0){
            // p2 will be the basis of our triangles.
            // p3 - p4 - p1
            someMethod(p2, p3,p4,p1,i2,i3,i4,i1,pb, direction, &pc, &pd, &pa);
        }else if(pcCount == 0){
            // p3 will be the basis of our triangles.
            // p4 - p1 - p2
            someMethod(p3,p4,p1,p2,i3,i4,i1,i2,pc,direction, &pd, &pa,&pb);
        }else if(pdCount == 0){
            // p4 will be the basis of our triangles.
            // p1 - p2 - p3
            someMethod(p4,p1,p2,p3,i4,i1,i2,i3, pd, direction, &pa, &pb, &pc);
        }*/
        /* case of two points*/
        /*else if(paCount ==1){
            if(pbCount == 1){
                // our two points are p1 and p2
                // p2 - p1 / p3 - p4
                someMethod2(p2,p3,p4,p1,i2,i3,i4,i1,pb,pa,direction, &pc, &pd);


            }else if(pcCount == 1){
                // our two points are p1 and p3= IMPOSSIBLE
                cout << "ERROR" << endl;

            }else if(pdCount == 1){
                // our two points are p1 and p4
                // p1 - p4 / p2 - p3
                someMethod2(p1,p2,p3,p4,i1,i2,i3,i4,pa,pd,direction, &pb, &pc);

                cout << "ok" << endl;
            }
        }else if(pbCount == 1){
            if(pcCount == 1){
                // our two points are p2 and p3
                // p3 - p2 / p4 - p1
                someMethod2(p3,p4,p1,p2,i3,i4,i1,i2,pc,pb,direction, &pd, &pa);

                cout << "ok" << endl;
            }else if(pdCount ==1){
                // our two points are p2 and p4= IMPOSSIBLE
                cout << "ERROR" << endl;

            }
        }else if(pcCount == 1){
            if(pdCount == 1){
                // our two points are p3 and p4
                // p4 - p3 / p1 - p2
                someMethod2(p4,p1,p2,p3,i4,i1,i2,i3,pd,pc,direction, &pa, &pb);

                cout << "ok" << endl;
            }
        }*/

        /*
            * If case of "two points", then we have to do two triangles and one square.
            * The idea is to create a new method "createBranch"
            * It must still create the cube, of course
            * But it takes additionnal parameters
            * That will define exactly the aforementioned behaviour
            * First, one method to find the number of points, as well as the points
            * It must then construct the additionnal forms (triangles + square)
            * Then, said method should call the createCube method accordingly
            * createCube will create the box, based on the "new points".
            * It is, of course, extremely important to have 4 stacks, for all 4 points.
            * Note that, since it will be an effective rotation and not a shear anymore
            * The control on the width will thus not become necessary anymore

           */
        /*
                vec3 dirbase = leftp1 - rightp1;
                vec3 axis = normalize(cross(dir0, dirbase));
                vec3 transdir = init_width*normalize(cross(axis,dir0));
                vec3 newLeftPoint = updatePoint(rightp1,transdir);
                vec3 newBotLeftPoint = updatePoint(botrightp1, transdir);
                int leftId = ++index_;
                int botleftId = ++index_;

                lefti1 = popLeftIndex();
                botlefti1 = popBotLeftIndex();

                righti1 = popRightIndex();
                botrighti1 = popBotRightIndex();

                updateTriangleIndicesAndIndexes(newLeftPoint, lefti1, righti1, leftId);
                updateTriangleIndicesAndIndexes(newBotLeftPoint,botlefti1, botrighti1, botleftId);

                ## Updating indices
                lefti0 = ++ index_;
                botlefti0 = ++ index_;
                botrighti0 = ++ index_;
                righti0 = ++ index_;

                #Saving 'em
                leftIndex.push_back(lefti0);
                botleftIndex.push_back(botlefti0);
                rightIndex.push_back(righti0);
                botrightIndex.push_back(botrighti0);


                #Updating points
                leftp0 = updatePoint(dir0, newLeftPoint);
                botleftp0 = updatePoint(dir0, newBotLeftPoint);
                rightp0 = updatePoint(dir0, rightp1);
                botrightp0 = updatePoint(dir0, botrightp1);

                #Saving 'em
                leftPoint.push_back(leftp0);
                botleftPoint.push_back(botleftp0);
                rightPoint.push_back(rightp0);
                botrightPoint.push_back(botrightp0);

                // left, bleft, bright, right
                #Pushing new points to vertices
                pushToVertices(leftp0);
                pushToVertices(botleftp0);
                pushToVertices(botrightp0);
                pushToVertices(rightp0);

                #Drawing the triangles
                updateIndicesAndIndexes(lefti1, righti1, lefti0, righti0);
                updateIndicesAndIndexes(botlefti1, lefti1, botlefti0, lefti0);
                updateIndicesAndIndexes(botrighti1, botlefti1, botrighti0, botlefti0);
                updateIndicesAndIndexes(righti1, botrighti1, righti0, botrighti0);
                //createCubeVolume(leftp1, rightp1, leftp0, lefti1, righti1, lefti0, righti0);
            }           */

        }
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
        vec3 botleftp0, botleftp1;
        vec3 rightp0, rightp1;
        vec3 botrightp0, botrightp1;

        leftp0 = leftPoint.back();
        botleftp0 = botleftPoint.back();
        rightp0 = rightPoint.back();
        botrightp0 = botRightPoint.back();

        int lefti0, lefti1;
        int botlefti0, botlefti1;
        int righti0, righti1;
        int botrighti0, botrighti1;

        lefti0 = leftIndex.back();
        botlefti0 = botLeftIndex.back();
        righti0 = rightIndex.back();
        botrighti0 = botRightIndex.back();

        char lo = 'n';
        char l1 = 'n';
        for(size_t i = 0; i < turtle.treeLength(); i++){
            char str = turtle.charAt(i);
            if(str == ']'){
                dir0 = popDirection();

                leftp0 = popLeftPoint();
                botleftp0 = popBotLeftPoint();
                rightp0 = popRightPoint();
                botrightp0 = popBotRightPoint();

                lefti0 = popLeftIndex();
                botlefti0 = popBotLeftIndex();
                righti0 = popRightIndex();
                botrighti0 = popBotRightIndex();

                branches.pop_back();
            } else if(str == 'A' || str == 'B'){
                lo = str;
                if(direction.size() > 0 && branches.size() > 0 && leftPoint.size() > 0 && rightPoint.size() > 0 && leftIndex.size()>0 && rightIndex.size()>0){
                    l1 = branches.back();

                    dir1 = popDirection();
                    dir0 = updateDirection(dir1, l1, lo);

                    direction.push_back(dir0);

                    leftp1 = popLeftPoint();
                    botleftp1 = popBotLeftPoint();
                    rightp1 =  popRightPoint();
                    botrightp1 = popBotRightPoint();

                    lefti1 = popLeftIndex();
                    botlefti1 = popBotLeftIndex();
                    righti1 = popRightIndex();
                    botrighti1 = popBotRightIndex();

                    createBranch(&leftp1, &botleftp1, &botrightp1, &rightp1,
                                 &lefti1, &botlefti1, &botrighti1, &righti1, &dir0);

                    leftp0 = leftPoint.back();
                    botleftp0 = botleftPoint.back();
                    rightp0 = rightPoint.back();
                    botrightp0 = botRightPoint.back();

                    lefti0 = leftIndex.back();
                    botlefti0 = botLeftIndex.back();
                    righti0 = rightIndex.back();
                    botrighti0 = botRightIndex.back();

                }
            } else if(str == '['){
                direction.push_back(dir0);

                leftPoint.push_back(leftp0);
                botleftPoint.push_back(botleftp0);
                rightPoint.push_back(rightp0);
                botRightPoint.push_back(botrightp0);

                leftIndex.push_back(lefti0);
                botLeftIndex.push_back(botlefti0);
                rightIndex.push_back(righti0);
                botRightIndex.push_back(botrighti0);

                branches.push_back(lo);
            }
        }

        botleftPoint.clear();
        leftPoint.clear();

        botRightPoint.clear();
        rightPoint.clear();

        botLeftIndex.clear();
        leftIndex.clear();

        botRightIndex.clear();
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
