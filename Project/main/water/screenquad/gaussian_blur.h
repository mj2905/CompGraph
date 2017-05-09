#pragma once
#include "icg_helper.h"

#include <vector>
#include "screenquad.h"
#include "framebuffer.h"

class GaussianBlur {

    private:
        ScreenQuad quadHorizontal, quadVertical;
        FrameBufferGaussian blur;
        GLuint vertical, texture;

    public:

        void Init(float screenquad_width, float screenquad_height, GLuint texture) {
            GLuint horizontal;
            std::tie(horizontal, vertical) = blur.Init(screenquad_width, screenquad_height, true);
            quadHorizontal.Init(screenquad_width, screenquad_height, texture, true);
            quadVertical.Init(screenquad_width, screenquad_height, horizontal, false);
        }

        void Clear() {
            blur.Clear();
        }

        GLuint getTexture() {
            return vertical;
        }

        void Cleanup() {
            quadHorizontal.Cleanup();
            quadVertical.Cleanup();
            blur.Cleanup();
        }

        void UpdateSize(int screenquad_width, int screenquad_height) {
            quadHorizontal.UpdateSize(screenquad_width, screenquad_height);
            quadVertical.UpdateSize(screenquad_width, screenquad_height);
            blur.Cleanup();
            blur.Init(screenquad_width, screenquad_height, true);
        }

        void Draw() {
            blur.Clear();
            blur.Bind(GL_COLOR_ATTACHMENT0);
                quadHorizontal.Draw();
            blur.Unbind();
            blur.RenderToWindow();
            blur.Bind(GL_COLOR_ATTACHMENT1);
                quadVertical.Draw();
            blur.Unbind();
        }
};
