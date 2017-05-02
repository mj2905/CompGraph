#version 330

in vec2 position;

out vec2 uv;
out vec4 vpoint_mv;
out vec3 light_dir, view_dir;
out float height;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;

uniform float time;

const float M_PI = 3.1415926535897932384626433832795;

uniform vec2 offset;

void main() {
    uv = (position + vec2(1.0, 1.0)) * 0.5;
    vec2 pos = uv + vec2(mod(offset.x, 1.0), mod(offset.y, 1.0));
    height = 0.4;//0.4 + sin(2 * M_PI * 25 * (0.5*pos.x + 0.5*pos.y) + time)/800 + sin(2 * M_PI * 25 * (pos.x * pos.y) + 2*time)/800 + sin(2 * M_PI * 25 * (0.5*pos.x - pos.y) + 4*time)/800;

    vec3 pos_3d = vec3(position.x, height, -position.y);

    vpoint_mv = view * model * vec4(pos_3d, 1.0);
    gl_Position = projection * vpoint_mv;
    light_dir = normalize(-light_pos - vec3(vpoint_mv));
    view_dir = normalize(- vec3(vpoint_mv));

}

