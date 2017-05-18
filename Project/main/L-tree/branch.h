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

    /*void translateBranch(vec3 dir, float length){
        mat4 tr = glm::translate(mat4(1.0f), dir*length);
        for(size_t i = 0; i < branchBasePoints.size(); ++i){
            branchBasePoints.at(i) = vec3(tr*vec4(branchBasePoints.at(i),1.0f));
        }
    }

    void createEndPointsFromDirAndLength(vec3 dir, float length){
        for(size_t i = 0; i < branchBasePoints.size(); ++i){
            vec3 p = branchBasePoints.at(i) + vec3(0.0, length,0.0);
            endPoints.push_back(p);
        }
    }
*/

/*    void rotateBranch(vec3 rotation){
        for(size_t i = 0; i < branchBasePoints.size(); ++i){
            branchBasePoints.at(i) = vec3(glm::toMat4(quat(rotation))*vec4(branchBasePoints.at(i),1.0f));
        }

        this->origin = vec3(glm::toMat4(quat(rotation))*vec4(this->origin,1.0f));
    }

    void rotateEndPoints(vec3 rotation){
        for(size_t i = 0; i< endPoints.size(); ++i){
            endPoints.at(i) = vec3(glm::toMat4(quat(rotation))*vec4(endPoints.at(i),1.0f));
        }
        this->childrenOrigin = vec3(glm::toMat4(quat(rotation))*vec4(this->childrenOrigin,1.0f));
    }*/

    void printVector(vec3 v, string name){
        cout << name << ": " <<  v.x << "," << v.y <<"," << v.z << endl;
    }

    void createBranch(vec3 direction, vec3 origin, vec3 upVec, vec3 leftBase, vec3 trans,
                      float length, float width, vector<int> baseIds, int *index){

        branchBasePoints.clear();

        upVec = vec3(0.0,1.0,0.0);
        direction = vec3(0.0,0.0,1.0);
        vec3 rotAxis = vec3(1.0f,0.0f,0.0f);
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
            //currP = float(0.5*width)*normalize(direction-dot(direction, normalize(upVec))*normalize(upVec));
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

            branchBasePoints.at(i) = vec3(rotate(mat4(1.0f), angle, normalize(rotAxis))*vec4(branchBasePoints.at(i),1.0f));
        }


        for(auto a: branchBasePoints){
            cout << "Point of base: x :" << a.x << ", y : " << a.y << " , z : " << a.z << endl;
        }


        //vec3 v1 = normalize(branchBasePoints.at(0)-branchBasePoints.at(1));
        vec3 v1 = branchBasePoints.at(1) - branchBasePoints.at(0);
        vec3 v2 = branchBasePoints.at(3) - branchBasePoints.at(0);

        vec3 u1 = v1;
        vec3 u2 = v2 - dot(v2,u1)/dot(u1,u1)*u1;
        vec3 v3 = cross(v1,v2);
       // vec3 u3 = v3 - dot(v3,u1)/dot(u1,u1)*u1 - dot(v3,u2)/dot(u2,u2)*u2;
        vec3 e1 = normalize(u1);
        vec3 e2 = normalize(u2);
        vec3 e3 = normalize(v3);
        printVector(e1, "e1");
        printVector(e2, "e2");
        printVector(e3, "e3");


        vec3 transDir = e3;
        printVector(transDir, "transDir");

        for(size_t i = 0; i < branchBasePoints.size(); ++i){
            vec3 t = branchBasePoints.at(i) + transDir;
            endPoints.push_back(t);
        }

        for(size_t i =0; i <branchBasePoints.size(); ++i){
            branchBaseIndices.push_back(++(*index));
        }
        //circleBranch(baseIds,branchBaseIndices, triangleIndices, index);
        circleBranch(branchBaseIndices,endIndices, triangleIndices,index);
        fusePointsArray();

        this->childrenOrigin = this->origin + normalize(direction)*length+
                normalize(direction)*std::sqrt(trans.x*trans.x + trans.y*trans.y + trans.z*trans.z);

        for(size_t i = 0; i < branchBasePoints.size(); ++i) pushPoint(branchBasePoints.at(i));

        // TO START AGAIN SO THAT IT WORKS
        // BASE OF THE ALGORITHM:
        /*
         * Create the base as points at the origin
         * Create a second base, that will be the end base
         * Link them (create the triangle ids)
         * Rotate both of them
         * Translate both of them to the origin of the mother branch
         * Translate both of them again by a ||trans|| norm vector so that it doesn't touch (much) with
         * mother branch (overlaps are bad for Z-buffer).
         * Finally translate the end base by a ||length|| vector of direction direction (from i/2 to end)
         * */

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
