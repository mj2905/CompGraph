#version 330

in vec2 uv;

uniform sampler2D tex;
uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;


uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;

in vec4 vpoint_mv;
in vec3 light_dir, view_dir;


layout(location=0)out vec3 color;

void main() {
    vec3 dfx = dFdx(vpoint_mv.xyz);
    vec3 dfy = dFdy(vpoint_mv.xyz);

    vec3 n = normalize(cross(dfx, dfy));
    vec3 r = normalize(reflect(-light_dir, n));
    vec3 amb = ka*La;
    vec3 diff = kd*(max(dot(normalize(n), normalize(light_dir)), 0.0))*Ld;
    vec3 spec = ks*(pow(max(dot(r, normalize(light_dir)),0.0), alpha))*Ls;
    vec3 c = amb + diff + spec;

    color = c;
}
