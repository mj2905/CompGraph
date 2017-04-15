#version 330

in vec2 uv;

out vec4 color;

uniform sampler2D tex;


const int SIZE_PERM = 512;
uniform int[SIZE_PERM] p;

int nb_octaves = 8;
const float persistence = 0.45f;
uniform vec2 off;

float f(float t) {
    return t*t*t*((t*(t*6.0 - 15.0)) + 10.0);
}

float grad(int hash, vec2 v)
{
    int hashMasked = hash & 0x3;
         if(hashMasked == 0x0) return  v.x + v.y - 1; // (1, 1) bet -1 and 1
    else if(hashMasked == 0x1) return  v.y - v.x; // (-1, 1) bet -1 and 1
    else if(hashMasked == 0x2) return  v.x - v.y; // (1, -1) bet -1 and 1
    else if(hashMasked == 0x3) return  -v.x - v.y + 1; // (-1, -1) bet -1 and 1
    else                       return 0;
}

int inc(int num) {
    return (num+1)%SIZE_PERM;
}

float perlin(vec2 xy, float nb_subdivisions) {
    vec2 xfyf = vec2(mod(xy.x*nb_subdivisions, 1.0), mod(xy.y*nb_subdivisions, 1.0));
    ivec2 xiyi = ivec2(xy * nb_subdivisions) % 256;
    float fx = f(xfyf.x), fy = f(xfyf.y);

    int s = p[p[xiyi.x ]+    xiyi.y ]; //bet 0 and 255
    int t = p[p[xiyi.x + 1 ]+    xiyi.y ]; //bet 0 and 255
    int u = p[inc(p[xiyi.x ]+ xiyi.y)]; //bet 0 and 255
    int v = p[inc(p[xiyi.x + 1 ]+ xiyi.y)]; //bet 0 and 255

    float st = mix(grad(s, xfyf), grad(t, xfyf-vec2(1, 0)), fx);
    float uv = mix(grad(u, xfyf-vec2(0, 1)), grad(v, xfyf-vec2(1, 1)), fx);

    return mix(st, uv, fy);
}

float octavePerlin(vec2 xy, vec2 offset) {
    float total = 0;
    float frequency = 3.2;
    float amplitude = 1;
    float maxValue = 0;
    for(int i = 0; i < nb_octaves; ++i) {
        total += amplitude * abs(perlin(uv+offset, frequency));
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2;
    }
    return (total/(2*maxValue) + 0.5); //bet -1/6 and 7/6
}


void main() {
    float x = uv.x;
    float y = uv.y;
    float t = sin(x+ octavePerlin(uv,vec2(0,0)));
    float a = (1.0/y - 0.3);
    color = vec4(t,0,0, a);//*mix(vec3(0,0,0),mix(texture(tex, uv).rgb,vec3(1,1,1), uv.y/5.0), uv.y);
}
