#pragma once
#include "icg_helper.h"
#include "../mountains_create/mountains_generator.h"
#include "../mountains_render/mountains_render.h"
#include "../framebuffer.h"
#include "../water/water.h"
#include "../perlin_noise/perlin.h"
#include "../L-tree/algae.h"
#include <array>

using namespace glm;

class Terrain {

    private:
        MountainsGenerator mountainsCreator;
        MountainsRender mountainsRender;
        FrameBuffer framebuffer;
        Water water;
        Algae algae;

    public:
        void Init(size_t width, size_t height) {
            mountainsCreator.Init();
            framebuffer.Init(width, height, true);
            mountainsRender.Init(framebuffer.getTextureId());
            water.Init(framebuffer.getTextureId());
            algae.Init(0, 'A', vec3(0,0.1,0));
            algae.printTree();
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

        void Draw(const glm::mat4 &model,
                  const glm::mat4 &view,
                  const glm::mat4 &projection) {
            //mountainsRender.Draw(model, view, projection);
            //water.Draw(model, view, projection);
            algae.Draw(model, view, projection);
        }

        void Cleanup() {
            mountainsCreator.Cleanup();
            mountainsRender.Cleanup();
            water.Cleanup();
            framebuffer.Cleanup();
            algae.Cleanup();
        }

};
