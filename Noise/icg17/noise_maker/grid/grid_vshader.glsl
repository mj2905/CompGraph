#version 330

in vec2 vtexcoord;

out vec2 uv;
out vec4 vpoint_mv;

out vec3 light_dir, view_dir;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;


uniform mat4 MVP;
uniform float time;

uniform sampler2D tex;

void main() {
    uv = (vtexcoord + vec2(1.0, 1.0)) * 0.5;

    // convert the 2D position into 3D positions that all lay in a horizontal
    // plane.
    float height = texture(tex, uv).r;
    vec3 pos_3d = vec3(vtexcoord.x, height, -vtexcoord.y);

    vpoint_mv = view * model * vec4(pos_3d, 1.0);

    gl_Position = MVP * vec4(pos_3d, 1.0);

    light_dir = normalize(light_pos - vec3(vpoint_mv));
    view_dir = normalize(- vec3(vpoint_mv));
}
