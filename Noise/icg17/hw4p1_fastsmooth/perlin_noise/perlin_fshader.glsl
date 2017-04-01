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



vec3 perlinNoise(vec2 zx){
    vec2 grad[4];
    grad[0] = vec2(1,1);
    grad[1] = vec2(1,-1);
    grad[2] = vec2(-1,1);
    grad[3] = vec2(-1,-1);


    int p[512];                                                    // Doubled permutation to avoid overflow
    for(int i=0; i < 512;i++){
        p[i] = permutation[i%256];
    }

    // numéro de zone
    ivec2 xiyi = ivec2(zx * CUBE_NUMBER )%256;

    int xx = xiyi.x;
    int yy = xiyi.y;
    // coordonnées dans le cube
    vec2 xryr = vec2(mod(zx.x*CUBE_NUMBER, 1.0), mod(zx.y*CUBE_NUMBER, 1.0));


    // On hash les coordonnées des coins du cube.
    int aa, ab, ba, bb;
    aa = p[(p[xx]+yy)];
    ab = p[(p[xx]+ yy+1)];
    ba = p[(p[xx+1]+ yy)];
    bb = p[(p[xx+1]+yy+1)];

    // On trouve les produits scalaires en utilisant les hash précédents pour accéder aux gradients
    float s = dot(grad[aa%4], xryr);
    float t = dot(grad[ab%4], xryr - vec2(1,0));
    float o = dot(grad[ba%4], xryr - vec2(0,1));
    float v = dot(grad[bb%4], xryr- vec2(1,1));

    // Interp linéaire (mix) puis renvoi
    float st = mix(s, t, fade(xryr.x));
    float ov = mix(o, v, fade(xryr.x));
    float noise = mix(st, ov, fade(xryr.y));

    return vec3(noise,noise,noise);

}


void main(void)
{
    color = perlinNoise(uv);
}
