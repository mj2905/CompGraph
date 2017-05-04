#version 330

in vec2 uv;

out vec4 color;

in vec3 light_dir;
in vec3 view_dir;
in vec4 vpoint_mv;
in float height;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

uniform sampler2D tex;
uniform sampler1D colormap;

uniform float fog_threshold;

void main() {

    vec3 p = vpoint_mv.xyz;

    vec3 normal_mv = normalize(cross(dFdx(p), dFdy(p)));

    float nDotL = max(dot(normal_mv, light_dir), 0);

    vec3 r = normalize(reflect(- light_dir, normal_mv));
    float rDotV = max(dot(r, view_dir), 0);

    vec3 c = texture(colormap, (height - 0.4)*10).rgb;

    color.xyz = c * La + kd * nDotL * Ld + ks * pow(rDotV, alpha) * Ls;
    color.a = texture(tex, uv).r < height ? 0.6 : 0.0;

    float distance = gl_FragCoord.z;
    if (distance > fog_threshold) {
      color.xyz = mix(color.xyz, vec3(0.85,0.85,0.85), (distance-fog_threshold)*5);
    }
}
