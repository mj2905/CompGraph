#pragma once
#include "icg_helper.h"
#include "../mountains_create/mountains_generator.h"
#include "../mountains_render/mountains_render.h"
#include "../framebuffer.h"
#include "framebuffer_terrain.h"
#include "../water/water.h"
#include "../perlin_noise/perlin.h"
#include "../skybox/skybox.h"
#include <array>

using namespace glm;

class Terrain {

    private:
        MountainsGenerator mountainsCreator;
        MountainsRender mountainsRender;
        FrameBuffer framebuffer;
        FrameBufferTerrain framebuffer2;
        Water water;
        Skybox skybox;

        const string skyboxTexture = "miramar";

    public:
        void Init(size_t width, size_t height) {
            mountainsCreator.Init();
            framebuffer.Init(width, height, true);
            framebuffer2.Init(width, height, true);
            mountainsRender.Init(framebuffer.getTextureId());
            water.Init(framebuffer.getTextureId(), framebuffer2.getTextureId());
            skybox.init(skyboxTexture);
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

            framebuffer2.ClearContent();
            framebuffer2.Bind();
                mat4 rot = glm::translate(glm::rotate(IDENTITY_MATRIX, (float)M_PI, vec3(1,0,0)), vec3(0, -0.8, 0));
                skybox.Draw(view * glm::scale(IDENTITY_MATRIX, vec3(1, -1, 1)), projection);
                mountainsRender.Draw(offsetX, offsetY, true, model * rot, view, projection);
            framebuffer2.Unbind();

            skybox.Draw(view, projection);
            mountainsRender.Draw(offsetX, offsetY, false, model, view, projection);
            water.Draw(offsetX, offsetY, model, view, projection);
        }

        void Cleanup() {
            mountainsCreator.Cleanup();
            mountainsRender.Cleanup();
            water.Cleanup();
            framebuffer.Cleanup();
            framebuffer2.Cleanup();
            skybox.Cleanup();
        }

};
