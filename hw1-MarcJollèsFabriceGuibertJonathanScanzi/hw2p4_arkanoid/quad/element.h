#pragma once
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "position.h"
#include "quad.h"
#include <memory>

class Element {

    protected:
        unique_ptr<Position> position;
        float demi_width, demi_height;
        float vecx;
        float vecy;

        Quad quad;

    public:
        Element(Position* position, float width, float height)
            : position(position), demi_width(width/2.0f), demi_height(height/2.0f) {}

        virtual ~Element()  = 0;
        void otherBounce(Element& other) {
            other.position->yBounce(position->bottomY(demi_height), position->upY(demi_height),
                                    position->leftX(demi_width), position->rightX(demi_width),
                                    other.demi_width, other.demi_height, other.vecy);
        }

        virtual void Init() {
            quad.Init();
            vecx = 0;
            vecy = 0;
        }

        virtual void Cleanup() {
            quad.Cleanup();
        }

        virtual void Draw() {
            quad.Draw(position->translate(*position) * position->scale(demi_width, demi_height));
        }
};

Element::~Element() {}
