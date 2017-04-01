#version 330


in vec3 vpoint;
in vec2 vtexcoord;

out vec2 uv;

//uniform mat4 MVP;

void main(void)
{
    gl_Position =  vec4(vpoint, 1.0);
    uv = vtexcoord;
}
