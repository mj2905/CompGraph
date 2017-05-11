#pragma once

#include <string.h>
#include <sstream>
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include "stack.h"

using namespace glm;
using namespace std;
/**
 * @brief The Turtle class
 */
class Turtle {
private:
    Stack<vec3> direction, origin, up, left;
    Stack<vector<int>> baseIndices;
    Stack<float> width, length;
    Stack<char> letter;

public:
    void pushBackStatesTurtle(vec3 dir, vec3 o, vec3 u, vec3 lef,vector<int> &bI, float w,
                              float l, char c){
        this->direction.push_back(dir);
        this->up.push_back(u);
        this->left.push_back(lef);
        this->origin.push_back(o);
        this->baseIndices.push_back(bI);
        this->width.push_back(w);
        this->length.push_back(l);
        this->letter.push_back(c);
    }

    void popStatesTurtle(){
        direction.popStack();
        origin.popStack();
        baseIndices.popStack();
        width.popStack();
        length.popStack();
        letter.popStack();
        up.popStack();
        left.popStack();
    }

    bool emptyTurtle(){
        return origin.size() == 0;
    }

    vector<int> getCurrBaseIds(){
        return baseIndices.back();
    }

    vec3 getCurrDir(){
        return direction.back();
    }

    vec3 getCurrUp(){
        return up.back();
    }

    vec3 getCurrLeft(){
        return left.back();
    }

    vec3 getCurrOrigin(){
        return origin.back();
    }

    float getCurrWidth(){
        return width.back();
    }

    float getCurrLength(){
        return length.back();
    }

    char getCurrLetter(){
        return letter.back();
    }

};
