#pragma once

#include "../constants.h"
#include "../globals.h"
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>

class shadowmap {

    private:
       GLuint program_id_;
      // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
       GLuint framebuffer_object_id_ ;
       GLuint depthTexture;
       GLuint MVP_id;
       GLuint VP_id;
       GLuint M_id;

    public:

      GLuint init() {

        // compile the shaders.
        program_id_ = icg_helper::LoadShaders("shadowmaps_vshader.glsl",
                                              "shadowmaps_fshader.glsl");

         glGenFramebuffers(1, &framebuffer_object_id_);
         glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object_id_);

         // Depth texture. Slower than a depth buffer, but you can sample it later in your shader

         glGenTextures(1, &depthTexture);
         glBindTexture(GL_TEXTURE_2D, depthTexture);
         glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_RES, SHADOW_MAP_RES, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

         glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

         M_id = glGetUniformLocation(program_id_, "M");
         VP_id = glGetUniformLocation(program_id_, "VP");
         MVP_id = glGetUniformLocation(program_id_, "MVP");

         glDrawBuffer(GL_NONE); // No color buffer is drawn to.
         glReadBuffer(GL_NONE); // No color buffer is drawn to.

         // Always check that our framebuffer is ok
         if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
           fprintf(stderr, "%s\n", "Error: could not initialise shadow map.");
           glBindFramebuffer(GL_FRAMEBUFFER, 0);
           return 0;
         }

         glm::mat4 VP = shadow_projection * shadow_view;
         glUniformMatrix4fv(VP_id, ONE, DONT_TRANSPOSE, glm::value_ptr(VP));

         glBindFramebuffer(GL_FRAMEBUFFER, 0);

         shadowmap_fb = framebuffer_object_id_;

         return framebuffer_object_id_;

      }

GLuint get_shadowmap_id() {
  return depthTexture;
}
void bind(const glm::mat4 &model = glm::mat4(1.0)) {

  in_shadowmap = true;

  glUseProgram(program_id_);
  glViewport(0, 0, SHADOW_MAP_RES, SHADOW_MAP_RES);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object_id_);
  glClear(GL_DEPTH_BUFFER_BIT);
  glBindTexture(GL_TEXTURE_2D, depthTexture);

  glm::mat4 MVP = shadow_projection * shadow_view * model;

  glUniformMatrix4fv(M_id, ONE, DONT_TRANSPOSE, glm::value_ptr(model));

  glUniformMatrix4fv(MVP_id, ONE, DONT_TRANSPOSE, glm::value_ptr(MVP));

  glClear(GL_DEPTH_BUFFER_BIT);

  glDrawBuffers(0, NULL);

}

void unbind() {
  in_shadowmap = false;

  glUseProgram(0);
  glViewport(0, 0, window_width, window_height);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}



};
