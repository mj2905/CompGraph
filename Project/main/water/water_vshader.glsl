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

vec3 mapXandZ(vec3 v) {
    return vec3(v.x * 2 - 1, v.y, v.z * 2 - 1);
}

void main() {
    uv = (position + vec2(1.0, 1.0)) * 0.5;
    vec2 pos = uv + vec2(mod(offset.x, 1.0), mod(offset.y, 1.0));
    height = 0.4;

    vec3 pos_3d = vec3(position.x, height, -position.y);

    float moveFactor = mod(time/50, 1.0);
    vec3 vnormal = 20*mapXandZ(texture(normal_map, vec2(100*pos.x, 100*pos.y - moveFactor)).rbg) + 20*mapXandZ(texture(normal_map, vec2(-100*pos.x + moveFactor, 100*pos.y + moveFactor)).rbg);
    vnormal_mv = mat3(transpose(inverse(view * model))) * vnormal;

    mat4 MV = view * model;

    vpoint_mv = MV * vec4(pos_3d, 1.0);
    gl_Position = projection * vpoint_mv;
    light_dir = normalize(mat3(MV) * light_pos - vpoint_mv.xyz);
    view_dir = normalize(- vec3(vpoint_mv));

    gl_ClipDistance[0] = height - texture(tex, uv).r; //to cut water

}

