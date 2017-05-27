#version 330


out vec3 color;
in vec2 uv;
uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;
uniform sampler2D tex1D;
uniform sampler2D tex2D;
in vec4 vpoint_mv;

void main() {    
    color = vec3(1.0,1.0,1.0);
}
