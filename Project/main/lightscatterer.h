#pragma once
#include "constants.h"
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>
#include <array>
#include "framebuffer.h"
#include "multitiles/multitiles.h"

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
    FrameBuffer frameBuffer;

    int window_width = 800;
    int window_height = 600;
    int fboId;

    GLuint fboTexId;

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

        // vertex one vertex array
        glGenVertexArrays(1, &vertex_array_id_);
        glBindVertexArray(vertex_array_id_);

        // vertex coordinates and indices
        {
            std::vector<GLfloat> vertices;
            std::vector<GLuint> indices;
            // makes a triangle grid with dimension 100x100.
            // always two subsequent entries in 'vertices' form a 2D vertex position.

            // the given code below are the vertices for a simple quad.
            // your grid should have the same dimension as that quad, i.e.,
            // reach from [-1, -1] to [1, 1].



            // position buffer
            glGenBuffers(1, &vertex_buffer_object_position_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_position_);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
                         &vertices[0], GL_STATIC_DRAW);

            // vertex indices
            glGenBuffers(1, &vertex_buffer_object_index_);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_index_);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
                         &indices[0], GL_STATIC_DRAW);

            // position shader attribute
            GLuint loc_position = glGetAttribLocation(program_id_, "position");
            glEnableVertexAttribArray(loc_position);
            glVertexAttribPointer(loc_position, 2, GL_FLOAT, DONT_NORMALIZE,
                                  ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }

        // load/Assign textures
        {
            glUniform1i(glGetUniformLocation(program_id_, "tex"), 0 /*GL_TEXTURE0*/);
            frameBuffer.Init(window_width,window_height, true);
            fboTexId = frameBuffer.getTextureId();
        }

        // lights and shading
        {
            light.Init(0,1,-1);
            //light_pos = glm::vec3(0.0f, 1, -1);

            glm::vec3 Ld = glm::vec3(1.0f, 1.0f, 0.8f);

            //light_pos_id = glGetUniformLocation(program_id_, "light_pos");

            GLuint Ld_id = glGetUniformLocation(program_id_, "Ld");

            glm::vec3 kd = glm::vec3(0.3f);

            GLuint kd_id = glGetUniformLocation(program_id_, "kd");

            //glUniform3fv(light_pos_id, 1, glm::value_ptr(light_pos));
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

    void ScatterWithMethod(GLFWwindow* window,
                           mat4 quad_model_matrix, mat4 projection_matrix,
                           mat4 mat, mat4 getView, mat4 projMatrix){
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
        glGetIntegerv(GL_VIEWPORT, viewport);
        glGetDoublev(GL_DEPTH_RANGE, depthRange);


        glfwGetFramebufferSize(window,&renderWidth, &renderHeight);


        // render light + obstruction
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);
        glClear ( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
        glColor4f(1,1,1,1);
        glViewport(0,0,window_width/*/OFF_SCREEN_RENDER_RATIO*/,window_height/*/OFF_SCREEN_RENDER_RATIO*/);
        glDisable(GL_TEXTURE_2D);
        glPushMatrix();
        //glTranslatef(light->lightPosition[0],light->lightPosition[1],light->lightPosition[2]);
        glTranslatef(light.getPosition().at(0), light.getPosition().at(1), light.getPosition().at(2));
        //light->render();
        glPopMatrix();
        GLdouble winX=0, winY =0, winZ = 0;

        gluProject(light.getPosition().at(0), light.getPosition().at(1), light.getPosition().at(2),
                   modelView,
                   projection,
                   viewport,
                   &winX,
                   &winY,
                   &winZ);


        uniformLightX = winX/((float)renderWidth/*/OFF_SCREEN_RENDER_RATIO*/);
        uniformLightY = winY/((float)renderHeight/*/OFF_SCREEN_RENDER_RATIO*/) ;


        // Draw occluding source black with light
        //glUseProgramObjectARB(0);
        glUseProgram(0);


        glEnable(GL_TEXTURE_2D);
        glColor4f(0,0,0,1);

    }

    void ScatterSecondStep(){
        frameBuffer.Bind();

        glViewport(0,0,renderWidth,renderHeight);
        glEnable(GL_TEXTURE_2D);


        // Render the scene with no light scattering (done in the main)
        glClear (GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);
        glColor4f(1,1,1,1);

    }

    void ScatterThirdstep(){
        /*
                       glPushMatrix();
                            glTranslatef(ikal->position.x,ikal->position.y,ikal->position.z);
                            glRotatef(180,0,1,0);
                            glRotatef(ikal->eulerRotation.z,0,0,1);
                            ikal->render();
                        glPopMatrix();

                       //sky1->render();
                       glPushMatrix();
                            glRotatef(180,1,0,0);
                            sky1->render();
                        glPopMatrix();*/

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

    void Draw(float offsetX, float offsetY, bool underwaterclip,
              const glm::mat4 &model = IDENTITY_MATRIX,
              const glm::mat4 &view = IDENTITY_MATRIX,
              const glm::mat4 &projection = IDENTITY_MATRIX) {

    }

};
