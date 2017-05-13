#include <vector>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace glm;
using namespace std;

class Branch{
private:
    vector<vec3> branchBasePoints, endPoints;
    vec3 leftVector;
    vector<int> branchBaseIndices, endIndices;
    vector<int> triangleIndices;
    vector<float> pointsPush;
    vec3 projOnUp, frontBase, sideBase, sideTrans, origin, direction, upVec, childrenOrigin;
    float width, length;
    bool initialized = false;


    void pushPoint(vec3 p){
        pointsPush.push_back(p.x);
        pointsPush.push_back(p.y);
        pointsPush.push_back(p.z);
    }


    void pushIndicesAsQuad(vector<int> &indicesToUpdate, int bl, int br, int ul, int ur){
        indicesToUpdate.push_back(bl);
        indicesToUpdate.push_back(br);
        indicesToUpdate.push_back(ur);
        indicesToUpdate.push_back(ur);
        indicesToUpdate.push_back(bl);
        indicesToUpdate.push_back(ul);
    }

    void circleBranch(vector<int> idsOriginBranch, vector<int> &indicesToUpdate,
                      vector<int> &triangleIds, int *index){

        // Creates first quad
        int i1 = idsOriginBranch.at(0);
        int i2 = idsOriginBranch.at(1);
        int i3 = ++(*index);
        int i4 = ++(*index);
        indicesToUpdate.push_back(i3);
        indicesToUpdate.push_back(i4);
        pushIndicesAsQuad(triangleIds,i1,i2,i3,i4);

        // Creates the remaining "quads" except the last one
        for(size_t i =1; i < idsOriginBranch.size()-1; i++){
            i1 = idsOriginBranch.at(i);
            i2 = idsOriginBranch.at(i+1);
            i3 = triangleIds.at(triangleIds.size()-3);
            i4 = ++(*index);
            indicesToUpdate.push_back(i4);
            pushIndicesAsQuad(triangleIds, i1,i2,i3,i4);
        }

        // Creates the last one
        i1 = triangleIds.at(triangleIds.size()-1-4);
        i2 = triangleIds.at(triangleIds.size()-1-13);
        i3 = triangleIds.at(triangleIds.size()-1-2);
        i4 = triangleIds.at(triangleIds.size()-1-12);
        pushIndicesAsQuad(triangleIds, i1,i2,i3,i4);
    }

    void fusePointsArray(){
        /*for(size_t i = 0; i < endPoints.size(); ++i){
            pushPoint(endPoints.at(i));
        }*/
        for(size_t i = 0; i < endPoints.size(); ++i){
            branchBasePoints.push_back(endPoints.at(i));
        }

        //endPoints.clear();
    }



public:

    void translateBranch(vec3 dir, float length){
        mat4 tr = glm::translate(mat4(1.0f), dir)*length;
        for(size_t i = 0; i < branchBasePoints.size(); ++i){
            branchBasePoints.at(i) = vec3(tr*vec4(branchBasePoints.at(i),1.0f)); //+ vec3(0.0,- biggerDepth,0.0) + origin+trans;
            //this->pushPoint(branchBasePoints.at(i));
        }
    }


    void rotateBranch(vec3 axis, float angle){
        mat4 rot = glm::rotate(mat4(1.0f), angle, axis);
        for(size_t i = 0; i < branchBasePoints.size(); ++i){
            branchBasePoints.at(i) = vec3(rot*vec4(branchBasePoints.at(i),1.0f)); //+ vec3(0.0,- biggerDepth,0.0) + origin+trans;
            //this->pushPoint(branchBasePoints.at(i));
        }
    }

    void createBranch(vec3 direction, vec3 origin, vec3 upVec, vec3 leftBase, vec3 trans,
                      float length, float width, vector<int> baseIds, int *index){

        this->direction = direction;

        if(!(upVec.x == direction.x && upVec.y == direction.y && upVec.z == direction.z)){
            this->leftVector = normalize(direction - dot(direction, normalize(upVec))*normalize(upVec));
        }else{
            this->leftVector = leftBase;
        }

        float angle = acos(dot(normalize(direction), normalize(upVec)));
        float anglePoints = 360.0f/baseIds.size();
        vec3 currP, rotation, dir;
        currP = vec3(0.5*width,0.0,0.0);


        // Creates the points as they should "originally" be.
        for(size_t i = 0; i < baseIds.size(); ++i){
            rotation = vec3(0.0,i *anglePoints * M_PI/180.0f,0.0);
            dir = vec3(glm::toMat4(quat(rotation))*vec4(currP,1.0f));
            branchBasePoints.push_back(dir);
        }

        circleBranch(baseIds, branchBaseIndices, triangleIndices, index);

        for(size_t i = 0; i < branchBasePoints.size(); ++i){
            vec3 p = branchBasePoints.at(i) + vec3(0.0,length,0.0);
            if(p.x == branchBasePoints.at(i).x && p.y == branchBasePoints.at(i).y && p.z == branchBasePoints.at(i).z){
                cout << "This is a problem" << endl;
            }
            endPoints.push_back(p);
        }

        circleBranch(branchBaseIndices, endIndices, triangleIndices, index);


        fusePointsArray();

        //Next up, we rotate every point with a rotation matrix and put them back up
        rotation = this->leftVector*angle*float(M_PI)/180.0f;
        float biggerDepth = 0.0f;
        for(size_t i =0; i < branchBasePoints.size(); ++i){
            branchBasePoints.at(i) = vec3(glm::toMat4(quat(rotation))*vec4(branchBasePoints.at(i),1.0f));
            biggerDepth = std::min(biggerDepth, dot(branchBasePoints.at(i), vec3(0.0,1.0,0.0)));
        }


        //translateBranch(vec3(1,0.0,0.0),1.0);

        //rotateBranch(vec3(0.0,1.0,0.0),90);
        //translateBranch(normalize(origin))

        /*translate(vec3(0.0,1.0,0.0), -biggerDepth);
        translateBranch(origin, 1.0);
        translateBranch(trans, 1.0);*/
        for(size_t i = 0; i < branchBasePoints.size(); ++i){
            branchBasePoints.at(i) = branchBasePoints.at(i) + vec3(0.0,- biggerDepth,0.0) + origin+trans;
        }

        for(size_t i = 0; i < branchBasePoints.size();++i){
            pushPoint(branchBasePoints.at(i));
        }


        this->childrenOrigin = vec3(0.0,-biggerDepth,0.0) + origin + trans+normalize(direction)*length;
        this->origin = vec3(0.0,-biggerDepth,0.0) + origin + trans;

        initialized = true;
    }


    vec3 getChildrenOrigin(){
        return childrenOrigin;
    }

    vec3 getLeftVector(){
        return leftVector;
    }

    vector<int> getTriangleArray(){
        return triangleIndices;
    }

    vector<float> getPointsArray(){
        return pointsPush;
    }

    vector<int> getEndPointIndices(){
        return endIndices;
    }

    vec3 getDirection(){
        return normalize(direction);
    }

    void clean(){
        this->branchBaseIndices.clear();
        this->branchBaseIndices.clear();
        this->triangleIndices.clear();
    }

};
