#version 330

out float heightmap;

in vec2 uv;

uniform sampler2D texBL;
uniform sampler2D texBR;
uniform sampler2D texUL;
uniform sampler2D texUR;

uniform vec2 offset;

#define MIDDLE_X 0
#define MIDDLE_Y 0
#define THRESHOLD_LINKS (2e-3)
#define MULT_THRESHOLD_FACTOR (0.5/THRESHOLD_LINKS)

void main() {

    vec2 xy = vec2(mod(offset.x, 1.0f), mod(offset.y, 1.0f)) - 1; //allows to know how much we need to substract from uv, to facilitate computations
    vec2 pos = uv + xy;
    float height = 0;

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

    heightmap = height;
}
