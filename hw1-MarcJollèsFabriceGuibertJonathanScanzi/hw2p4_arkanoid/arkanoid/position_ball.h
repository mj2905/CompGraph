#pragma once
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "position.h"

class Position_Ball : public Position {

    public:
        Position_Ball(float x = 0, float y = 0) : Position(x,y) {}


        virtual void yBounce(float ybottom, float yup, float xleft, float xright, float demi_width, float demi_height, float& vecy) {
            float yP = y - demi_height;
            float xPLeft = x - demi_width;
            float xPRight = x + demi_width;

            if(yP >= ybottom && yP <= yup && (xPRight >= xleft && xPLeft <= xright)) {
                y = yup + demi_height;
                vecy = -vecy;
            }
        }

        //Whatever the vecx and vecy are, we have to keep the ball within the field
        virtual void moveTo(float demi_width, float demi_height, float& vecx, float& vecy) {

            x = x + vecx;
            y = y + vecy;
            do {
                float xLeft = x - demi_width;
                if(xLeft < -1) {
                    float newXLeft = -1 + abs(xLeft + 1);
                    x = newXLeft + demi_width;
                    vecx = -vecx;
                }

                float xRight = x + demi_width;
                if(xRight > 1) {
                    float newXRight = 1 - abs(xRight - 1);
                    x = newXRight - demi_width;
                    vecx = -vecx;
                }

                float yDown = y - demi_height;
                if(yDown < -1) {
                    float newYDown = -1 + abs(yDown + 1);
                    y = newYDown + demi_height;
                    vecy = -vecy;
                }

                float yUp = y + demi_height;
                if(yUp > 1) {
                    float newYUp = 1 - abs(yUp - 1);
                    y = newYUp - demi_height;
                    vecy = -vecy;
                }
            }while(x < -1 || x > 1 || y < -1 || y > 1);
        }
};
