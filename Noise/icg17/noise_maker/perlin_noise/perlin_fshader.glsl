#version 330

#extension GL_EXT_gpu_shader4 : enable

// the array and permutations of the perlinNoise function, as well as the fade function all comes from:
// http://flafla2.github.io/2014/08/09/perlinnoise.html

in vec2 uv;

uniform int permutation[256];

uniform float H, lacunarity, offset, persistence, amplitude;
uniform int octaves;

out vec3 color;

#define SQRT2 1.41421356237
#define FREQ 2
#define CUBE_NUMBER FREQ

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

float perlinNoise(vec2 zx){
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

    return noise;

}


float OctavePerlin(float x, float y, int octaves, float persistence) {
    float total = 0;
    float frequency = FREQ;
    float amplitude = amplitude;
    float maxValue = 0;
    for(int i=0;i<octaves;i++) {
        total += perlinNoise(vec2(x*frequency,y*frequency))*amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2;
    }

    return total/maxValue;
}

float multifrac(vec2 point, float H, float lacunarity, int octaves, float offset){
    float value = 1.0;
    for(int i = 0 ; i < octaves; ++i){
        //value *= (OctavePerlin(point.x, point.y, octaves, persistence)+offset) * pow(lacunarity, -H*i);
        value *= (perlinNoise(point)+offset)*pow(lacunarity, -H*i);
        point *= lacunarity;
    }
    return value;
}

void main(void)
{
    //float n = OctavePerlin(uv.x,uv.y, OCTAVES, PERSISTENCE); // 2nd parameter is directly related to the spike numbers
   // float H = 0.65;
    float n = multifrac(uv, H, lacunarity, octaves, offset);
    color = vec3(n,n,n);
}