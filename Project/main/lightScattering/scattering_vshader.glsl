#version 330

in vec2 vtexcoord;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

out vec3 vpoint;

void main(){
    vec2 uv = (vtexcoord + vec2(1.0, 1.0)) * 0.5;


    mat4 MV = view * model;
    vpoint = vec3(vtexcoord.x, 1.0, vtexcoord.y);

    vec4 vpoint_mv = MV * vec4(vpoint, 1.0);
    gl_Position = projection * vpoint_mv;
}
