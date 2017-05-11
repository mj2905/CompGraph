#pragma once
#include "icg_helper.h"
#include "../mountains_create/mountains_generator.h"
#include "../mountains_render/mountains_render.h"
#include "../framebuffer.h"
#include "../water/water.h"
#include "../perlin_noise/perlin.h"
//#include "../L-tree/algae.h"
//#include "../L-tree/plant.h"
#include "../L-tree/algaemodif.h"
#include <array>

using namespace glm;

class Terrain {

    private:
        MountainsGenerator mountainsCreator;
        MountainsRender mountainsRender;
        FrameBuffer framebuffer;
        Water water;
        //Algae algae;
        PerlinNoise perlin;
        algaeModif algae;

  //      Plant plant;

    public:
        void Init(size_t width, size_t height) {
            /*mountainsCreator.Init();
            GLuint i = framebuffer.Init(width, height, true);
            mountainsRender.Init(framebuffer.getTextureId());
            water.Init(framebuffer.getTextureId());*/
            int id = -1;
            GLuint tex_id = framebuffer.Init(width, height, true);
            algae.Init(1, vec3(0.0,1.0,0.0),'A',&id, tex_id );
            algae.initTree();
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
            framebuffer.ClearContent();
            framebuffer.Bind();
            {
                perlin.Draw(0.0,0.0);
            }
            framebuffer.Unbind();
            algae.Draw(model, view, projection);
        }

        void Cleanup() {
            mountainsCreator.Cleanup();
            mountainsRender.Cleanup();
            water.Cleanup();
            framebuffer.Cleanup();
            //algae.Cleanup();
        }

};
