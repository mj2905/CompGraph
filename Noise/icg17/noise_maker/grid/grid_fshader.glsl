#version 330

in vec2 uv;


in vec3 light_dir, view_dir;
in vec4 vpoint_mv;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

layout(location=0)out vec3 color;

uniform sampler2D tex;

void main() {

    vec3 p = vpoint_mv.xyz;
    float epsilonx = 1.0;
    float p1 = texture(tex, uv + vec2(epsilonx, epsilonx)).r;
    float p2 = texture(tex, uv + vec2(epsilonx, -epsilonx)).r;
    float p3 = texture(tex, uv + vec2(-epsilonx, epsilonx)).r;
    float p4 = texture(tex, uv + vec2(-epsilonx, -epsilonx)).r;
    vec3 v1 = vec3(-1,0,p1);
    vec3 v2 = vec3(1,0,p2);
    vec3 v3 = vec3(0,1, p3);
    vec3 v4 = vec3(0,-1, p4);
    vec3 normal_mv = normalize(cross(v2 - v1, v3- v4));

    //vec3 normal_mv = normalize(cross(dFdx(p), dFdy(p)));
    float nDotL = max(dot(normal_mv, light_dir), 0);

    vec3 r = normalize(reflect(- light_dir, normal_mv));
    float rDotV = max(dot(r, view_dir), 0);

    color = texture(tex, uv).r * La + kd * nDotL * Ld
                    + ks * pow(rDotV, alpha) * Ls;

}
