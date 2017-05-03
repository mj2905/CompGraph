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

uniform float time;

out vec3 normal_t;

float getHeight(vec2 uv) {
    return 0.35 + (sin(2*M_PI*12*(uv.x + 2*uv.y) - 2*time)/500 + sin(2*M_PI*12*(2*uv.x + uv.y) -2*time)/500 + sin(2*M_PI*12*(uv.x + uv.y) -2*time)/500);
}

void main() {
    uv = (position + vec2(1.0, 1.0)) * 0.5;
    vec2 pos = uv + vec2(mod(offset.x, 1.0), mod(offset.y, 1.0));
    height = getHeight(uv);

    vec3 pos_3d = vec3(position.x, height, -position.y);

    normal_t = mat3(transpose(inverse(view * model))) * texture(normal_map, uv + time/1000).xzy; //xzy

    //test = mat3(transpose(inverse(view * model))) * texture(normal_map, 2*uv + time/1000).xyz;

    mat4 MV = view * model;

    vpoint_mv = MV * vec4(pos_3d, 1.0);
    gl_Position = projection * vpoint_mv;
    light_dir = normalize(mat3(MV) * light_pos - vpoint_mv.xyz);
    view_dir = normalize(- vec3(vpoint_mv));

}

