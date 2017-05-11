#include "rulesystem.h"
#include <glm/gtc/type_ptr.hpp>
#include <string.h>
#include "branch.h"
#include <algorithm>
//#include <vector>

using namespace std;
using namespace glm;

/**
 * @brief The Plant class
 * It is the parent class that represents a plant.
 * A plant has a set of rules, a depth, an origin, and is represented with a string, as a tree.
 */
class Plant{
private:
    RuleSystem ruleSystem;
    int depth, sideNo;
    vec3 origin;
    string tree;
    vector<vec3> points;
    vector<int> triangleBranchesIndices, leafTriangleIndices;
    vector<float> branchPoints, leafPoints, branchNormals;
    vector<Branch> branches;
    float angle;

public:

    void updateTriangleIndices(vector<GLuint> &v, vector<int> &a){
        for(size_t i = 0; i < a.size(); ++i){
            v.push_back(GLuint(a.at(i)));
        }
    }

    void updatePointsPush(vector<GLfloat> &p1, vector<float> &p){
        for(size_t i = 0; i < p.size(); ++i){
            p1.push_back(GLfloat(p.at(i)));
        }
    }

    vector<Branch> getBranches(){
        return branches;
    }

    void putBranch(Branch &b){
        branches.push_back(b);
    }

    Branch getBackBranch(){
        return branches.back();
    }


    bool inAlphabet(char c, vector<char> &alphabet){
        return std::find(alphabet.begin(), alphabet.end(), c ) != alphabet.end();
    }

    bool isRotation(char c){
        return c == '-' || c =='+' || c == '/' || c == '\\' || c == '&' || c =='^' || c =='|';
    }

    glm::mat4 getRotation(char c){
        vec3 k = vec3(0.0f,0.0f,0.0f);
        int sign = 1;
        if(isRotation(c)){
            if(c== '-' || c == '+'){
                if(c=='-') sign = -1;
                k = vec3(1.0,0.0,0.0);
            }
            else if(c == '&' || c=='^'){
                if(c=='^') sign = -1;
                k = vec3(0.0,1.0,0.0);
            }
            else if(c == '\\' || c == '/'){
                if(c=='\\') sign =-1;
                k = vec3(0.0,0.0,1.0);
            }
            else{
                k = vec3(180.0,0.0,0.0);
                k*= M_PI/180.0;
                return glm::toMat4(quat(k));
            }
        }
        return glm::toMat4(quat(k*(float(sign*M_PI)/180.0f*angle)));

    }

    void createBase(float width, vector<GLfloat> &branchPoints){
        vec3 upVec = vec3(0.0f,1.0f,0.0f);
        vec3 leftBase = vec3(1.0f,0.0,0.0);
        vec3 startP = origin + leftBase*0.5f*width;
        float anglePoints = 360.0f/sideNo;
        vec3 currP = startP;
        vec3 rotation, dir;

        // Creates the points as they should "originally" be.
        for(size_t i = 0; i < sideNo; ++i){
            currP = normalize(origin - leftBase)*0.5f*width;
            rotation = i*anglePoints*normalize(upVec);
            rotation *= M_PI/180.0;
            dir = vec3(glm::toMat4(quat(rotation))*vec4(currP,1.0f));
            vec3 p = -currP+dir+origin;
            points.push_back(p);
            branchPoints.push_back(p.x);
            branchPoints.push_back(p.y);
            branchPoints.push_back(p.z);
        }

    }


    virtual void Init(int depth, vec3 origin, char axiom, int sideNo, float angle){
        this->depth = depth;
        this->origin = origin;
        this->sideNo = sideNo;
        ruleSystem.Init(axiom);
        tree = string(1, axiom);
        this->angle = angle;
    }

    void addExpansionRule(char base, string expansionRule){
        ruleSystem.addRule(base, expansionRule);
    }

    string substituteString(char a){
        string s,b(1,a);
        s = ruleSystem.getRule(a);
        if(s != ""){
            /*b+= " -> ";*/ // not necessary, apart from clarity
            b+= s;
        }
        return b;
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
     * Puts everything together: expands the tree from the axiom and then generates the plant
     */
    void initTree(){
        firstExpand();
        if(depth>0){
            for(size_t i = 0; i < (depth -1); ++i){
                 expand();
             }
        }
    }

    vector<float> getBranchNormals(){
        return branchNormals;
    }

    vector<int> getBranchIds(){
        return triangleBranchesIndices;
    }

    vector<float> getBr(){
        return this->branchPoints;
    }

    string getTree(){
        return tree;
    }

    void printTree(){
        cout << tree << endl;
    }

    vec3 getOrigin(){
        return origin;
    }

    virtual void drawLeaf(vec3 origin){
        cout << "Drawing a leaf" << endl;
    }

    virtual void drawBranch(char branchType){
        cout <<"Drawing a tree" << endl;
    }

    virtual void drawTree(){
        cout << "Drawing the whole tree" << endl;
    }


    vec3 getPointFromID(vector<float> &vertices, int id){
        return vec3(vertices.at(id*3), vertices.at(id*3+1), vertices.at(id*3+2));
    }

    void createNormals(vector<GLfloat> &branchPoints, vector<GLuint> &indices, vector<GLfloat> *normals){
        for(size_t vertex = 0; vertex < branchPoints.size()/3; ++vertex){
            vector<vec3> vertexNormal; // accumulates all the normals for one vertex
            for(size_t triangle = 0; triangle < indices.size(); triangle+=3){
                int i1 = indices.at(triangle);
                int i2 = indices.at(triangle +1);
                int i3 = indices.at(triangle +2);
                if(vertex == i1 || vertex == i2 || vertex == i3){
                    // bingo! compute the normal of this triangle and add
                    // it to the array of normals for this vertex
                    vec3 p1 = getPointFromID(branchPoints, i1);
                    vec3 p2 = getPointFromID(branchPoints, i2);
                    vec3 p3 = getPointFromID(branchPoints, i3);
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
            (*normals).push_back(normalf.x);
            (*normals).push_back(normalf.y);
            (*normals).push_back(normalf.z);// by hypothesis the normals should be pushed in the same order as the points
            vertexNormal.clear();
        }
    }


};
