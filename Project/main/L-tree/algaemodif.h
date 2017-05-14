#include "plant.h"
#include <algorithm>
#include "turtle.h"
#include "flexigrid.h"

#define SIDE_NBR 6
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
    Branch br;

public:

    void Init(int depth, vec3 origin, char axiom, int *index, GLuint texture_id){
        Plant::Init(depth, origin, axiom,SIDE_NBR, ANGLE_FUNC);
        addExpansionRule('A', "[+A--B]");
        addExpansionRule('B', "[A]");
        alphabet.push_back('A');
        alphabet.push_back('B');

        (*index) = -1;
        w = 0.2;
        l = 0.5;
        this->index = (*index);

        dir = vec3(0.0,1.0,0.0);
        o = origin;
        up = vec3(0.0,1.0,0.0);
        left = vec3(1.0,0.0,0.0);

        //First, initialize the points that constitute the very base
        Plant::createBase(w, points);

        //Then, push them to the indices as "baseIds" denote the id of the base
        for(size_t i = 0; i < SIDE_NBR; ++i){
            baseIds.push_back(++(this->index));
        }

        //The translation from the base
        trans = vec3(0.0f,0.01f,0.0f);

        //Expands the tree and then draws it in 3D
        drawTree();

        //Creates the normals for the points
        Plant::createNormals(points,pointsIds, &normals);

        cout << "#ids:" << pointsIds.size() << endl;
        cout << "#normals: " << normals.size() << endl;
        cout << "#branch: " << points.size() << endl;

        //And finally pass it all to the appropriate grid
        branchGrid.Init(points, pointsIds, normals, texture_id);
    }

    void drawBranch(char c, bool update){
        cout << "Drawing algae branch" << endl;
        Branch b;
        // Creates a branch
        b.createBranch(dir,o,up,left,trans,l,w,baseIds,&index);
        // Stores it
        Plant::putBranch(b);
        if(update){
            this->br = b;
        }
        vector<float> branchPoints = b.getPointsArray();
        vector<int> branchIndices = b.getTriangleArray();
        Plant::updatePointsPush(points, branchPoints);
        Plant::updateTriangleIndices(pointsIds,branchIndices);
    }

    void drawLeaf(){
        cout << "Drawing leaf branch" << endl;
    }


     void drawTree(){
        Plant::initTree();
        this->tree = Plant::getTree();
        //this->tree = "-A[AB[A]]";

        printTree();

        char c = ';';
        vec3 endOrigin = vec3(0.0,0.0,0.0);
        vector<int> endIds;
        vec3 endUp, endDir;
        vec3 endLeft;
        Branch endB;

        for(size_t i = 0; i < tree.size(); ++i){
            c = tree.at(i);
            if(inAlphabet(c,alphabet)){
                //If the turtle is empty (happens for the 1st letter), we must update it
                cout << "--- Used: ---" << endl;
                cout << "Direction: x: " << dir.x << " y: " << dir.y << " z : " << dir.z << endl;
                cout << "Origin: x: " << o.x << " y: " << o.y << " z: " << o.z << endl;
                if(turtle.emptyTurtle()){
                    drawBranch(c, true);
                    turtle.pushBackStatesTurtle(dir, o, vec3(0.0,1.0,0.0), left, baseIds, w, l, c, br);
                }else{
                    drawBranch(c,false); // draw the branch, updates the indices and points to add it to the global array
                }
                //Finally, we store all end values as the values of the end of the created branch
                endOrigin = Plant::getBackBranch().getChildrenOrigin();
                endIds = Plant::getBackBranch().getEndPointIndices();
                endUp = vec3(0.0,1.0,0.0);//Plant::getBackBranch().getDirection();
                endLeft = Plant::getBackBranch().getLeftVector();
                endB = Plant::getBackBranch();
                endDir = Plant::getBackBranch().getDirection();

                //cout << "End origin:" << " x:" << endOrigin.x << " y:" << endOrigin.y << " z:" << endOrigin.z << endl;
                //cout << "Origin: " << " x:" << o.x << " y:" << o.y << " z:" << o.z << endl;

            }
            else if(c == '['){
                // If we recurse, then the "base" values are now the values of the mother branch
                // We save the previous values, and then update our current ones
                o = endOrigin;
                up = endUp;
                left = endLeft;
                baseIds = endIds;
                br = endB;
                dir = endDir;
                turtle.pushBackStatesTurtle(dir, o, vec3(0.0,1.0,0.0), left, baseIds, w, l, c, br);

            }
            else if(c == ']') {
                // When going out of the recursion, we pop the turtle and then take as value the ones at the end of the stack
                turtle.popStatesTurtle();
                o = turtle.getCurrOrigin();
                up = turtle.getCurrUp();
                left = turtle.getCurrLeft();
                baseIds = turtle.getCurrBaseIds();
                br = turtle.getCurrBranch();
                dir = turtle.getCurrDir();
            }
            else if(Plant::isRotation(c)){
                // Update the rotation according to the rules of the L system and the angle of the plant
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
