#version 330

in vec2 position;

out vec4 vpoint_mv;
out vec3 light_dir;
out float height;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;

uniform sampler2D tex;


void main() {
    vec2 uv = (position + vec2(1.0, 1.0)) * 0.5;

    height = texture(tex, uv).r;
    vec3 pos_3d = vec3(position.x, height, -position.y);

    vpoint_mv = view * model * vec4(pos_3d, 1.0);
    gl_Position = projection * vpoint_mv;
    light_dir = normalize(light_pos - vec3(vpoint_mv));

}

