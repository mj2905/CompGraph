#version 330

in vec2 position;

out vec2 uv;

uniform mat4 MVP;
uniform float time;

#define M_PI 3.1415926535897932384626433832795

void main() {
    uv = (position + vec2(1.0, 1.0)) * 0.5;

    // convert the 2D position into 3D positions that all lay in a horizontal
    // plane.
    // TODO 6: animate the height of the grid points as a sine function of the
    // 'time' and the position ('uv') within the grid.

    float frequency = 2.0f;
    float amplitude = 0.1f;

    float height = amplitude * sin(2 * M_PI * frequency * (uv.x + uv.y) + time);

    vec3 pos_3d = vec3(position.x, height, -position.y);

    gl_Position = MVP * vec4(pos_3d, 1.0);
}
