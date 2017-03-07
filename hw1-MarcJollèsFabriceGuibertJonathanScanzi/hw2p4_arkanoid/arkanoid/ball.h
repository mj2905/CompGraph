#pragma once
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "element.h"
#include "position_ball.h"

class Ball : public Element {

    private:
        const float step = 0.02f;
        bool switchBallDir = false;

        void initVec() {
            vecx = (switchBallDir ? -1 : 1) * step/2.0f; //initial x direction of ball
            vecy = step/1.5f; //initial y direction of ball
            switchBallDir = !switchBallDir;
        }

    public:

        Ball(float diameter) : Element(new Position_Ball(0, 0), diameter, diameter) {}

        bool under(float limit) {
            return position->under(limit, demi_height);
        }

        void Reset() {
            position->reset();
            initVec();
        }

        virtual void Init() {
            quad.Init(Quad::Circle);
            initVec();
        }

        void Update() {
            position->moveTo(demi_width, demi_height, vecx, vecy);
            if(abs(vecx) < 0.04f && abs(vecy) < 0.04f) {
                vecx *= 1.0001;
                vecy *= 1.0001;
            }
        }
};
