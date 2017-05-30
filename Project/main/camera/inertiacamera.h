#pragma once
#include <vector>
#include "icg_helper.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "abstractcamera.h"
#define ACCEL_FACTOR 0.000005
#define MAX_T 10
#define delta_t 1/2.0f

using namespace glm;

class InertiaCamera : public AbstractCamera {

private:

    float a1, a2;
    float frontTrans, frontSpeed, sideTrans, sideSpeed, upTrans, upSpeed;
    bool frontInc, frontDec,frontIncv, frontDecv,frontInch, frontDech;
    bool increment, incrementrh, incrementrv;
    glm::mat4 view_matrix;

    mat4 LookAt(vec3 eye, vec3 center, vec3 up) {
        vec3 z_cam = normalize(eye - center);
        vec3 x_cam = normalize(cross(up, z_cam));
        vec3 y_cam = cross(z_cam, x_cam);

        mat3 R(x_cam, y_cam, z_cam);
        R = transpose(R);

        mat4 look_at(vec4(R[0], 0.0f),
                vec4(R[1], 0.0f),
                vec4(R[2], 0.0f),
                vec4(-R * (eye), 1.0f));
        return look_at;
    }


public:

    InertiaCamera() : AbstractCamera(), a1(0.0),
        a2(0.0),
        frontInc(false), frontDec(false), frontIncv(false), frontDecv(false), frontInch(false), frontDech(false),
        frontTrans(0.0), frontSpeed(0.0), sideTrans(0.0), sideSpeed(0.0), upTrans(0.0), upSpeed(0.0),
        increment(false), incrementrh(false), incrementrv(false){}

    virtual ~InertiaCamera() {}

    virtual void Init(vec3 initEye = vec3(0), vec3 initCenter = vec3(0), vec3 up = vec3(0,1,0)) override {
        view_matrix = LookAt(initEye, initCenter, up);
        a1 = ACCEL_FACTOR;
        a2= -ACCEL_FACTOR;

        //--
        frontTrans = 0.0;
        frontSpeed = 0.0;
        sideTrans = 0.0;
        sideSpeed = 0.0;
        upTrans = 0.0;
        upSpeed = 0.0;


        frontInc = false;
        frontDec= false;
        frontIncv= false;
        frontDecv= false;
        frontInch= false;
        frontDech= false;
        increment= false;
        incrementrh= false;
        incrementrv= false;
        AbstractCamera::Init(initEye,initCenter, up);

    }

    void inertiaFunc(bool &front, bool &back,
                     float &posValue, float &speedValue,
                     float &fwAccel, float &bwAccel,
                     bool &incremBool){
       if(front && incremBool){
           speedValue = speedValue + fwAccel*delta_t;
           posValue = posValue + speedValue*delta_t;
       }
       else if(back && incremBool){
           speedValue = speedValue + bwAccel*delta_t;
           posValue = posValue + speedValue*delta_t;
       }
       else if(incremBool){
           if(posValue > ACCEL_FACTOR){
               speedValue = speedValue + bwAccel*delta_t;
               posValue = posValue + speedValue*delta_t;
               if(posValue <=0){
                   posValue = 0;
                   speedValue = 0;
               }
           } else if(posValue < - ACCEL_FACTOR){
               speedValue = speedValue + fwAccel*delta_t;
               posValue = posValue + speedValue*delta_t;
               if(posValue >=0){
                   posValue = 0;
                   speedValue = 0;
               }
           }
       }
    }


    void transFunc(bool &front, bool &back){
        inertiaFunc(front, back, frontTrans, frontSpeed, a1,a2, increment);
        view_matrix = glm::translate(IDENTITY_MATRIX, vec3(0.0,0.0,frontTrans))*view_matrix;
        frontInc = false;
        frontDec = false;
    }

    void rotHFunc(bool &front, bool &back){
        inertiaFunc(front, back, sideTrans, sideSpeed, a1,a2, incrementrh);
        view_matrix = glm::rotate(IDENTITY_MATRIX, -sideTrans, vec3(1.0,0.0,0.0f))*view_matrix;
        frontInch = false;
        frontDech = false;

    }

    void rotVFunc(bool &front, bool &back){
        inertiaFunc(front, back, upTrans, upSpeed, a1, a2, incrementrv);
        view_matrix = glm::rotate(IDENTITY_MATRIX, -upTrans, vec3(0.0,1.0,0.0f))*view_matrix;
        frontIncv = false;
        frontDecv = false;
    }


    virtual void animate() override {
        if(increment){
            transFunc(frontInc, frontDec);
        }
        if(incrementrh){
            rotHFunc(frontInch, frontDech);
        }
        if(incrementrv){
            rotVFunc(frontIncv, frontDecv);
        }

        AbstractCamera::Init(view_matrix);

    }


    virtual void beginFwAccel() override {
        frontInc = true;
        frontDec =false;
        increment = true;
    }

    virtual void beginBwAccel() override{
        frontDec = true;
        frontInc = false;
        increment = true;
    }

    virtual void beginYawAccel() override{
        frontInch = true;
        frontDech = false;
        incrementrh = true;
    }

    virtual void beginReverseYawAccel() override{
        frontDech = true;
        frontInch = false;
        incrementrh = true;
    }

    virtual void beginPitchAccel() override{
        frontIncv = true;
        frontDecv = false;
        incrementrv = true;
    }

    virtual void beginReversePitchAccel() override{
        frontDecv = true;
        frontIncv = false;
        incrementrv = true;
    }


    void rotate(float x, float z) override {}

    void move(float x, float y, float z) override {}

    virtual void increaseVelocity() override {}

    virtual void decreaseVelocity() override {}



};
