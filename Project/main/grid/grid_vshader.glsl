#version 330

in vec2 position;

out vec2 uv;
out vec4 vpoint_mv;
out vec3 light_dir, view_dir;
out float height;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;

uniform sampler2D texBL;
uniform sampler2D texBR;
uniform sampler2D texUL;
uniform sampler2D texUR;

uniform vec2 offset;

const float zoom = 1;

#define MIDDLE_X 0
#define MIDDLE_Y 0

void main() {
    uv = (position + vec2(1.0, 1.0)) * 0.5;
    uv /= zoom;

    vec2 xy = vec2(mod(offset.x, 1.0f), mod(offset.y, 1.0f)) - 1; //bet -1 and 0
    vec2 pos = uv + xy;

    if(abs(pos.x) <= 2e-3) {
        if(pos.y < 0) {
            height = mix(texture(texBL, vec2(1-2e-3, pos.y + 1)).r, texture(texBR, vec2(2e-3, pos.y + 1)).r, pos.x * 250 + 0.5);
        }
        if(pos.y > 0) {
            height = mix(texture(texUL, vec2(1-2e-3, pos.y)).r, texture(texUR, vec2(2e-3, pos.y)).r, pos.x * 250 + 0.5);
        }
    }
    else if(abs(pos.y) <= 2e-3) {
        if(pos.x < 0) {
            height = mix(texture(texBL, vec2(pos.x + 1, 1-2e-3)).r, texture(texUL, vec2(pos.x + 1, 2e-3)).r, pos.y * 250 + 0.5);
        }
        if(pos.x > 0) {
            height = mix(texture(texBR, vec2(pos.x, 1-2e-3)).r, texture(texUR, vec2(pos.x, 2e-3)).r, pos.y * 250 + 0.5);
        }
    }
    else if(pos.x < MIDDLE_X && pos.y < MIDDLE_Y) {
        height = texture(texBL, pos + 1).r;
    }
    else if(pos.x > MIDDLE_X && pos.y < MIDDLE_Y) {
        height = texture(texBR, vec2(pos.x, pos.y + 1)).r;
    }
    else if(pos.x < MIDDLE_X && pos.y > MIDDLE_Y) {
        height = texture(texUL, vec2(pos.x + 1, pos.y)).r;
    }
    else if(pos.x > MIDDLE_X && pos.y > MIDDLE_Y) {
        height = texture(texUR, pos).r;
    }

    vec3 pos_3d = vec3(position.x, height, -position.y);

    vpoint_mv = view * model * vec4(pos_3d, 1.0);
    gl_Position = projection * vpoint_mv;
    light_dir = normalize(light_pos - vec3(vpoint_mv));
    view_dir = normalize(- vec3(vpoint_mv));

}

