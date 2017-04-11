#version 410

in vec2 position;

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

const float zoom = 1; //constant to zoom in the textures

#define MIDDLE_X 0
#define MIDDLE_Y 0
#define THRESHOLD_LINKS (2e-3)
#define MULT_THRESHOLD_FACTOR (0.5/THRESHOLD_LINKS)


void main() {
    vec2 uv = (position + vec2(1.0, 1.0)) * 0.5;
    uv /= zoom;

    vec2 xy = vec2(mod(offset.x, 1.0f), mod(offset.y, 1.0f)) - 1; //allows to know how much we need to substract from uv, to facilitate computations
    vec2 pos = uv + xy;

    if(abs(pos.x) <= THRESHOLD_LINKS) { //does the links between textures (otherwise, strange boundaries)
        if(pos.y <= MIDDLE_Y) {
            height = mix(texture(texBL, vec2(1 - THRESHOLD_LINKS, pos.y + 1)).r, texture(texBR, vec2(THRESHOLD_LINKS, pos.y + 1)).r, pos.x * MULT_THRESHOLD_FACTOR + 0.5);
        }
        if(pos.y > MIDDLE_Y) {
            height = mix(texture(texUL, vec2(1 - THRESHOLD_LINKS, pos.y)).r, texture(texUR, vec2(THRESHOLD_LINKS, pos.y)).r, pos.x * MULT_THRESHOLD_FACTOR + 0.5);
        }
    }
    else if(abs(pos.y) <= THRESHOLD_LINKS) { //does the links between textures (otherwise, strange boundaries)
        if(pos.x <= MIDDLE_X) {
            height = mix(texture(texBL, vec2(pos.x + 1, 1 - THRESHOLD_LINKS)).r, texture(texUL, vec2(pos.x + 1, THRESHOLD_LINKS)).r, pos.y * MULT_THRESHOLD_FACTOR + 0.5);
        }
        if(pos.x > MIDDLE_X) {
            height = mix(texture(texBR, vec2(pos.x, 1 - THRESHOLD_LINKS)).r, texture(texUR, vec2(pos.x, THRESHOLD_LINKS)).r, pos.y * MULT_THRESHOLD_FACTOR + 0.5);
        }
    }
    else if(pos.x < MIDDLE_X && pos.y < MIDDLE_Y) { //we seek the bottom left texture entries for corresponding points
        height = texture(texBL, pos + 1).r;
    }
    else if(pos.x > MIDDLE_X && pos.y < MIDDLE_Y) { //we seek the bottom right texture entries for corresponding points
        height = texture(texBR, vec2(pos.x, pos.y + 1)).r;
    }
    else if(pos.x < MIDDLE_X && pos.y > MIDDLE_Y) { //we seek the top left texture entries for corresponding points
        height = texture(texUL, vec2(pos.x + 1, pos.y)).r;
    }
    else { //we seek the top right texture entries for corresponding points
        height = texture(texUR, pos).r;
    }

    vec3 pos_3d = vec3(position.x, height, -position.y);

    vpoint_mv = view * model * vec4(pos_3d, 1.0);
    gl_Position = projection * vpoint_mv;
    light_dir = normalize(light_pos - vec3(vpoint_mv));
    view_dir = normalize(- vec3(vpoint_mv));

}

