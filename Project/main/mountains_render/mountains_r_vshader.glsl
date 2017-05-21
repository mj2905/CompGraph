#version 330

in vec2 position;

out vec4 vpoint_mv;
out vec3 vpoint;
out vec3 light_dir;
out float height;
out vec2 uv;

out vec4 shadow_pos_mvp;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat4 shadow_mvp;
uniform vec3 light_pos;

uniform sampler2D tex;

uniform bool clip;


void main() {
    uv = (position + vec2(1.0, 1.0)) * 0.5;

    height = texture(tex, uv).r;
    vpoint = vec3(position.x, height, clip ? position.y : -position.y);//-position.y);

    vpoint_mv = view * model * vec4(vpoint, 1.0);
    gl_Position = projection * vpoint_mv;
    light_dir = normalize(light_pos - vec3(vpoint_mv));
    shadow_pos_mvp = shadow_mvp * vec4(vpoint, 1.0);

}
