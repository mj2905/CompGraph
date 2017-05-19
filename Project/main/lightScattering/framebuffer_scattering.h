#pragma once
#include "icg_helper.h"

#define OFF_SCREEN_RENDER_RATIO 2;

class FrameBufferScattering{

private:
    int width_;
    int height_;
    GLuint framebuffer_object_id_;
    GLuint depth_render_buffer_id_;
    GLuint color_texture_id_;

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
        return color_texture_id_;
    }

    void Init(int image_width, int image_height) {

        width_ = image_width/OFF_SCREEN_RENDER_RATIO;
        height_ = image_height/OFF_SCREEN_RENDER_RATIO;

        {
            glGenTextures(1, &color_texture_id_);
            glBindTexture(GL_TEXTURE_2D, color_texture_id_);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap


            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
            //    glBindTexture(GL_TEXTURE_2D, 0);
        }

        // create a renderbuffer object to store depth info

        {
            glGenRenderbuffers(1, &depth_render_buffer_id_);
            glBindRenderbuffer(GL_RENDERBUFFER, depth_render_buffer_id_);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32,width_, height_);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }


        //tie it all together
        {
            glGenFramebuffers(1, &framebuffer_object_id_);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object_id_);
            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                   GL_COLOR_ATTACHMENT0 /*location = 0*/,
                                   GL_TEXTURE_2D, color_texture_id_,
                                   0 /*level*/);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                      GL_RENDERBUFFER, depth_render_buffer_id_);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
                    GL_FRAMEBUFFER_COMPLETE) {
                cerr << "!!!ERROR: Framebuffer not OK :(" << endl;
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }

    void ClearContent() {
        glViewport(0, 0, width_, height_);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object_id_);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glClearColor(0.4294117647f, 0.7078431373f, 0.9803921569f, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Cleanup() {
        glDeleteTextures(1, &color_texture_id_);
        glDeleteRenderbuffers(1, &depth_render_buffer_id_);
        glBindFramebuffer(GL_FRAMEBUFFER, 0 /*UNBIND*/);
        glDeleteFramebuffers(1, &framebuffer_object_id_);
    }
};


