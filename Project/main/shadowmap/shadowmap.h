#pragma once

class shadowmap {

    private:
       GLuint program_id_;
      // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
       GLuint FramebufferName = 0;
       GLuint depthTexture;
       GLuint MVP_id;
       const GLuint SHADOW_MAP_RES = 2048;
       GLsizei orig_width;
       GLsizei orig_height;



    public:

      GLuint init(GLsizei orig_width, GLsizei orig_height) {
        this->orig_width = orig_width;
        this->orig_height = orig_height;


        // compile the shaders.
        program_id_ = icg_helper::LoadShaders("shadowmaps_vshader.glsl",
                                              "shadowmaps_fshader.glsl");

         glGenFramebuffers(1, &FramebufferName);
         glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

         // Depth texture. Slower than a depth buffer, but you can sample it later in your shader

         glGenTextures(1, &depthTexture);
         glBindTexture(GL_TEXTURE_2D, depthTexture);
         glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, SHADOW_MAP_RES, SHADOW_MAP_RES, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

         glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

         MVP_id = glGetUniformLocation(program_id_, "MVP");

         glDrawBuffer(GL_NONE); // No color buffer is drawn to.

         // Always check that our framebuffer is ok
         if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
           fprintf(stderr, "%s\n", "Error: could not initialise shadow map.");
           return 0;
         }

         glBindFramebuffer(GL_FRAMEBUFFER, 0);

         return FramebufferName;

      }

void bind(const glm::mat4 &model = IDENTITY_MATRIX,
          const glm::mat4 &view = IDENTITY_MATRIX,
          const glm::mat4 &projection = IDENTITY_MATRIX,
          const glm::mat4 &MVP = IDENTITY_MATRIX) {


  glUseProgram(program_id_);

  glViewport(0, 0, SHADOW_MAP_RES, SHADOW_MAP_RES);
  glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
  glClear(GL_DEPTH_BUFFER_BIT);
  glUniformMatrix4fv(MVP_id, ONE, DONT_TRANSPOSE, glm::value_ptr(MVP));
}

void unbind() {
  glUseProgram(0);
  glViewport(0, 0, orig_width, orig_height);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Draw() {




    glUseProgram(program_id_);






    // setup MVP


    // draw
    // TODO 5: for debugging it can be helpful to draw only the wireframe.
    // You can do that by uncommenting the next line.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // TODO 5: depending on how you set up your vertex index buffer, you
    // might have to change GL_TRIANGLE_STRIP to GL_TRIANGLES.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);




}


};
