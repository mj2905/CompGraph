#version 330

out vec3 color;

in vec3 light_dir;
in vec4 vpoint_mv;
in vec3 vpoint;
in vec2 uv;
in float height;

uniform vec3 Ld;
uniform vec3 kd;
uniform float alpha;

uniform sampler2D grass;
uniform sampler2D rock;
uniform sampler2D snow;
uniform sampler2D sand;

uniform sampler1D colormap;


uniform bool clip;

uniform vec2 offset;

float sq(float x) {
    return x*x;
}

float grass_distrib(float height, float n) {
    if(height >= 0.3 && height <= 0.7) {
        return mix(n, 0, abs(height - 0.5) * 5)*1.3;
    }
    return 0;
}

float rock_distrib(float height, float n) {
        if(height >= 0.65 && height <= 0.85) {
            return mix(0.4, -n + 1, abs(height - 0.75) * 10);
        }
        return -n +1;
}

float snow_distrib(float height, float n) {
    if(height >= 0.8) {
        return mix(n, 0, abs(1 - height) * 5);
    }
    return 0;
}

float sand_distrib(float height, float n) {
    if(height <= 0.5) {
        return mix(n, 0, height*2)*1.2;
    }
    return 0;
}

uniform float fog_threshold;


void main() {

    vec3 p = vpoint_mv.xyz;

    vec3 normal_mv = normalize(cross(dFdx(p), dFdy(p)));
    vec3 normal = normalize(cross(dFdx(vpoint), dFdy(vpoint)));
    if(normal.y < 0) {
        normal = -normal;
    }

    float nDotL = max(dot(normal_mv, light_dir), 0);

    float alpha1=grass_distrib(height, normal.y),
          alpha2=rock_distrib(height, normal.y),
          alpha3=snow_distrib(height, normal.y),
          alpha4=sand_distrib(height, normal.y);

    if(clip && height < 0.4) {
        discard;
    }

    color =   0.9*(
              alpha1 * texture(grass, (uv + offset)*5).rgb
            + alpha2 * texture(rock, (uv + offset)*10).rgb
            + alpha3 * texture(snow, (uv + offset)*10).rgb
            + alpha4 * texture(sand, (uv + offset)*60).rgb
            + kd * nDotL * Ld); //computation of the color : we use the height, and we add the diffuse component so that we have shadings

            //float distance = gl_FragCoord.z;
            //if (distance > fog_threshold) {
              //color.xyz = mix(color.xyz, vec3(0.9,0.9,0.9), (distance-fog_threshold)*9);
            //}


}
