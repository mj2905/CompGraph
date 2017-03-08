#version 330

in vec2 uv;

out vec4 color;

uniform sampler2D tex;

void main() {
    float x = uv.x*2 - 1, y = uv.y*2 - 1;

    float value = (x * x + y * y <= 1) ? 0.0 : 1.0;
    color = vec4(value,value,value,value);
}


