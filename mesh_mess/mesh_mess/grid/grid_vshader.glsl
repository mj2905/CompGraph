#version 330

in vec2 position;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;
out vec2 uv;

out vec4 vertex_point_MV;


uniform float time;

uniform sampler2D height_map;

in vec3 vpoint;
in vec3 vnormal;

out vec3 normal_mv;
out vec3 light_dir;
out vec3 view_dir;


void main() {
    //mostly from previous homeworks
    uv = (position + vec2(1.0, 1.0)) * 0.5;

    float height = texture(height_map, uv)[0];

    mat4 MV = view * model;

    vec3 pos = vec3(position.x, height, -position.y);

    vertex_point_MV = vec4(pos, 1.0);

    mat4 MVP = projection * MV;
    gl_Position = MVP * vertex_point_MV;
    light_dir = normalize(light_pos - vec3(vertex_point_MV));
    view_dir = normalize(- vec3(vertex_point_MV));
}
