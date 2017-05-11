#pragma once
#include "icg_helper.h"
#include "../mountains_create/mountains_generator.h"
#include "../mountains_render/mountains_render.h"
#include "../framebuffer.h"
#include "framebuffer_reflect.h"
#include "../water/water.h"
#include "../perlin_noise/perlin.h"
#include "../skybox/skybox.h"
#include <array>
#include "../water/screenquad/gaussian_blur.h"
#include "../shadow/shadow.h"
#include "../shadow/framebuffer_shadow.h"

using namespace glm;

class Terrain {

    private:
        MountainsGenerator mountainsCreator;
        MountainsRender mountainsRender;
        FrameBuffer framebuffer_terrain;
        FrameBufferReflect framebuffer_reflect;
        FrameBufferShadow framebuffer_shadow;
        Water water;
        Skybox skybox;
        GaussianBlur blur;
        Shadow shadow;

        const string skyboxTexture = "miramar";

    public:
        void Init(size_t width, size_t height) {
            mountainsCreator.Init();
            framebuffer_terrain.Init(width, height, true);
            framebuffer_reflect.Init(width, height, true);
            mountainsRender.Init(framebuffer_terrain.getTextureId());
            blur.Init(width, height, framebuffer_reflect.getTextureId());
            water.Init(framebuffer_terrain.getTextureId(), blur.getTexture());
            skybox.init(skyboxTexture);

            framebuffer_shadow.Init(width, height, true);
            shadow.Init(framebuffer_shadow.getTextureId());
        }

        void resize(size_t width, size_t height) {

        }

        void changeTexture(const array<GLuint, 4>& textures) {
            mountainsCreator.changeTexture(textures);
        }

        void Moved(float offsetX, float offsetY) {
            framebuffer_terrain.ClearContent();
            framebuffer_terrain.Bind();
                mountainsCreator.Draw(offsetX, offsetY);
            framebuffer_terrain.Unbind();
        }

        void Draw(float offsetX, float offsetY,
                  const glm::mat4 &model,
                  const glm::mat4 &view,
                  const glm::mat4 &projection) {


            mat4 skyboxRot = glm::rotate(IDENTITY_MATRIX, (float)glfwGetTime()/100, vec3(0,1,0));

            framebuffer_reflect.ClearContent();
            framebuffer_reflect.Bind();
                mat4 rot = glm::translate(glm::rotate(IDENTITY_MATRIX, (float)M_PI, vec3(1,0,0)), vec3(0, -0.8, 0));
                skybox.Draw(view * glm::scale(IDENTITY_MATRIX, vec3(1, -1, 1)) * skyboxRot, projection);
                mountainsRender.Draw(offsetX, offsetY, true, model * rot, view, projection);
            framebuffer_reflect.Unbind();

            framebuffer_shadow.ClearContent();
            framebuffer_shadow.Bind();
                mountainsRender.Draw(offsetX, offsetY, false, model, view, projection);
            framebuffer_shadow.Unbind();

            blur.Draw();

            skybox.Draw(view * skyboxRot, projection);
            mountainsRender.Draw(offsetX, offsetY, false, model, view, projection);
            water.Draw(offsetX, offsetY, model, view, projection);

            //shadow.Draw(model, view, projection);
        }

        void Cleanup() {
            mountainsCreator.Cleanup();
            mountainsRender.Cleanup();
            water.Cleanup();
            framebuffer_terrain.Cleanup();
            framebuffer_reflect.Cleanup();
            skybox.Cleanup();
            framebuffer_shadow.Cleanup();
            shadow.Cleanup();
        }

};
