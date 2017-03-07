#pragma once
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "element.h"
#include "position_paddle.h"

class Paddle : public Element {

    private:
        const float step = 0.1f;


    public:
        //0.04f
        Paddle(float width, float height) : Element(new Position_Paddle(0, -0.7f), width, height) {}

        void moveRight() {
            vecx = step, vecy = 0;
            position->moveTo(demi_width, demi_height, vecx, vecy);
        }

        void moveLeft() {
            vecx = -step, vecy = 0;
            position->moveTo(demi_width, demi_height, vecx, vecy);
        }
};
