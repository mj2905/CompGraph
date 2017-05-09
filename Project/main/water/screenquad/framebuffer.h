#pragma once
#include "icg_helper.h"

class FrameBufferGaussian {

    private:
        int width_;
        int height_;
        GLuint framebuffer_object_id_;
        GLuint depth_render_buffer_id_;
        GLuint color_texture_id_;
        GLuint color_texture_id_2_;

    public:
        // warning: overrides viewport!!
        void Bind(GLuint attachment) {
            glViewport(0, 0, width_, height_);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object_id_);
            const GLenum buffers[] = { attachment };
            glDrawBuffers(1 /*length of buffers[]*/, buffers);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        void Unbind() {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void RenderToWindow() {
            glViewport(0, 0, width_, height_);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        void Clear() {
            glViewport(0, 0, width_, height_);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object_id_);
                glDrawBuffer(GL_COLOR_ATTACHMENT0);
                glClearColor(1.0, 1.0, 1.0, 1.0);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glDrawBuffer(GL_COLOR_ATTACHMENT1);
                glClearColor(1.0, 1.0, 1.0, 1.0);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        std::tuple<int, int> Init(int image_width, int image_height, bool use_interpolation = false) {
            this->width_ = image_width;
            this->height_ = image_height;

            // create color attachment
            {
                glGenTextures(1, &color_texture_id_);
                glBindTexture(GL_TEXTURE_2D, color_texture_id_);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                if(use_interpolation){
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                } else {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                }

                // create texture for the color attachment
                // see Table.2 on
                // khronos.org/opengles/sdk/docs/man3/docbook4/xhtml/glTexImage2D.xml
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width_, height_, 0,
                             GL_RGB, GL_UNSIGNED_BYTE, NULL);
                // how to load from buffer
            }

            // create color attachment
            {
                glGenTextures(1, &color_texture_id_2_);
                glBindTexture(GL_TEXTURE_2D, color_texture_id_2_);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                if(use_interpolation){
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                } else {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                }

                // create texture for the color attachment
                // see Table.2 on
                // khronos.org/opengles/sdk/docs/man3/docbook4/xhtml/glTexImage2D.xml
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width_, height_, 0,
                             GL_RGB, GL_UNSIGNED_BYTE, NULL);
                // how to load from buffer
            }

            // create render buffer (for depth channel)
            {
                glGenRenderbuffers(1, &depth_render_buffer_id_);
                glBindRenderbuffer(GL_RENDERBUFFER, depth_render_buffer_id_);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width_, height_);
                glBindRenderbuffer(GL_RENDERBUFFER, 0);
            }

            // tie it all together
            {
                glGenFramebuffers(1, &framebuffer_object_id_);
                glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object_id_);
                glFramebufferTexture2D(GL_FRAMEBUFFER,
                                       GL_COLOR_ATTACHMENT0 /*location = 0*/,
                                       GL_TEXTURE_2D, color_texture_id_,
                                       0 /*level*/);
                glFramebufferTexture2D(GL_FRAMEBUFFER,
                                       GL_COLOR_ATTACHMENT1 /*location = 0*/,
                                       GL_TEXTURE_2D, color_texture_id_2_,
                                       0 /*level*/);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                          GL_RENDERBUFFER, depth_render_buffer_id_);

                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
                    GL_FRAMEBUFFER_COMPLETE) {
                    cerr << "!!!ERROR: Framebuffer not OK :(" << endl;
                }
                glBindFramebuffer(GL_FRAMEBUFFER, 0); // avoid pollution
            }

            return std::tie(color_texture_id_, color_texture_id_2_);
        }

        void Cleanup() {
            glDeleteTextures(1, &color_texture_id_);
            glDeleteTextures(1, &color_texture_id_2_);
            glDeleteRenderbuffers(1, &depth_render_buffer_id_);
            glBindFramebuffer(GL_FRAMEBUFFER, 0 /*UNBIND*/);
            glDeleteFramebuffers(1, &framebuffer_object_id_);
        }
};