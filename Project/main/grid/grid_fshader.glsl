#version 330

in vec2 uv;

out vec3 color;

in vec3 light_dir;
in vec3 view_dir;
in vec4 vpoint_mv;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

uniform sampler2D tex;

uniform sampler1D colormap;

void main() {

    vec3 p = vpoint_mv.xyz;

    vec3 normal_mv = normalize(cross(dFdx(p), dFdy(p)));

    float nDotL = max(dot(normal_mv, light_dir), 0);

    vec3 r = normalize(reflect(- light_dir, normal_mv));
    float rDotV = max(dot(r, view_dir), 0);

    color = texture(colormap, texture(tex, uv).r).rgb * La + kd * nDotL * Ld
                    + ks * pow(rDotV, alpha) * Ls;
}
