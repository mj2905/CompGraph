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
#include "../shadow/shadow.h"
#include "../shadow/framebuffer_shadow.h"
#include "../constants.h"
#include "../lightScattering/lightscattering.h"

using namespace glm;

class Terrain {

private:
    MountainsGenerator mountainsCreator;
    MountainsRender mountains;
    MountainsRender reflect;
    FrameBuffer framebuffer_terrain;
    FrameBufferReflect framebuffer_reflect;
    FrameBufferShadow framebuffer_shadow;
    Water water;
    Skybox skybox;
    Shadow shadow;
    FrameBufferScattering fboScatter1, fboScatter2;
    ScreenQuad screenQuad;

    const string skyboxTexture = "miramar";

public:
    void Init(size_t width, size_t height, LightSource light) {
        mountainsCreator.Init();
        framebuffer_terrain.Init(width, height, true);
        framebuffer_reflect.Init(width, height, true);

        int frameId1, frameId2;
        frameId1 = fboScatter1.Init(width, height, true);
        frameId2 = fboScatter2.Init(width, height);

        screenQuad.Init(width, height, frameId1, frameId2);


        mountains.Init(framebuffer_terrain.getTextureId(), 1024);
        reflect.Init(framebuffer_terrain.getTextureId(), 128);
        water.Init(framebuffer_terrain.getTextureId(), framebuffer_reflect.getTextureId(), light, 128);
        skybox.init(skyboxTexture);

        framebuffer_shadow.Init(width, height, true);
        shadow.Init(framebuffer_shadow.getTextureId());
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
              const glm::mat4 &projection,
              int drawBlack = 1) {


        mat4 skyboxRot = glm::rotate(IDENTITY_MATRIX, (float)glfwGetTime()/100, vec3(0,1,0));
        fboScatter1.Clear();
        fboScatter2.Clear();

        framebuffer_reflect.ClearContent();
        framebuffer_reflect.Bind();
        mat4 rot = glm::translate(glm::rotate(IDENTITY_MATRIX, (float)M_PI, vec3(1,0,0)), vec3(0, -0.8, 0));
        skybox.Draw(view * glm::scale(IDENTITY_MATRIX, vec3(1, -1, 1)) * skyboxRot, projection);
        reflect.Draw(offsetX, offsetY, true, model * rot, view, projection);
        framebuffer_reflect.Unbind();


        fboScatter1.Bind();{
            mountains.Draw(offsetX, offsetY, false, model, view, projection, 0);
        }
        fboScatter1.Unbind();
        fboScatter2.Bind();{

            skybox.Draw(view * skyboxRot, projection);
            mountains.Draw(offsetX, offsetY, false, model, view, projection, 1);
            water.Draw(offsetX, offsetY, model, view, projection);
        }
        fboScatter2.Unbind();

        screenQuad.Draw();


    }

    void Cleanup() {
        mountainsCreator.Cleanup();
        mountains.Cleanup();
        reflect.Cleanup();
        water.Cleanup();
        framebuffer_terrain.Cleanup();
        framebuffer_reflect.Cleanup();
        skybox.Cleanup();
        framebuffer_shadow.Cleanup();
        shadow.Cleanup();
    }

};
