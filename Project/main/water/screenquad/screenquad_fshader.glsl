#version 330

in vec2 uv;

out vec3 color;

const int MAX_SIZE = 400; // /!\ modify too in main if modified

uniform int SIZE_OPT;
uniform float G[MAX_SIZE];

uniform bool is_horizontal;

uniform sampler2D tex;
uniform float tex_width;
uniform float tex_height;

void main() {

    vec2 direction;
    if(is_horizontal) {
        direction = vec2(1, 0);
    }
    else {
        direction = vec2(0, 1);
    }

    vec3 color_tot = vec3(0);
    float weight_tot = 0;
    //We choose these boundaries, as they are the same as for basic gaussian blur
    for(int i = -SIZE_OPT; i <= SIZE_OPT; ++i){
        float w = G[i + SIZE_OPT];
        vec3 neigh_color = texture(tex, uv + direction*vec2(i/tex_width,i/tex_height)).rgb; //direction is (0, 1) or (1, 0)
        color_tot += w * neigh_color;
        weight_tot += w;
    }
    color = color_tot / weight_tot; // ensure \int w = 1

}

