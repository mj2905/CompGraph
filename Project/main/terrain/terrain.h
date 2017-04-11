#pragma once
#include "icg_helper.h"
#include "../mountains/mountains.h"
#include "../water/water.h"
#include <array>

class Terrain {

    private:
        Mountains mountains;
        Water water;

    public:
        void Init() {
            mountains.Init();
            water.Init();
        }

        void changeTexture(const array<GLuint, 4>& textures) {
            mountains.changeTexture(textures);
        }

        void Draw(float offsetX, float offsetY,
                  const glm::mat4 &model,
                  const glm::mat4 &view,
                  const glm::mat4 &projection) {
            mountains.Draw(offsetX, offsetY, model, view, projection);
            water.Draw(model, view, projection);
        }

        void Cleanup() {
            mountains.Cleanup();
            water.Cleanup();
        }

};
