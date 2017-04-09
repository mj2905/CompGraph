#pragma once
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Leaf {

    private:
        Leaf parent;
        Leaf leftChild;
        Leaf rightChild;
        vec3 origin;
        GLuint type;
        GLuint* depth_;
        GLuint* maxDepth_;


    public:
        void Init(vec3 origin, Leaf parent, GLuint type, GLuint depth, GLuint maxDepth) {
            this->origin = origin;
            this->parent =parent;
            this->type = type;
            depth_ = depth;
            maxDepth_ = maxDepth;
        }

        void generateChild(){
            if(&depth_ < &maxDepth_){
                if(type == 0){
                    leftChild = new Leaf(this->origin+vec3(0.25,0.0,0.25), &this, 0, &(depth_+1), &maxDepth_);
                    rightChild = new Leaf(this->origin+vec3(-0.25,0.0,0.25), &this, 1,&(depth_+1), &maxDepth_);
                    leftChild.generateChild();
                    rightChild.generateChild();
                } else {
                    leftChild = new Leaf(this->origin+vec3(0.0,0.0,0.30), &this, 0,&(depth_+1), &maxDepth_);
                    leftChild.generateChild();
                }
            }
        }
};
