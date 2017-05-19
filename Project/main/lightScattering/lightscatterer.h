#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../constants.h"
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>
#include <array>
#include "../framebuffer.h"
#include "../multitiles/multitiles.h"
#include "framebuffer_scattering.h"


using namespace glm;
using namespace std;

class LightScatterer{

private:
    GLuint vertex_array_id_;                // vertex array object
    GLuint vertex_buffer_object_position_;  // memory buffer for positions
    GLuint vertex_buffer_object_index_;     // memory buffer for indices
    GLuint program_id_;                     // GLSL shader program ID
    unsigned int glsl_loc_light;
    unsigned int glsl_loc_exposure;
    unsigned int glsl_loc_decay;
    unsigned int glsl_loc_density;
    unsigned int glsl_loc_weight;
    unsigned int glsl_loc_myTexture;
    int renderWidth, renderHeight;
    float uniformExposure;
    float uniformDecay;
    float uniformDensity;
    float uniformWeight;
    //FrameBuffer frameBuffer;
    FrameBufferScattering frameBufferScattering;

    int window_width = 800;
    int window_height = 600;
    bool fboUsed;

    GLuint fboTexId, fboId;

    float uniformLightX ;
    float uniformLightY ;

    GLuint screenCopyTextureId;
    GLuint light_pos_id;

    //glm::vec3 light_pos;
    LightSource light;

    GLuint num_indices_;                    // number of vertices to render
    GLuint M_id_;                           // model matrix ID
    GLuint V_id_;                           // view matrix ID
    GLuint P_id_;                           // proj matrix ID

public:


    void Init() {
        // compile the shaders.
        program_id_ = icg_helper::LoadShaders("scattering_vshader.glsl",
                                              "scattering_fshader.glsl");
        if(!program_id_) {
            exit(EXIT_FAILURE);
        }

        uniformExposure = 0.0034f;
        uniformDecay = 1.0f;
        uniformDensity = 0.84f;
        uniformWeight = 5.65f;


        glUseProgram(program_id_);

        frameBufferScattering.Init(window_width, window_height);

        // vertex one vertex array
        glGenVertexArrays(1, &vertex_array_id_);
        glBindVertexArray(vertex_array_id_);


        // vertex coordinates and indices
        {
            // position shader attribute
            GLuint loc_position = glGetAttribLocation(program_id_, "vtexcoord");
            glEnableVertexAttribArray(loc_position);
        }

        // lights and shading
        {
            light.Init(0.0,1.0,-1.0);
            glm::vec3 Ld = glm::vec3(1.0f, 1.0f, 0.8f);
            GLuint Ld_id = glGetUniformLocation(program_id_, "Ld");
            glm::vec3 kd = glm::vec3(0.3f);
            GLuint kd_id = glGetUniformLocation(program_id_, "kd");
            glUniform3fv(Ld_id, 1, glm::value_ptr(Ld));
            glUniform3fv(kd_id, ONE, glm::value_ptr(kd));
            GLuint fog_threshold_id = glGetUniformLocation(program_id_, "fog_threshold");
            glUniform1f(fog_threshold_id, FOG_THRESHOLD);
        }

        glsl_loc_light = glGetUniformLocationARB(program_id_,"lightPositionOnScreen");
        glUniform2fARB(glsl_loc_light,uniformLightX,uniformLightY);

        glsl_loc_exposure = glGetUniformLocationARB(program_id_,"exposure");
        glUniform1fARB(glsl_loc_exposure,uniformExposure);

        glsl_loc_decay = glGetUniformLocationARB(program_id_,"decay");
        glUniform1fARB(glsl_loc_decay,uniformDecay);

        glsl_loc_density = glGetUniformLocationARB(program_id_,"density");
        glUniform1fARB(glsl_loc_density,uniformDensity);

        glsl_loc_weight = glGetUniformLocationARB(program_id_,"weight");
        glUniform1fARB(glsl_loc_weight,uniformWeight);

        glsl_loc_myTexture = glGetUniformLocationARB(program_id_,"tex");
        glUniform1iARB(glsl_loc_myTexture,screenCopyTextureId);


        // other uniforms
        M_id_ = glGetUniformLocation(program_id_, "model");
        V_id_ = glGetUniformLocation(program_id_, "view");
        P_id_ = glGetUniformLocation(program_id_, "projection");


        // to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
    }

    FrameBufferScattering getFBO(){
        return frameBufferScattering;
    }

    void putFBOToTexture(){
        glBindTexture(GL_TEXTURE_2D,screenCopyTextureId);
        glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0,renderWidth,renderHeight);
    }

    void ScatterWithMethod(GLFWwindow* window,
                           mat4 quad_model_matrix, mat4 projection_matrix,
                           mat4 mat, mat4 getView, mat4 projMatrix){

        /*glm::vec3 rot_light_pos =  glm::mat3(glm::rotate(IDENTITY_MATRIX, (float)glfwGetTime()/100-1, glm::vec3(0,1,0))) * light.getPosition();

        glUniform3fv(light_pos_id, 1, glm::value_ptr(rot_light_pos));*/



        int renderWidth, renderHeight;
        double modelView[16]={quad_model_matrix[0][0], quad_model_matrix[0][1], quad_model_matrix[0][2],quad_model_matrix[0][3],
                              quad_model_matrix[1][0], quad_model_matrix[1][1], quad_model_matrix[1][2],quad_model_matrix[1][3],
                              quad_model_matrix[2][0], quad_model_matrix[2][1], quad_model_matrix[2][2],quad_model_matrix[2][3],
                              quad_model_matrix[3][0], quad_model_matrix[3][1], quad_model_matrix[3][2],quad_model_matrix[3][3]};

        double projection[16] = {
            projection_matrix[0][0], projection_matrix[0][1], projection_matrix[0][2], projection_matrix[0][3],
            projection_matrix[1][0], projection_matrix[1][1], projection_matrix[1][2], projection_matrix[1][3],
            projection_matrix[2][0], projection_matrix[2][1], projection_matrix[2][2], projection_matrix[2][3],
            projection_matrix[3][0], projection_matrix[3][1], projection_matrix[3][2], projection_matrix[3][3],
        };

        GLint viewport[4];
        double depthRange[2];
        glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
        glGetDoublev(GL_PROJECTION_MATRIX, projection);
        glGetIntegerv(GL_VIEWPORT, viewport);
        glGetDoublev(GL_DEPTH_RANGE, depthRange);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glfwGetFramebufferSize(window,&renderWidth, &renderHeight);


        // render light + obstruction
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);
        glClear ( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
        glColor4f(1,1,1,1);
        glViewport(0,0,window_width/*/OFF_SCREEN_RENDER_RATIO*/,window_height/*/OFF_SCREEN_RENDER_RATIO*/);
        glDisable(GL_TEXTURE_2D);
        glPushMatrix();
        //glTranslatef(light->lightPosition[0],light->lightPosition[1],light->lightPosition[2]);
        glTranslatef(light.getPosition().x, light.getPosition().y, light.getPosition().z);
        light.drawLight();
        //light->render();
        glPopMatrix();
        GLdouble winX=0, winY =0, winZ = 0;

        gluProject(light.getPosition().x, light.getPosition().y, light.getPosition().z,
                   modelView,
                   projection,
                   viewport,
                   &winX,
                   &winY,
                   &winZ);


        uniformLightX = winX/ float(renderWidth/2.0/*OFF_SCREEN_RENDER_RATIO*/) ;
        uniformLightY = winY/ float(renderHeight/2.0/*OFF_SCREEN_RENDER_RATIO*/)  ;


        // Draw occluding source black with light
        //glUseProgramObjectARB(0);
        glUseProgram(0);

        glEnable(GL_TEXTURE_2D);
        glColor4f(0,0,0,1);

    }

    void DrawSphere(){
        light.drawLight();
    }

    void ScatterSecondStep(){
        //frameBuffer.Bind();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0,0,renderWidth,renderHeight);
        glEnable(GL_TEXTURE_2D);


        // Render the scene with no light scattering (done in the main)
        glClear (GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);
        glColor4f(1,1,1,1);

    }

    void ScatterThirdstep(){
        //glDisable(GL_BLEND);

        // Paint the light scaterring effect

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho( -renderWidth/2,renderWidth/2, -renderHeight/2, renderHeight/2, 000, 50000.0 );
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glClear (GL_DEPTH_BUFFER_BIT );


        glActiveTextureARB(GL_TEXTURE0_ARB);
        glBindTexture(GL_TEXTURE_2D, fboTexId);

        glBindTexture(GL_TEXTURE_2D, screenCopyTextureId);


        /*Utiliser ce qui suit pour le shader (à coder)*/
        glUseProgram(program_id_);
        glUniform2fARB(glsl_loc_light,uniformLightX,uniformLightY);
        glUniform1fARB(glsl_loc_exposure,uniformExposure);
        glUniform1fARB(glsl_loc_decay,uniformDecay);
        glUniform1fARB(glsl_loc_density,uniformDensity);
        glUniform1fARB(glsl_loc_weight,uniformWeight);
        glUniform1iARB(glsl_loc_myTexture,0);

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        glBegin(GL_QUADS);
        glTexCoord2f(0,0);
        glVertex2f(-renderWidth/2,-renderHeight/2);

        glTexCoord2f(1,0);
        glVertex2f(renderWidth/2,-renderHeight/2);

        glTexCoord2f(1,1);
        glVertex2f(renderWidth/2,renderHeight/2);

        glTexCoord2f(0,1);
        glVertex2f(-renderWidth/2,renderHeight/2);
        glEnd();


        glUseProgramObjectARB(0);

        // DRAWING TEXT
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho( -renderWidth/2,renderWidth/2, -renderHeight/2, renderHeight/2, 0.01f, 600.0f );
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslated(0,0,-500);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glClear ( GL_DEPTH_BUFFER_BIT );
        glDisable(GL_BLEND);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST);

        /*sprintf(fps,"fps: %.0f",Timer::fps );
                sprintf(polyCount,"polygones rendered: %d",polygonRendered );
                sprintf(s_textSwitchs,"textures switches: %d",textureSwitchs );
                drawString(fps,renderWidth/2-250,-renderHeight/2+60);
                drawString(polyCount,renderWidth/2-250,-renderHeight/2+40);

                drawString(s_textSwitchs,renderWidth/2-250,-renderHeight/2+20);
                //drawString(s_extensions,-renderWidth/2+10,renderHeight/2-80);*/

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        // END DRAWING TEXT

    }


    void Cleanup() {
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &vertex_buffer_object_position_);
        glDeleteBuffers(1, &vertex_buffer_object_index_);
        glDeleteVertexArrays(1, &vertex_array_id_);
        glDeleteProgram(program_id_);
        glDeleteTextures(1, &fboTexId);
    }


};
