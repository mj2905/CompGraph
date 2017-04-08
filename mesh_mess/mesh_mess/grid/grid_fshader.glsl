#version 330

in vec2 uv;

//from hw3
out vec3 color;

in vec3 normal_mv;
in vec3 light_dir;
in vec3 view_dir;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

uniform sampler2D tex;

vec3 curr_point;

void main() {
    //curr_point = vec3()

    color = vec3(1.0,0.0,0.0);
}
