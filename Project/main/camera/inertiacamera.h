#pragma once
#include <vector>
#include "icg_helper.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "abstractcamera.h"
#define ACCEL_FACTOR 0.0005
#define MAX_T 5

using namespace glm;

class InertiaCamera : public AbstractCamera {

private:

    float a1, a2, currTrans, x0, rh1, rh2, currRoth, rh0, rv1, rv2, currRotv, rv0;
    int t, trh, trv,t10, t20, rH10, rH20,rV10, rV20;
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
        a2(0.0), x0(0.0), rh1(0.0), rh2(0.0), rh0(0.0), rv1(0.0), rv2(0.0), rv0(0.0), t(0), trh(0), trv(0),
        frontInc(false), frontDec(false), frontIncv(false), frontDecv(false), frontInch(false), frontDech(false),
        increment(false), incrementrh(false), incrementrv(false),
        t10(0), t20(0), rH10(0), rH20(0), rV10(0), rV20(0){}

    virtual ~InertiaCamera() {}

    virtual void Init(vec3 initEye = vec3(0), vec3 initCenter = vec3(0), vec3 up = vec3(0,1,0)) override {
        view_matrix = LookAt(initEye, initCenter, up);
        a1 = 0.0;
        a2= 0.0;
        x0= 0.0;
        rh1= 0.0;
        rh2= 0.0;
        rh0= 0.0;
        rv1= 0.0;
        rv2= 0.0;
        rv0= 0.0;
        t= 0.0;
        trh= 0.0;
        trv= 0.0;
        frontInc = false;
        frontDec= false;
        frontIncv= false;
        frontDecv= false;
        frontInch= false;
        frontDech= false;
        increment= false;
        incrementrh= false;
        incrementrv= false;
        t10= 0.0;
        t20= 0.0;
        rH10= 0.0;
        rH20= 0.0;
        rV10= 0.0;
        rV20= 0.0;
        AbstractCamera::Init(view_matrix);
    }


    void startIncrementingTranslation(){
        increment = true;
        t = 0;
    }

    void startIncrementingRotationh(){
        incrementrh = true;
        trh = 0;
    }

    void startIncrementingRotationv(){
        incrementrv = true;
        trv = 0;
    }

    void stopIncrementingTranslation(){
        increment = false;
        t = 0;
    }

    void stopIncrementingRotationh(){
        incrementrh = false;
        trh=0;

    }

    void stopIncrementingRotationv(){
        incrementrv = false;
        trv =0;
    }


    void inertiaFunc(bool *front, bool* back,
                     float *posValue, float *initPos,
                     float *fwAccel, float *bwAccel,
                     int *time, int *fwStartTime, int *bwStartTime,
                     void (InertiaCamera::*startIncremF)(), void (InertiaCamera::*stopIncremF)()){
        if((*front)){
            if((*posValue) == 0){
                (this->*startIncremF)();
                ++(*time);
                (*posValue) = 0.5*(*fwAccel)*(*time)*(*time);
            }
            if((*posValue)>=0 && (*posValue) < 0.5*(*fwAccel)*MAX_T*MAX_T){
                (*bwAccel) = 0.0;
                (*posValue) = (*initPos)+ 0.5*(*fwAccel)*(*time)*(*time);
            } else if((*posValue) >=0.5*(*fwAccel)*MAX_T*MAX_T){
                (*bwAccel) = 0.0;
                (*posValue) = (*initPos)+0.5*(*fwAccel)*MAX_T*MAX_T;
            } else{
                (*bwAccel) = 0.0;
                (*posValue) = (*initPos) + 0.5*(*fwAccel)*((*time)-(*bwStartTime))*((*time)-(*bwStartTime));

            }


        } else if((*back)){
            if((*posValue) == 0){
                (this->*startIncremF)();
                ++(*time);
                (*posValue) = 0.5*(*bwAccel)*(*time)*(*time);
            }
            if((*posValue)<=0 && (*posValue) > 0.5*(*bwAccel)*MAX_T*MAX_T){
                (*fwAccel) = 0.0;
                (*posValue) = (*initPos)+ 0.5*(*bwAccel)*(*time)*(*time);
            }else if((*posValue) <= 0.5*(*bwAccel)*MAX_T*MAX_T){
                (*fwAccel) = 0.0;
                (*posValue) = (*initPos)+0.5*(*bwAccel)*MAX_T*MAX_T;
            }else{
                (*fwAccel) = 0.0;
                (*posValue)=  (*initPos) + 0.5*(*bwAccel)*((*time)-(*fwStartTime))*((*time)-(*fwStartTime));
            }

        }else{
            if((*posValue) > 0){
                (*initPos) = (*posValue);
                (*fwAccel) = 0.0;
                (*bwAccel) = -ACCEL_FACTOR;
                (*posValue) = (*initPos) + 0.5*(*bwAccel)*((*time)-(*fwStartTime))*((*time)-(*fwStartTime));
                if((*posValue) <= 0){
                    (*posValue) = 0;
                }
            }else if((*posValue) < 0){
                (*initPos) = (*posValue);
                (*bwAccel) = 0.0;
                (*fwAccel) = ACCEL_FACTOR;
                (*posValue) = (*initPos) + 0.5*(*fwAccel)*((*time)-(*bwStartTime))*((*time)-(*bwStartTime));
                if((*posValue) >=0){
                    (*posValue) = 0;
                }
            }
            else if((*posValue) == 0){
                (this->*stopIncremF)();
                (*fwAccel) = 0.0;
                (*bwAccel) = 0.0;
                (*initPos) =0;
                (*fwStartTime) = 0;
                (*bwStartTime) = 0;
            }
        }
    }

    void transFunc(bool *front, bool *back){
        inertiaFunc(front,back, &currTrans, &x0, &a1, &a2, &t, &t10, &t20, &InertiaCamera::startIncrementingTranslation,
                    &InertiaCamera::stopIncrementingTranslation);
        view_matrix = glm::translate(IDENTITY_MATRIX, vec3(0.0,0.0,currTrans))*view_matrix;
        frontInc = false;
        frontDec = false;
    }

    void rotHFunc(bool *front, bool *back){
        inertiaFunc(front, back, &currRoth, &rh0,&rh1,&rh2, &trh, &rH10, &rH20,
                    &InertiaCamera::startIncrementingRotationh, &InertiaCamera::stopIncrementingRotationh);

        view_matrix = glm::rotate(IDENTITY_MATRIX, -currRoth, vec3(1.0,0.0,0.0f))*view_matrix;
        frontInch = false;
        frontDech = false;

    }

    void rotVFunc(bool *front, bool *back){
        inertiaFunc(front, back, &currRotv, &rv0, &rv1, &rv2, &trv, &rV10, &rV20,
                    &InertiaCamera::startIncrementingRotationv, &InertiaCamera::stopIncrementingRotationv);

        view_matrix = glm::rotate(IDENTITY_MATRIX, -currRotv, vec3(0.0,1.0,0.0f))*view_matrix;
        frontIncv = false;
        frontDecv = false;
    }




    virtual void animate() override {

        if(increment) {
            t++;
        }
        if(incrementrh) {
            trh++;
        }
        if(incrementrv) {
            trv++;
        }
        transFunc(&frontInc, &frontDec);
        rotHFunc(&frontInch, &frontDech);
        rotVFunc(&frontIncv, &frontDecv);
        AbstractCamera::Init(view_matrix);

    }

    void rotate(float x, float z) override {}

    void move(float x, float y, float z) override {}

    virtual void increaseVelocity() override {}

    virtual void decreaseVelocity() override {}

    virtual void beginFwAccel() override {
        frontInc = true;
        a1 = ACCEL_FACTOR;
        t10 = t;
    }

    virtual void beginBwAccel() override{
        frontDec = true;
        a2 = -ACCEL_FACTOR;
        t20 = t;
    }

    virtual void beginYawAccel() override{
        frontInch = true;
        rh1 = ACCEL_FACTOR;
        rH10 = trh;
    }

    virtual void beginReverseYawAccel() override{
        frontDech = true;
        rh2 = -ACCEL_FACTOR;
        rH20 = trh;
    }

    virtual void beginPitchAccel() override{
        frontIncv = true;
        rv1 = ACCEL_FACTOR;
        rV10 = trv;
    }

    virtual void beginReversePitchAccel() override{
        frontDecv = true;
        rv2 = -ACCEL_FACTOR;
        rV20 = trv;
    }


};
