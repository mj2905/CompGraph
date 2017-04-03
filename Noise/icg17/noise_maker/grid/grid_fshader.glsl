#version 330

in vec2 uv;


layout(location=0)out vec3 color;

//uniform sampler1D tex;

void main() {
    color = vec3(1.0,0.0,0.0);//texture(tex, uv).rgb;
}
