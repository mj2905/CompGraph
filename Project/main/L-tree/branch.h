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
    vector<int> branchBaseIndices, endIndices;
    vector<int> triangleIndices;
    vector<float> pointsPush;
    vec3 origin, direction, upVec, childrenOrigin,leftVector, frontVector;
    float width, length;


    void pushPoint(vec3 p){
        pointsPush.push_back(p.x);
        pointsPush.push_back(p.y);
        pointsPush.push_back(p.z);
    }

    vec3 goodCross(vec3 v1, vec3 v2){
        return vec3(v1.y*v2.z - v2.y*v1.z, v1.x*v2.z-v1.z*v2.x, v1.x*v2.y-v1.y*v2.x);
    }


    void pushIndicesAsQuad(vector<int> &indicesToUpdate, int bl, int br, int ul, int ur){
        indicesToUpdate.push_back(bl);
        indicesToUpdate.push_back(br);
        indicesToUpdate.push_back(ur);
        indicesToUpdate.push_back(ur);
        indicesToUpdate.push_back(bl);
        indicesToUpdate.push_back(ul);
    }

    void connectBase(vector<int> idsOriginBranch, vector<int> &indicesToUpdate,
                      vector<int> &triangleIds, int *index){

        // Creates first quad
        vector<int> tab;
        for(size_t i =0; i < idsOriginBranch.size(); ++i){
            tab.push_back(idsOriginBranch.at((i+idsOriginBranch.size()/2)%idsOriginBranch.size()));
        }
        int i1 = tab.at(0);
        int i2 = tab.at(1);
        int i3 = ++(*index);
        int i4 = ++(*index);
        indicesToUpdate.push_back(i3);
        indicesToUpdate.push_back(i4);
        pushIndicesAsQuad(triangleIds,i1,i2,i3,i4);

        // Creates the remaining "quads" except the last one
        for(size_t i =1; i < idsOriginBranch.size()-1; i++){
            i1 = tab.at(i);
            i2 = tab.at(i+1);
            i3 = triangleIds.at(triangleIds.size()-3);
            i4 = ++(*index);
            indicesToUpdate.push_back(i4);
            pushIndicesAsQuad(triangleIds, i1,i2,i3,i4);
        }

        // Creates the last one
        i1 = triangleIds.at(triangleIds.size()-1-4);
        i2 = tab.at(0);
        i3 = triangleIds.at(triangleIds.size()-1-2);
        i4 = tab.back()+1;
        pushIndicesAsQuad(triangleIds, i1,i2,i3,i4);
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
        i2 = idsOriginBranch.at(0);
        i3 = triangleIds.at(triangleIds.size()-1-2);
        i4 = idsOriginBranch.back()+1;
        pushIndicesAsQuad(triangleIds, i1,i2,i3,i4);
    }

    void fusePointsArray(){
        for(size_t i = 0; i < endPoints.size(); ++i){
            branchBasePoints.push_back(endPoints.at(i));
        }
    }


public:

    void printVector(vec3 v, string name){
        cout << name << ": " <<  v.x << "," << v.y <<"," << v.z << endl;
    }

    void createBranch(vec3 direction, vec3 origin, vec3 upVec, vec3 leftBase, vec3 trans,
                      float length, float width, vector<int> baseIds, int *index){

        branchBasePoints.clear();

        vec3 rotAxis;
        if(upVec != direction){
            rotAxis = normalize(cross(upVec, direction));
        }else{
            rotAxis = vec3(1.0,0.0,0.0);
        }

        this->direction = direction;

        if(upVec.x != direction.x || upVec.y != direction.y || upVec.z != direction.z){
            this->leftVector = cross(upVec,normalize(direction - dot(direction, normalize(upVec))*normalize(upVec)));
        }else{
            this->leftVector = leftBase;
        }

        float angle = acos(dot(normalize(direction), normalize(upVec)));
        float anglePoints = 360.0f/baseIds.size();

        vec3 currP, rotation, dir;
        if(direction != upVec){
            currP = direction - dot(direction, upVec)/dot(upVec,upVec)*upVec;
            currP = float(0.5*width)*normalize(currP);
        }
        else{
            currP = vec3(0.5*width,0.0,0.0);
        }

        // Creates the points as they should "originally" be.
        printVector(currP, "currP");
        printVector(direction, "direction");
        printVector(upVec, "upVec");

        for(size_t i = 0; i < baseIds.size(); ++i){
            rotation = normalize(upVec)*float(i*anglePoints*M_PI/180.0f);
            dir = vec3(glm::toMat4(quat(rotation))*vec4(currP,1.0f));
            branchBasePoints.push_back(dir);
        }

        cout << "angle:" << angle << endl;
        for(size_t i =0; i<branchBasePoints.size(); ++i){
            branchBasePoints.at(i) = vec3(rotate(mat4(1.0f), angle, normalize(rotAxis))*vec4(branchBasePoints.at(i),1.0f)) +
                    origin + trans;
        }


        for(auto a: branchBasePoints){
            cout << "Point of base: x :" << a.x << ", y : " << a.y << " , z : " << a.z << endl;
        }


        vec3 v1 = branchBasePoints.at(1) - branchBasePoints.at(0);
        vec3 v2 = branchBasePoints.at(3) - branchBasePoints.at(0);

        vec3 u1 = v1;
        vec3 u2 = v2 - dot(v2,u1)/dot(u1,u1)*u1;
        vec3 v3 = cross(v1,v2);
        vec3 e1 = normalize(u1);
        vec3 e2 = normalize(u2);
        vec3 e3 = normalize(v3);

        vec3 transDir = e3;
        printVector(transDir, "transDir");

        for(size_t i = 0; i < branchBasePoints.size(); ++i){
            vec3 t = branchBasePoints.at(i) + transDir*length;
            endPoints.push_back(t);
        }

        circleBranch(baseIds,branchBaseIndices, triangleIndices, index);
        circleBranch(branchBaseIndices,endIndices, triangleIndices,index);
        fusePointsArray();


        this->childrenOrigin = this->origin + transDir*length+origin + trans;

        for(size_t i = 0; i < branchBasePoints.size(); ++i) pushPoint(branchBasePoints.at(i));

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
