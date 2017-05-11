#include "plant.h"
#include <algorithm>
#include "turtle.h"
#include "flexigrid.h"

#define SIDE_NBR 4
#define ANGLE_FUNC 22.5

/**
 * @brief The algaeModif class
 */
class algaeModif: public Plant{
private:
    Flexigrid branchGrid;
    string tree;
    vector<char> alphabet;
    Turtle turtle;
    int index;
    vec3 dir, up, left, o, trans;
    float l, w;
    vector<int> baseIds;
    vector<GLuint> pointsIds;
    vector<GLfloat> points, normals;

public:

    void Init(int depth, vec3 origin, char axiom, int *index, GLuint texture_id){
        Plant::Init(depth, origin, axiom,SIDE_NBR, ANGLE_FUNC);
        addExpansionRule('A', "[//AB\\]");
        alphabet.push_back('A');
        alphabet.push_back('B');
        addExpansionRule('B', "[A]");
        (*index) = -1;
        w = 0.2;
        l = 0.5;
        this->index = (*index);

        dir = vec3(0.0,1.0,0.0);
        o = origin;
        up = vec3(0.0,1.0,0.0);
        left = vec3(1.0,0.0,0.0);

        Plant::createBase(w, points);

        printTree();

        for(size_t i = 0; i < SIDE_NBR; ++i){
            baseIds.push_back(++(this->index));
        }

        drawTree();
        Plant::createNormals(points,pointsIds, &normals);
        vector<int> idsTmp = getBranchIds();
        vector<GLuint> ids;
        for(size_t i =0; i < idsTmp.size(); ++i){
            ids.push_back(GLuint(idsTmp.at(i)));
        }
        cout << "#ids:" << pointsIds.size() << endl;
        cout << "#normals: " << normals.size() << endl;
        cout << "#branch: " << points.size() << endl;
        vector<GLuint> test;
        test.push_back(0);
        test.push_back(1);
        test.push_back(2);
        branchGrid.Init(points, pointsIds, normals, texture_id);
    }

    void drawBranch(char c){
        cout << "Drawing algae branch" << endl;
        Branch b;
        b.createBranch(dir, o,up, left, l, w, baseIds,trans, &index);
        Plant::putBranch(b);
        vector<float> a = b.getPointsArray();
        vector<int> t = b.getTriangleArray();
        Plant::updatePointsPush(points, a);
        Plant::updateTriangleIndices(pointsIds,t);
    }

    void drawLeaf(){
        cout << "Drawing leaf branch" << endl;
    }


    void drawTree(){
        Plant::initTree();
        this->tree = Plant::getTree();

        printTree();

        char c = ';';
        vec3 endOrigin = vec3(0.0,0.0,0.0);
        vector<int> endIds;
        vec3 endUp;
        vec3 endLeft;

        for(size_t i = 0; i < tree.size(); ++i){
            c = tree.at(i);
            if(inAlphabet(c,alphabet)){ // we need to draw it
                if(turtle.emptyTurtle()){
                    turtle.pushBackStatesTurtle(dir, o, up, left, baseIds, w, l, c);
                }
                drawBranch(c); // draw the branch, updates the indices and points to add it to the global array
                endOrigin = Plant::getBackBranch().getChildrenOrigin();
                endIds = Plant::getBackBranch().getEndPointIndices();
                endUp = Plant::getBackBranch().getUpVector();
                endLeft = Plant::getBackBranch().getLeftVector();
            }
            else if(c == '['){
                turtle.pushBackStatesTurtle(dir, o, up, left, baseIds, w, l, c);
                o = endOrigin;
                up = endUp;
                left = endLeft;
                baseIds = endIds;
            }
            else if(c == ']') {
                turtle.popStatesTurtle();
                o = turtle.getCurrOrigin();
                up = turtle.getCurrUp();
                left = turtle.getCurrLeft();
                baseIds = turtle.getCurrBaseIds();
            }
            else if(Plant::isRotation(c)){
                dir = vec3(Plant::getRotation(c)*vec4(dir,1.0f));
            }

        }

    }

    void Draw(const glm::mat4 &model = IDENTITY_MATRIX,
              const glm::mat4 &view = IDENTITY_MATRIX,
              const glm::mat4 &projection = IDENTITY_MATRIX){
        branchGrid.Draw(model, view, projection);
    }




};
