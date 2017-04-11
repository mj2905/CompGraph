#pragma once
#include "icg_helper.h"
#include "../mountains_create/mountains_generator.h"
#include "../mountains_render/mountains_render.h"
#include "../framebuffer.h"
#include "../water/water.h"
#include "../perlin_noise/perlin.h"
#include <array>

class Terrain {

    private:
        MountainsGenerator mountainsCreator;
        MountainsRender mountainsRender;
        FrameBuffer framebuffer;
        Water water;

    public:
        void Init(size_t width, size_t height) {
            mountainsCreator.Init();
            framebuffer.Init(width, height, true);
            mountainsRender.Init(framebuffer.getTextureId());
            water.Init();
        }

        void changeTexture(const array<GLuint, 4>& textures) {
            mountainsCreator.changeTexture(textures);
        }

        void Draw(float offsetX, float offsetY,
                  const glm::mat4 &model,
                  const glm::mat4 &view,
                  const glm::mat4 &projection) {
            framebuffer.ClearContent();
            framebuffer.Bind();
                mountainsCreator.Draw(offsetX, offsetY);
            framebuffer.Unbind();

            mountainsRender.Draw(model, view, projection);
            water.Draw(model, view, projection);
        }

        void Cleanup() {
            mountainsCreator.Cleanup();
            mountainsRender.Cleanup();
            water.Cleanup();
            framebuffer.Cleanup();
        }

};
