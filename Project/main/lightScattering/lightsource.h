#include <vector>
#include "icg_helper.h"
#include "solidsphere.h"


using namespace std;
using namespace glm;

class LightSource{
private:
    vec3 position;
    float diffuseProperty[3];
    float materialDiffuseProp[4];
    GLuint dList;
    SolidSphere sphere;

public:
    void Init(float x, float y, float z){
        position = vec3(x,y,z);
        diffuseProperty[0] = 1.0f ; //x
        diffuseProperty[1] = 1.0f ; //y
        diffuseProperty[2] = 1.0f ; //z


        materialDiffuseProp[0] = 1.0f;//x
        materialDiffuseProp[1] = 1.0f;//y
        materialDiffuseProp[2] = 1.0f;//z
        materialDiffuseProp[3] = 1.0f;//q

        //dList = glGenLists(1);
   //     glNewList(dList,GL_COMPILE);

            glColor4f(diffuseProperty[0],diffuseProperty[1],diffuseProperty[2],1);
            sphere.Init(1.0,3,3);


        //glEndList();

    }

    vec3 getPosition(){
        return position;
    }


    void modifyLight(float x, float y, float z){
        position = vec3(x,y,z);
    }

    void drawLight(){
        sphere.Draw(IDENTITY_MATRIX, IDENTITY_MATRIX, IDENTITY_MATRIX);

 //       glCallList(dList);
    }
};
