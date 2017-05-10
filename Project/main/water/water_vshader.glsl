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

const float M_PI = 3.1415926535897932384626433832795;

uniform vec2 offset;
uniform sampler2D normal_map;
uniform sampler2D normal_map_2;

uniform float time;

out vec3 vnormal_mv;
uniform sampler2D tex;

void main() {
    uv = (position + vec2(1.0, 1.0)) * 0.5;
    vec2 pos = uv + vec2(mod(offset.x, 1.0), mod(offset.y, 1.0));
    height = 0.4;

    vec3 pos_3d = vec3(position.x, height, -position.y);

    vnormal_mv = mat3(transpose(inverse(view * model))) *(texture(normal_map, 2*pos + vec2(0, -time/100)).xyz + texture(normal_map_2, 2*pos + vec2(time/500, time/200)).xyz);

    mat4 MV = view * model;

    vpoint_mv = MV * vec4(pos_3d, 1.0);
    gl_Position = projection * vpoint_mv;
    light_dir = normalize(mat3(MV) * light_pos - vpoint_mv.xyz);
    view_dir = normalize(- vec3(vpoint_mv));

    gl_ClipDistance[0] = height - texture(tex, uv).r; //to cut water

}

