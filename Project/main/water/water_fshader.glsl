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

uniform sampler2D ref;
uniform sampler2D dudv;

in vec3 vnormal_mv;

uniform float time;

uniform float fog_threshold;

const float wave_strength = 0.03f;


void main() {

    vec3 p = vpoint_mv.xyz;

    vec3 vnormal_mv = normalize(vnormal_mv);

    float nDotL = max(dot(vnormal_mv, light_dir), 0);

    vec3 r = normalize(reflect(- light_dir, vnormal_mv));
    float rDotV = max(dot(r, view_dir), 0);

    vec3 c = vec3(0.0, 0.0, .3);

    color = vec4(c * La, 0.6f);

    float moveFactor = mod(time/500, 1.0);
    vec2 distorsion = wave_strength * (texture(dudv, 6*vec2(uv.x, uv.y - moveFactor)).rg * 2 - 1)
            + wave_strength * (texture(dudv, 6*vec2(-uv.x + moveFactor, uv.y + moveFactor)).rg * 2 - 1);

    ivec2 window_dim = textureSize(ref, 0);

    vec2 window_rel = vec2(gl_FragCoord.x / window_dim.x, gl_FragCoord.y / window_dim.y);
    window_rel = clamp(window_rel + distorsion, 0.001, 0.999);
    vec4 reflection = texture(ref, window_rel);

    color = mix(color, reflection, 0.5);
    color.xyz += kd * nDotL * Ld + mix(0, 0.5, height*3) * ks * pow(rDotV, alpha) * Ls;

}
