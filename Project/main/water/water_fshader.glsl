#version 330

in vec2 pos;

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

uniform float time;

uniform float fog_threshold;

const float wave_strength = 0.02f;

uniform mat4 model;
uniform mat4 view;
uniform sampler2D normal_map;

vec3 mapXandZ(vec3 v) {
    return vec3(v.x * 2 - 1, v.y, v.z * 2 - 1);
}

const float repeatFactor = 18.0;

void main() {

    vec3 p = vpoint_mv.xyz;

    float moveFactor = mod(time/300, 1.0);
    vec3 vnormal = mapXandZ(texture(normal_map, repeatFactor * vec2(pos.x, pos.y - moveFactor)).rbg)
                    + mapXandZ(texture(normal_map, repeatFactor*vec2(-pos.x + moveFactor, pos.y + moveFactor)).rbg);
    vec3 vnormal_mv = normalize(mat3(transpose(inverse(view * model))) * vnormal);

    vec2 distorsion = wave_strength * ((texture(dudv, repeatFactor*vec2(pos.x, pos.y - moveFactor)).rg * 2 - 1)
                                        + (texture(dudv, repeatFactor*vec2(-pos.x + moveFactor, pos.y + moveFactor)).rg * 2 - 1));

    float nDotL = max(dot(vnormal_mv, light_dir), 0);

    vec3 r = normalize(reflect(- light_dir, vnormal_mv));
    float rDotV = max(dot(r, view_dir), 0);

    vec3 c = vec3(0.0, 0.0, .3);

    color.xyz = c * La;

    ivec2 window_dim = textureSize(ref, 0);

    vec2 window_rel = vec2(gl_FragCoord.x / window_dim.x, gl_FragCoord.y / window_dim.y);
    window_rel = clamp(window_rel + distorsion, 0, 1);
    vec4 reflection = texture(ref, window_rel);

    color = mix(color, reflection, 0.7f);
    color.xyz += kd * nDotL * Ld + mix(height*3, 0, 0.5) * ks * pow(rDotV, alpha) * Ls;
    color.a = 0.7;
}
