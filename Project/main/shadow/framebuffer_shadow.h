#pragma once
#include "icg_helper.h"

class FrameBufferShadow {

    private:
        int width_;
        int height_;
        GLuint framebuffer_object_id_;
        GLuint depth_render_buffer_id_;

    public:

        // warning: overrides viewport!!
        void Bind() {
            glViewport(0, 0, width_, height_);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object_id_);
            const GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
            glDrawBuffers(1 /*length of buffers[]*/, buffers);
        }

        void Unbind() {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        GLuint getTextureId() {
            return depth_render_buffer_id_;
        }

        void Init(int image_width, int image_height,
                                  bool use_interpolation = false) {
            this->width_ = image_width;
            this->height_ = image_height;

            // tie it all together
            {
                glGenFramebuffers(1, &framebuffer_object_id_);
                glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object_id_);

                glGenTextures(1, &depth_render_buffer_id_);
                glBindTexture(GL_TEXTURE_2D, depth_render_buffer_id_);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, image_width, image_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_render_buffer_id_, 0);
                glDrawBuffer(GL_NONE);

                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
                    GL_FRAMEBUFFER_COMPLETE) {
                    cerr << "!!!ERROR: Framebuffer not OK :(" << endl;
                }
                glBindFramebuffer(GL_FRAMEBUFFER, 0); // avoid pollution
            }
        }

        void ClearContent() {
            glViewport(0, 0, width_, height_);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object_id_);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void Cleanup() {
            glDeleteRenderbuffers(1, &depth_render_buffer_id_);
            glBindFramebuffer(GL_FRAMEBUFFER, 0 /*UNBIND*/);
            glDeleteFramebuffers(1, &framebuffer_object_id_);
        }
};
