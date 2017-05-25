#version 330

in vec2 position;

out vec2 pos;
out vec4 vpoint_mv;
out vec3 light_dir, view_dir;
out float height;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;

uniform vec2 offset;

uniform sampler2D tex;

void main() {
    vec2 uv = (position + vec2(1.0, 1.0)) * 0.5;
    pos = uv + vec2(mod(offset.x, 1.0), mod(offset.y, 1.0));
    height = 0.4;

    vec3 pos_3d = vec3(position.x, height, -position.y);

    mat4 MV = view * model;

    vpoint_mv = MV * vec4(pos_3d, 1.0);
    gl_Position = projection * vpoint_mv;
    light_dir = normalize(mat3(MV) * light_pos - vpoint_mv.xyz);
    view_dir = normalize(- vec3(vpoint_mv));

    gl_ClipDistance[0] = height - texture(tex, uv).r; //to cut water

}

