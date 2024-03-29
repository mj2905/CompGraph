#pragma once
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Position {

    protected:
        float x, y;
        float limit_left = -1;
        float limit_right = 1;
        float limit_up = 1;
        float limit_down = -1;

    public:
        Position(float x = 0, float y = 0) : x(x), y(y) {}

        void reset(float x = 0, float y = 0) {
            this->x = x;
            this->y = y;
        }

        float under(float limit, float half_height) {
            return y - half_height < limit;
        }

        float bottomY(float half_height) {
            return y - half_height;
        }

        float upY(float half_height) {
            return y + half_height;
        }

        float leftX(float half_width) {
            return x - half_width;
        }

        float rightX(float half_width) {
            return x + half_width;
        }

        virtual void yBounce(float ybottom, float yup, float xleft, float xright, float half_width, float half_height, float& vecy) = 0;

        virtual void moveTo(float half_width, float half_height, float& vecx, float& vecy) = 0;

        virtual ~Position() {}

        static glm::mat4 rotation(float theta) {
            return glm::mat4(
                        cos(theta), sin(theta), 0, 0,
                       -sin(theta), cos(theta), 0, 0,
                        0,          0,          1, 0,
                        0,          0,          0, 1
                        );
        }

        static glm::mat4 translate(const Position& p) {
            return translate(p.x, p.y);
        }

        static glm::mat4 translate(float tx, float ty = 0.0) {
            return glm::mat4(
                        1,  0,  0, 0,
                        0,  1,  0, 0,
                        0,  0,  1, 0,
                        tx,  ty,  0, 1
                        );
        }

        static glm::mat4 scale(float sx, float sy = 1.0) {
            return glm::mat4(
                        sx,  0,  0, 0,
                        0,  sy,  0, 0,
                        0,  0,  1, 0,
                        0,  0,  0, 1
                        );
        }
};
