#pragma once
#include "icg_helper.h"

#include <glm/gtc/type_ptr.hpp>

#define CUBE_FACES 6
#define SKYBOX_BOUNDS 1000.0f

GLfloat skyboxVertices[] = {
  // Positions
  -SKYBOX_BOUNDS,  SKYBOX_BOUNDS, -SKYBOX_BOUNDS,
  -SKYBOX_BOUNDS, -SKYBOX_BOUNDS, -SKYBOX_BOUNDS,
   SKYBOX_BOUNDS, -SKYBOX_BOUNDS, -SKYBOX_BOUNDS,
   SKYBOX_BOUNDS, -SKYBOX_BOUNDS, -SKYBOX_BOUNDS,
   SKYBOX_BOUNDS,  SKYBOX_BOUNDS, -SKYBOX_BOUNDS,
  -SKYBOX_BOUNDS,  SKYBOX_BOUNDS, -SKYBOX_BOUNDS,

  -SKYBOX_BOUNDS, -SKYBOX_BOUNDS,  SKYBOX_BOUNDS,
  -SKYBOX_BOUNDS, -SKYBOX_BOUNDS, -SKYBOX_BOUNDS,
  -SKYBOX_BOUNDS,  SKYBOX_BOUNDS, -SKYBOX_BOUNDS,
  -SKYBOX_BOUNDS,  SKYBOX_BOUNDS, -SKYBOX_BOUNDS,
  -SKYBOX_BOUNDS,  SKYBOX_BOUNDS,  SKYBOX_BOUNDS,
  -SKYBOX_BOUNDS, -SKYBOX_BOUNDS,  SKYBOX_BOUNDS,

   SKYBOX_BOUNDS, -SKYBOX_BOUNDS, -SKYBOX_BOUNDS,
   SKYBOX_BOUNDS, -SKYBOX_BOUNDS,  SKYBOX_BOUNDS,
   SKYBOX_BOUNDS,  SKYBOX_BOUNDS,  SKYBOX_BOUNDS,
   SKYBOX_BOUNDS,  SKYBOX_BOUNDS,  SKYBOX_BOUNDS,
   SKYBOX_BOUNDS,  SKYBOX_BOUNDS, -SKYBOX_BOUNDS,
   SKYBOX_BOUNDS, -SKYBOX_BOUNDS, -SKYBOX_BOUNDS,

  -SKYBOX_BOUNDS, -SKYBOX_BOUNDS,  SKYBOX_BOUNDS,
  -SKYBOX_BOUNDS,  SKYBOX_BOUNDS,  SKYBOX_BOUNDS,
   SKYBOX_BOUNDS,  SKYBOX_BOUNDS,  SKYBOX_BOUNDS,
   SKYBOX_BOUNDS,  SKYBOX_BOUNDS,  SKYBOX_BOUNDS,
   SKYBOX_BOUNDS, -SKYBOX_BOUNDS,  SKYBOX_BOUNDS,
  -SKYBOX_BOUNDS, -SKYBOX_BOUNDS,  SKYBOX_BOUNDS,

  -SKYBOX_BOUNDS,  SKYBOX_BOUNDS, -SKYBOX_BOUNDS,
   SKYBOX_BOUNDS,  SKYBOX_BOUNDS, -SKYBOX_BOUNDS,
   SKYBOX_BOUNDS,  SKYBOX_BOUNDS,  SKYBOX_BOUNDS,
   SKYBOX_BOUNDS,  SKYBOX_BOUNDS,  SKYBOX_BOUNDS,
  -SKYBOX_BOUNDS,  SKYBOX_BOUNDS,  SKYBOX_BOUNDS,
  -SKYBOX_BOUNDS,  SKYBOX_BOUNDS, -SKYBOX_BOUNDS,

  -SKYBOX_BOUNDS, -SKYBOX_BOUNDS, -SKYBOX_BOUNDS,
  -SKYBOX_BOUNDS, -SKYBOX_BOUNDS,  SKYBOX_BOUNDS,
   SKYBOX_BOUNDS, -SKYBOX_BOUNDS, -SKYBOX_BOUNDS,
   SKYBOX_BOUNDS, -SKYBOX_BOUNDS, -SKYBOX_BOUNDS,
  -SKYBOX_BOUNDS, -SKYBOX_BOUNDS,  SKYBOX_BOUNDS,
   SKYBOX_BOUNDS, -SKYBOX_BOUNDS,  SKYBOX_BOUNDS
};



class Skybox {
private:
  GLuint texture_id;
  GLuint skyboxVAO;
  GLuint skyboxVBO;
  GLuint program_id_;
  std::vector<string> skybox_files;
  //skybox from www.zfight.com

public:
  void init(string folder) {
    program_id_ = icg_helper::LoadShaders("skybox_vshader.glsl",
                                          "skybox_fshader.glsl");


    //array and buffer objects
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);


    glGenTextures(1, &texture_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    skybox_files.push_back(folder + "/right.tga");
    skybox_files.push_back(folder + "/left.tga");
    skybox_files.push_back(folder + "/up.tga");
    skybox_files.push_back(folder + "/down.tga");
    skybox_files.push_back(folder + "/back.tga");
    skybox_files.push_back(folder + "/front.tga");

    // skybox textures loading
    // set stb_image to have the same coordinates as OpenGL
    stbi_set_flip_vertically_on_load(1);

    int width,height;
    int nb_component;
    for(GLuint i = 0; i < CUBE_FACES; i++)
    {

      char * imagename;
      imagename = new char [skybox_files[i].length()+1];
      std::strcpy (imagename, skybox_files[i].c_str());


        unsigned char* image;
        std::cout << imagename << std::endl;
        image = stbi_load(imagename, &width,
        &height, &nb_component, 0);

        if(image == nullptr) {
          throw(string("Failed to load all 6 textures for skybox."));
        }
        glTexImage2D(
          GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
          GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
        );
      //  stbi_image_free(image);

      delete[] imagename;
      }
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
      glBindTexture(GL_TEXTURE_CUBE_MAP, 0);




    }

    void draw(const glm::mat4 &view = IDENTITY_MATRIX,
              const glm::mat4 &projection = IDENTITY_MATRIX) {
                glm::mat4 new_view = glm::mat4(glm::mat3(view));
                glUseProgram(program_id_);
                glBindVertexArray(skyboxVAO);
                glDepthMask(GL_FALSE);// RNecessary for proper skybox generation
                glUniformMatrix4fv(glGetUniformLocation(program_id_, "view"), 1, GL_FALSE, glm::value_ptr(new_view));
                glUniformMatrix4fv(glGetUniformLocation(program_id_, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
                glBindVertexArray(skyboxVAO);
                glActiveTexture(GL_TEXTURE0);
                glUniform1i(glGetUniformLocation(program_id_, "skybox"), 0);
                glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                glBindVertexArray(0);
                glDepthMask(GL_TRUE);



              }

  };
