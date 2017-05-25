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

using namespace glm;

class Terrain {

    private:
        MountainsGenerator mountainsCreator;
        MountainsRender mountains;
        MountainsRender reflect;
        FrameBuffer framebuffer_terrain;
        FrameBufferReflect framebuffer_reflect;
        Water water;
        Skybox skybox;
        GLsizei width;
        GLsizei height;

        const string skyboxTexture = "miramar";

    public:
        void Init(size_t width, size_t height) {
            mountainsCreator.Init();
            framebuffer_terrain.Init(width, height, true);
            framebuffer_reflect.Init(width, height, true);

            mountains.Init(framebuffer_terrain.getTextureId(), 1024);
            reflect.Init(framebuffer_terrain.getTextureId(), 128);
            water.Init(framebuffer_terrain.getTextureId(), framebuffer_reflect.getTextureId(), 128);
            skybox.init(skyboxTexture);
            this->width = width;
            this->height = height;
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

        float getCurrentHeight() {
          GLfloat ret = 0.1f;
          framebuffer_terrain.Bind();
        //  glReadPixels(width/2, height/2, 1, 1, GL_RED, GL_FLOAT, &ret);
          glReadPixels(width/2, height/2, 1, 1, GL_RED, GL_FLOAT, &ret);
          framebuffer_terrain.Unbind();

          return ret;
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
                reflect.Draw(offsetX, offsetY, true, model * rot, view, projection);
            framebuffer_reflect.Unbind();

            /*
            framebuffer_shadow.ClearContent();
            framebuffer_shadow.Bind();
                mountains.Draw(offsetX, offsetY, false, model, view, projection);
            framebuffer_shadow.Unbind();
            */

            skybox.Draw(view * skyboxRot, projection);
            mountains.Draw(offsetX, offsetY, false, model, view, projection);
            water.Draw(offsetX, offsetY, model, view, projection);

            //shadow.Draw(model, view, projection);
        }

        void Cleanup() {
            mountainsCreator.Cleanup();
            mountains.Cleanup();
            reflect.Cleanup();
            water.Cleanup();
            framebuffer_terrain.Cleanup();
            framebuffer_reflect.Cleanup();
            skybox.Cleanup();
        }

};
