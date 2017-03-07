#version 330

#define M_PI 3.1415926535897932384626433832795

in vec2 uv;

out vec3 color;

uniform sampler1D colormap;

void main() {
    int nb_carreaux = 5;
    float value = sin(uv.x * 2 * M_PI * nb_carreaux)*sin(uv.y * 2 * M_PI * nb_carreaux)/2 + 0.5;
    color = texture(colormap, value).rgb;
}


