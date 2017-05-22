#include "glm/gtc/type_ptr.hpp"

class LightSource{
private:
    glm::vec3 pos;
public:
    void Init(float x, float y, float z){
        this->pos = glm::vec3(x,y,z);
    }

    glm::vec3 getPosition(){
        return pos;
    }

    void update(glm::vec3 updateVector){
        this->pos = updateVector;
    }
};
