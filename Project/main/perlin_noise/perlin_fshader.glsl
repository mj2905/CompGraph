#version 330

in vec2 uv;

out float heightmap;

const int SIZE_PERM = 256;
uniform int[SIZE_PERM] perm;

//const int nb_subdivisions = 1;
const int nb_octaves = 20;
uniform float persistence;
uniform float time;

int p[2*SIZE_PERM];

void generateP() {
    for(int i = 0; i < 512; ++i) {
        p[i] = perm[i%256];
    }
}

float f(float t) {
    return t*t*t*((t*(t*6.0 - 15.0)) + 10.0);
}

float grad(int hash, vec2 v)
{
    int hashMasked = hash & 0x7;
         if(hashMasked == 0x0) return  v.x + v.y; // (1, 1)
    else if(hashMasked == 0x1) return  v.y - v.x; // (-1, 1)
    else if(hashMasked == 0x2) return  v.x - v.y; // (1, -1)
    else if(hashMasked == 0x3) return  -v.x - v.y; // (-1, -1)
    else if(hashMasked == 0x4) return  -0.5*v.x+ 0.5*v.y; // (-0.5, 0.5)
    else if(hashMasked == 0x5) return  0.5*v.x + 0.5*v.y; // (0.5, 0.5)
    else if(hashMasked == 0x6) return  -0.5*v.x-0.5*v.y; // (-0.5, -0.5)
    else if(hashMasked == 0x7) return  0.5*v.x-0.5*v.y; // (0.5, -0.5)
    else                       return 0;
}

int inc(int num) {
    return (num+1)%SIZE_PERM;
}

float perlin(vec2 xy, float nb_subdivisions) {
    vec2 xfyf = vec2(mod(xy.x*nb_subdivisions, 1.0), mod(xy.y*nb_subdivisions, 1.0));
    ivec2 xiyi = ivec2(xy * nb_subdivisions) % 256;
    float fx = f(xfyf.x), fy = f(xfyf.y);

    int s = p[p[xiyi.x ]+    xiyi.y ];
    int t = p[p[xiyi.x + 1 ]+    xiyi.y ];
    int u = p[inc(p[xiyi.x ]+ xiyi.y)];
    int v = p[inc(p[xiyi.x + 1 ]+ xiyi.y)];

    float st = mix(grad(s, xfyf), grad(t, xfyf-vec2(1, 0)), fx);
    float uv = mix(grad(u, xfyf-vec2(0, 1)), grad(v, xfyf-vec2(1, 1)), fx);

    return (mix(st, uv, fy));
}

float octavePerlin(vec2 xy, float time) {
    float total = 0;
    float frequency = 2;
    float amplitude = 0.5;
    float maxValue = 0;
    for(int i = 0; i < nb_octaves; ++i) {
        total += amplitude * perlin(uv+time, frequency);
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2;
    }
    return (total/maxValue + 0.5); //bet -0.5 and 1.5 => more of lowest and highest values
}

void main() {

    generateP();
    heightmap = (octavePerlin(uv, time));


}
