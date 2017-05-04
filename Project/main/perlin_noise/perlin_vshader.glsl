#version 330

in vec2 vtexcoord;
in vec3 vpoint;

out vec2 uv;

void main() {
    uv = vtexcoord;
    gl_Position = vec4(vpoint, 1.0);
}
