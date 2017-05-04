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
uniform sampler2D reflection;
uniform sampler1D colormap;

void main() {

    vec3 p = vpoint_mv.xyz;

    vec3 normal_mv = normalize(cross(dFdx(p), dFdy(p)));

    float nDotL = max(dot(normal_mv, light_dir), 0);

    vec3 r = normalize(reflect(- light_dir, normal_mv));
    float rDotV = max(dot(r, view_dir), 0);

    vec3 c = texture(colormap, (height - 0.4)*10).rgb;


    //reflection
    ivec2 window_dim = textureSize(reflection, 0);
    vec2 window_rel = vec2( (gl_FragCoord.x / float(window_dim.x))*0.5, (1 - gl_FragCoord.y / float(window_dim.y))*0.5 );

    color.xyz = mix(c * La + kd * nDotL * Ld + ks * pow(rDotV, alpha) * Ls, texture(reflection, window_rel).rrr, vec3(0.5));
    //color.xyz = texture(reflection, window_rel).xyz;
    //color.xyz = mix(c * La + kd * nDotL * Ld + ks * pow(rDotV, alpha) * Ls, vec3(1.0,1.0,1.0), vec3(.2));
    color.a = texture(tex, uv).r < height ? 0.6 : 0.0;
    //color.a = 1;
}
