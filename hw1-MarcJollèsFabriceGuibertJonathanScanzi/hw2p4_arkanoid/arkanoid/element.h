#pragma once
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "position.h"
#include "../quad/quad.h"
#include <memory>

class Element {

    protected:
        unique_ptr<Position> position;
        float half_width, half_height;
        float vecx;
        float vecy;

        Quad quad;

    public:
        Element(Position* position, float width, float height)
            : position(position), half_width(width/2.0f), half_height(height/2.0f) {}

        virtual ~Element()  = 0;
        void otherBounce(Element& other) {
            other.position->yBounce(position->bottomY(half_height), position->upY(half_height),
                                    position->leftX(half_width), position->rightX(half_width),
                                    other.half_width, other.half_height, other.vecy);
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
            quad.Draw(position->translate(*position) * position->scale(half_width, half_height));
        }
};

Element::~Element() {}
