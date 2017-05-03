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
uniform sampler2D ref;

in vec3 normal_t;

uniform float time;

void main() {

    float terrainHeight = texture(tex, uv).r;

    if(terrainHeight < height) {

        vec3 p = vpoint_mv.xyz;

        vec3 normal_mv = normalize(normal_t);//normalize(cross(dFdx(p), dFdy(p)));//normalize(texture(normal_map, 4*uv - time/100).xyz);//normalize(cross(dFdx(p), dFdy(p))); // normalize(test);////

        float nDotL = max(dot(normal_mv, light_dir), 0);

        vec3 r = normalize(reflect(- light_dir, normal_mv));
        float rDotV = max(dot(r, view_dir), 0);

        vec3 c = vec3(0.0, 0.0, .5);

        color.xyz = c * La;

        ivec2 window_dim = textureSize(ref, 0);
        vec2 window_rel = vec2(gl_FragCoord.x / window_dim.x, gl_FragCoord.y / window_dim.y);
        color.xyz = mix(color.xyz, texture(ref, window_rel).rgb, 0.5);
        color.a = 0.6;

        color.xyz +=  kd * nDotL * Ld + ks * pow(rDotV, alpha) * Ls;

    }
    else {
        discard;
    }

}
