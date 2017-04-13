#pragma once
#include "icg_helper.h"

#include <glm/gtc/type_ptr.hpp>

#define CUBE_FACES 6
#define SKYBOX_BOUNDS 1.0f

GLfloat skyboxVertices[] = {
  // Positions
  -1.0f,  1.0f, -1.0f,
  -1.0f, -1.0f, -1.0f,
  1.0f, -1.0f, -1.0f,
  1.0f, -1.0f, -1.0f,
  1.0f,  1.0f, -1.0f,
  -1.0f,  1.0f, -1.0f,

  -1.0f, -1.0f,  1.0f,
  -1.0f, -1.0f, -1.0f,
  -1.0f,  1.0f, -1.0f,
  -1.0f,  1.0f, -1.0f,
  -1.0f,  1.0f,  1.0f,
  -1.0f, -1.0f,  1.0f,

  1.0f, -1.0f, -1.0f,
  1.0f, -1.0f,  1.0f,
  1.0f,  1.0f,  1.0f,
  1.0f,  1.0f,  1.0f,
  1.0f,  1.0f, -1.0f,
  1.0f, -1.0f, -1.0f,

  -1.0f, -1.0f,  1.0f,
  -1.0f,  1.0f,  1.0f,
  1.0f,  1.0f,  1.0f,
  1.0f,  1.0f,  1.0f,
  1.0f, -1.0f,  1.0f,
  -1.0f, -1.0f,  1.0f,

  -1.0f,  1.0f, -1.0f,
  1.0f,  1.0f, -1.0f,
  1.0f,  1.0f,  1.0f,
  1.0f,  1.0f,  1.0f,
  -1.0f,  1.0f,  1.0f,
  -1.0f,  1.0f, -1.0f,

  -1.0f, -1.0f, -1.0f,
  -1.0f, -1.0f,  1.0f,
  1.0f, -1.0f, -1.0f,
  1.0f, -1.0f, -1.0f,
  -1.0f, -1.0f,  1.0f,
  1.0f, -1.0f,  1.0f
};



class Skybox {
private:
  GLuint skyboxVAO;
  GLuint skyboxVBO;
  GLuint program_id_;
  GLuint projection_id;
  GLuint view_id;
  GLuint tex_id;
  std::vector<string> skybox_files;
  //skybox from www.zfight.com

public:
  void init(string folder) {
    program_id_ = icg_helper::LoadShaders("skybox_vshader.glsl",
    "skybox_fshader.glsl");

    if(!program_id_) {
      exit(EXIT_FAILURE);
    }

    glUseProgram(program_id_);


    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);

    /*
    //array and buffer objects
    glGenVertexArrays(1, &skyboxVAO);
    glBindVertexArray(skyboxVAO);


    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), ZERO_BUFFER_OFFSET);


    glGenBuffers(1, &skyboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex_id);
    */


    // ------- skybox textures loading --------

    skybox_files.push_back(folder + "/right.tga");
    skybox_files.push_back(folder + "/left.tga");
    skybox_files.push_back(folder + "/up.tga");
    skybox_files.push_back(folder + "/down.tga");
    skybox_files.push_back(folder + "/back.tga");
    skybox_files.push_back(folder + "/front.tga");


    // set stb_image to have the same coordinates as OpenGL
    //stbi_set_flip_vertically_on_load(1);

    int width,height;
    int nb_component;
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex_id);
    for(GLuint i = 0; i < CUBE_FACES; i++)
    {

      string& file = skybox_files[i];


      char * imagename;
      imagename = new char [file.length()+1];
      std::strcpy(imagename, file.c_str());

      unsigned char* image;
      image = stbi_load(imagename, &width,
        &height, &nb_component, 0);

        if(image == nullptr) {
          throw(string("Failed to load all 6 textures for skybox."));
        }


        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        stbi_image_free(image);

        delete[] imagename;
      }
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
      glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

      projection_id = glGetUniformLocation(program_id_, "projection");
      view_id = glGetUniformLocation(program_id_, "view");
      glBindVertexArray(0);
      glUseProgram(0);

    }

    void draw(const glm::mat4 &view = IDENTITY_MATRIX,
      const glm::mat4 &projection = IDENTITY_MATRIX) {
        glDisable(GL_DEPTH_TEST);
        //glDepthFunc(GL_LEQUAL);
        //glDepthMask(GL_FALSE);// RNecessary for proper skybox generation
        glm::mat4 new_view = glm::mat4(glm::mat3(view));
        //new_view = view;
        glUseProgram(program_id_);
        glBindVertexArray(skyboxVAO);
        //bind texture
        glActiveTexture(GL_TEXTURE0);

        //setup VP (no model in skybox)
        glUniformMatrix4fv(view_id, 1, GL_FALSE, glm::value_ptr(new_view));
        glUniformMatrix4fv(projection_id, 1, GL_FALSE, glm::value_ptr(projection));

        glBindTexture(GL_TEXTURE_CUBE_MAP, tex_id);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glUniform1i(tex_id, 0);
        glEnable(GL_DEPTH_TEST);
        //cleanup
        glBindVertexArray(0);
        glUseProgram(0);
        //glDepthMask(GL_TRUE);
        //glDepthFunc(GL_LESS);
      }

    };
