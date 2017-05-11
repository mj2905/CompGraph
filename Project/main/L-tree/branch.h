#include <vector>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace glm;
using namespace std;

class Branch{
private:
    vector<vec3> upPoints, endPoints;
    vec3 upVector, leftVector;
    vector<int> upIndices, endIndices;
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


    void pushIndicesAsQuad(int bl, int br, int ul, int ur){
        triangleIndices.push_back(bl);
        triangleIndices.push_back(br);
        triangleIndices.push_back(ur);
        triangleIndices.push_back(ur);
        triangleIndices.push_back(bl);
        triangleIndices.push_back(ul);
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
        pushIndicesAsQuad(i1,i2,i3,i4);

        // Creates the remaining "quads" except the last one
        for(size_t i =1; i < idsOriginBranch.size()-1; i++){
            i1 = idsOriginBranch.at(i);
            i2 = idsOriginBranch.at(i+1);
            i3 = triangleIds.at(triangleIds.size()-3);
            i4 = ++(*index);
            indicesToUpdate.push_back(i4);
            pushIndicesAsQuad(i1,i2,i3,i4);
        }

        // Creates the last one
        //i1 = idsOriginBranch.at(idsOriginBranch.size()-2);
        //i2 = idsOriginBranch.back();
        //i3 = indicesToUpdate.at(indicesToUpdate.size()-2);
        //i4 = indicesToUpdate.back();

        i1 = triangleIds.at(triangleIds.size()-1-4);
        i2 = triangleIds.at(triangleIds.size()-1-13);
        i3 = triangleIds.at(triangleIds.size()-1-2);
        i4 = triangleIds.at(triangleIds.size()-1-12);
        pushIndicesAsQuad(i1,i2,i3,i4);
    }

    void fusePointsArray(){
        for(size_t i = 0; i < endPoints.size(); ++i){
            upPoints.push_back(endPoints.at(i));
        }

        for(size_t i = 0; i < upPoints.size(); ++i){
            pushPoint(upPoints.at(i));
        }

        endPoints.clear();
    }



public:


    void createBranch(vec3 direction, vec3 origin, vec3 upVec, vec3 leftBase, float length, float width,
                      vector<int> baseIds, vec3 trans, int *index){
        float angle = acos(dot(normalize(direction), normalize(upVec)));
        vec3 startP = origin + leftBase*0.5f*width;
        float anglePoints = 360.0f/baseIds.size();
        vec3 currP = startP;
        vec3 rotation, dir;
        if(direction.x == 0 && direction.y == 0 && direction.z == 0){
            this->upVector = vec3(0.0,0.0,0.0);
        }else{
            this->upVector = normalize(direction);
        }
        vec3 ca = direction;
        vec3 cb= dot(direction, normalize(upVec))*normalize(upVec);
        if(ca.x == cb.x && ca.y == cb.y && ca.z == cb.z){ // they are equal, so leftBase is the same
            this->leftVector = leftBase;
        }else{
            this->leftVector = normalize(ca-cb);
        }

        // Creates the points as they should "originally" be.
        for(size_t i = 0; i < baseIds.size(); ++i){
            vec3 o = origin - leftBase;
            if(o.x ==0 && o.y == 0 && o.z == 0 ){
                currP = vec3(0.0,0.0,0.0);
            }else{
                currP = normalize(origin - leftBase)*0.5f*width;
            }
            if(upVec.x == 0 && upVec.y == 0 && upVec.z ==0){
                rotation *= 0.0f;
            }else{
                rotation = i*anglePoints*normalize(upVec);
                rotation *= M_PI/180.0;
            }

            dir = vec3(glm::toMat4(quat(rotation))*vec4(currP,1.0f));
            upPoints.push_back(origin-currP+dir);
        }

        //Now to translate all points to the origin
        for(size_t i = 0; i < upPoints.size(); ++i){
            upPoints.at(i) = upPoints.at(i) - origin;
        }

        //Next up, we rotate every point with a rotation matrix and put them back up

        vec3 rotationAxis = this->leftVector;//normalize(direction - dot(direction, normalize(upVec))*normalize(upVec));

        rotation = rotationAxis*(angle*float(M_PI)*180.0f);
        float currMin = 0.0f;
        for(size_t i =0; i < upPoints.size(); ++i){
            upPoints.at(i) = vec3(glm::toMat4(quat(rotation))*vec4(upPoints.at(i),1.0f));
            currMin = std::min(currMin, dot(upPoints.at(i), normalize(upVec)));
        }
        for(size_t i = 0; i < upPoints.size(); ++i){
            upPoints.at(i) = upPoints.at(i) - currMin*upVec + origin + trans;
            this->pushPoint(upPoints.at(i));
        }
        this->childrenOrigin = -currMin*upVec + origin + trans+normalize(direction)*length;
        this->origin = -currMin*upVec + origin + trans;
        // takes care of the base
        circleBranch(baseIds, upIndices, triangleIndices, index);

        // Now to create the new points from the branch
        for(size_t i = 0; i < upPoints.size(); ++i){
            endPoints.push_back(upPoints.at(i) + normalize(direction)*length);
        }

        circleBranch(upIndices, endIndices, triangleIndices, index);
        fusePointsArray();
        initialized = true;
    }


    vec3 getChildrenOrigin(){
        return childrenOrigin;
    }

    vec3 getLeftVector(){
        return leftVector;
    }

    vec3 getUpVector(){
        return upVector;
    }

    vector<int> getTriangleArray(){
        return triangleIndices;
    }

    vector<float> getPointsArray(){
        return pointsPush;
    }

    vector<vec3> getPoints(){
        return upPoints;
    }

    vector<int> getEndPointIndices(){
        return endIndices;
    }

    vec3 getUpVec(){
        return normalize(direction);
    }
    /*
    vec3 getLeftPoint(){
        return normalize(upPoints.at(0) - origin);
    }*/

    void clean(){
        this->upIndices.clear();
        this->upIndices.clear();
        this->triangleIndices.clear();
    }

};
