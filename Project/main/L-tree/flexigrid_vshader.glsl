#version 330

in vec2 vtexcoord;

out vec2 uv;
out vec4 vpoint_mv;

out vec3 light_dir, view_dir;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;

uniform mat4 MVP;
uniform float time;

void main() {
    uv = (vtexcoord + vec2(1.0, 1.0)) * 0.5;

    // convert the 2D position into 3D positions that all lay in a horizontal
    // plane.
}
