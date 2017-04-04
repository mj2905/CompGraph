#version 330

in vec2 position;

out vec2 uv;
out vec4 vpoint_mv;
out vec3 light_dir, view_dir;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;

uniform sampler2D tex;

uniform vec2 offset;

float zoom = 2;

void main() {
    uv = (position + vec2(1.0, 1.0)) * 0.5;
    uv +=offset;

    //vec2 xy = vec2(uv.x / zoom + offset.x, uv.y / zoom + offset.y);
    //vec2 d = vec2(mod(int(xy.x), 2) == 0 ? 1 : 0,
                  //mod(int(xy.y), 2) == 0 ? 1 : 0);
    //d = vec2(xy.x >= 0 ? d.x : 1-d.x, xy.y >= 0 ? d.y : 1-d.y);

    //uv = vec2(d.x == 0 ? mod(xy.x, 1.0) : 1 - mod(xy.x, 1.0), d.y == 0 ? mod(xy.y, 1.0) : 1-mod(xy.y, 1.0));

    // convert the 2D position into 3D positions that all lay in a horizontal
    // plane.
    // TODO 6: animate the height of the grid points as a sine function of the
    // 'time' and the position ('uv') within the grid.

    float height = texture(tex, uv).r;
    vec3 pos_3d = vec3(position.x, height, -position.y);

    vpoint_mv = view * model * vec4(pos_3d, 1.0);
    gl_Position = projection * vpoint_mv;
    light_dir = normalize(light_pos - vec3(vpoint_mv));
    view_dir = normalize(- vec3(vpoint_mv));



}

