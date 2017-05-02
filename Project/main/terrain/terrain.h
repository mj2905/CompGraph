#pragma once
#include "icg_helper.h"
#include "../mountains_create/mountains_generator.h"
#include "../mountains_render/mountains_render.h"
#include "../framebuffer.h"
#include "framebuffer_terrain.h"
#include "../water/water.h"
#include "../perlin_noise/perlin.h"
#include <array>

using namespace glm;

class Terrain {

    private:
        MountainsGenerator mountainsCreator;
        MountainsRender mountainsRender;
        FrameBuffer framebuffer;
        FrameBufferTerrain framebuffer2;
        Water water;

    public:
        void Init(size_t width, size_t height) {
            mountainsCreator.Init();
            framebuffer.Init(width, height, true);
            framebuffer2.Init(width, height, true);
            mountainsRender.Init(framebuffer.getTextureId());
            water.Init(framebuffer.getTextureId(), framebuffer2.getTextureId());
        }

        void changeTexture(const array<GLuint, 4>& textures) {
            mountainsCreator.changeTexture(textures);
        }

        void Moved(float offsetX, float offsetY) {
            framebuffer.ClearContent();
            framebuffer.Bind();
                mountainsCreator.Draw(offsetX, offsetY);
            framebuffer.Unbind();
        }

        void Draw(float offsetX, float offsetY,
                  const glm::mat4 &model,
                  const glm::mat4 &view,
                  const glm::mat4 &projection) {
            mountainsRender.Draw(offsetX, offsetY, false, model, view, projection);
            water.Draw(offsetX, offsetY, model, view, projection);
            framebuffer2.ClearContent();
            framebuffer2.Bind();
                mat4 scale = glm::translate(glm::scale(IDENTITY_MATRIX, vec3(1, -1, 1)), vec3(0, -0.8, 0));
                mountainsRender.Draw(offsetX, offsetY, true, model * scale, view, projection);
            framebuffer2.Unbind();
        }

        void Cleanup() {
            mountainsCreator.Cleanup();
            mountainsRender.Cleanup();
            water.Cleanup();
            framebuffer.Cleanup();
            framebuffer2.Cleanup();
        }

};
