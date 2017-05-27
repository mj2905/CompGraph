#version 330

in vec3 vpoint;
in vec3 vnormal;
in vec2 vtexcoord;


out vec2 uv;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;

out vec4 vpoint_mv;

void main() {
    mat4 MV = view * model;
    vpoint_mv = MV * vec4(vpoint, 1.0);
    gl_Position = projection * vpoint_mv;
    uv = vtexcoord;
}
