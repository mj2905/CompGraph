#pragma once
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "position.h"

class Position_Paddle : public Position {

    public:
        Position_Paddle(float x = 0, float y = 0) : Position(x,y) {}

        virtual void yBounce(float ybottom, float yup, float xleft, float xright, float demi_width, float demi_height, float& vecy) {}

        virtual void moveTo(float demi_width, float demi_height, float& vecx, float& vecy) {
            x = x + vecx;
            if(x - demi_width < -1) {
                x = -1 + demi_width;
            }

            if(x + demi_width > 1) {
                x = 1 - demi_width;
            }
        }
};
