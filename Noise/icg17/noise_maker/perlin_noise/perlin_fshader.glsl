#version 330

#extension GL_EXT_gpu_shader4 : enable

// the array and permutations of the perlinNoise function, as well as the fade function all comes from:
// http://flafla2.github.io/2014/08/09/perlinnoise.html



in vec2 uv;

uniform int permutation[256];

out vec3 color;

#define CUBE_NUMBER 10.0
#define SQRT2 1.41421356237

float fade(float t){
    return t*t*t*(t*(t*6.0 -15.0) +10.0);
}


int inc(int num){
    return (num+1)%256;
}

/**
  This function is used to compute the dot product between a gradient and a vector v(x,y).
  The 4 gradients we have are (1,1), (1,-1), (-1,1), (-1,-1)
  Given an index, we must select between 4 possible values;
  x+y, x-y, -x+y, -x-y.

  **/
float grad_dot(int index, vec2 pos){
    int h = index & 3;
    float u = h < 2? pos.x: -pos.x;
    float v = h == 0? pos.y : h == 2? pos.y : -pos.y;
    return u+v;
}

vec3 perlinNoise(vec2 zx){
    int p[512];
    for(int i=0; i < 512;i++){
        p[i] = permutation[i%256];
    }

    // Selects the cube we're in
    ivec2 xiyi = ivec2(zx * CUBE_NUMBER )%256;

    int xx = xiyi.x;
    int yy = xiyi.y;

    // Represents the coordinates inside of the cube
    vec2 xryr = vec2(mod(zx.x*CUBE_NUMBER, 1.0), mod(zx.y*CUBE_NUMBER, 1.0));


    // We hash the coordinates of the corner
    int aa, ab, ba, bb;
    aa = p[(p[xx]+yy)];
    ab = p[(p[xx]+ inc(yy))];
    ba = p[p[xx+1]+ yy];
    bb = p[p[xx+1]+ inc(yy)];

    // We compute with grad_dot s,t, u and v directly, and use them in st and ov
    float st = mix(grad_dot(aa, xryr), grad_dot(ba, xryr - vec2(1,0)), fade(xryr.x));
    float ov = mix(grad_dot(ab, xryr - vec2(0,1)), grad_dot(bb, xryr - vec2(1,1)), fade(xryr.x));
    float noise = mix(st, ov, fade(xryr.y));

    return vec3(noise,noise,noise);

}


void main(void)
{
    color = perlinNoise(uv);
}
