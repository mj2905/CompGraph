#version 330

in vec2 position;

uniform sampler2D shadow;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

void main() {
    vec2 uv = (position + vec2(1.0, 1.0)) * 0.5;
    float height = texture(shadow, uv).r;

    vec3 pos_3d = vec3(position.x, height, -position.y);

    gl_Position = projection * view * model * vec4(pos_3d, 1.0);

}

