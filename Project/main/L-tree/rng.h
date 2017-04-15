#pragma once
#include <stdlib.h>
#include <math.h>


class RNG{
    private:
        int seed = 179424691;
        int n = 0;
        int q = 103;
        int c = 3;
        int a = 2;

    public:
        void Seed(int s){
            if(s != 0){
                seed = s;
            }
        }

        float rand(){
           float xn = fmod(pow((a*n + c), q-2), q);
           n++;
           return xn;
        }


};
