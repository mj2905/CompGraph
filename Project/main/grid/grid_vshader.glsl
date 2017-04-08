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

uniform sampler2D texBL;
uniform sampler2D texBR;
uniform sampler2D texUL;
uniform sampler2D texUR;

uniform vec2 offset;

float zoom = 1;

void main() {
    uv = (position + vec2(1.0, 1.0)) * 0.5;

    uv /= zoom;
    //uv +=offset;

    //vec2 xy = vec2(uv.x / zoom + offset.x, uv.y / zoom + offset.y);
    //vec2 d = vec2(mod(int(xy.x), 2) == 0 ? 1 : 0,
                  //mod(int(xy.y), 2) == 0 ? 1 : 0);
    //d = vec2(xy.x >= 0 ? d.x : 1-d.x, xy.y >= 0 ? d.y : 1-d.y);

    //uv = vec2(d.x == 0 ? mod(xy.x, 1.0) : 1 - mod(xy.x, 1.0), d.y == 0 ? mod(xy.y, 1.0) : 1-mod(xy.y, 1.0));

    // convert the 2D position into 3D positions that all lay in a horizontal
    // plane.
    // TODO 6: animate the height of the grid points as a sine function of the
    // 'time' and the position ('uv') within the grid.

    //height = texture(texBL, uv).r;
    vec2 xy = vec2(mod(offset.x, 1.0f), mod(offset.y, 1.0f)) - 1; //bet -1 and 0
    vec2 pos = uv + xy;
    if(pos.x <= 0 && pos.y <= 0) {
        height = texture(texBL, pos + 1).r;
    }
    else if(pos.x > 0 && pos.y <= 0) {
        height = texture(texBR, vec2(pos.x, pos.y + 1)).r;
    }
    else if(pos.x <= 0 && pos.y > 0) {
        height = texture(texUL, vec2(pos.x + 1, pos.y)).r;
    }
    else {
        height = texture(texUR, vec2(pos.x, pos.y)).r;
    }


/*
    if(uv.x < 1 - xy.x && uv.y < xy.y) {
        height = texture(texBL, uv + vec2(xy.x, xy.y)).r;
    }
    else if(uv.x > 1 - xy.x && uv.y < xy.y) {
        height = texture(texBR, uv + vec2(-xy.x, xy.y)).r;
    }
    else if(uv.x < 1-xy.x && uv.y > xy.y) {
        height = texture(texUL, uv + vec2(xy.x, -xy.y)).r;
    }
    else {
        height = texture(texUR, uv + vec2(-xy.x, -xy.y)).r;
    }
    */


    vec3 pos_3d = vec3(position.x, height, -position.y);

    vpoint_mv = view * model * vec4(pos_3d, 1.0);
    gl_Position = projection * vpoint_mv;
    light_dir = normalize(light_pos - vec3(vpoint_mv));
    view_dir = normalize(- vec3(vpoint_mv));



}

