#version 330

out vec3 color;

in vec3 light_dir;
in vec4 vpoint_mv;
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

uniform vec2 offset;

float grass_distrib(float height) {
    if(height >= 0.3 && height <= 0.7) {
        return (-abs(height - 0.5) + 0.2)*5;
    }
    return 0;
}

float rock_distrib(float height) {
    if(height >= 0.6 && height <= 0.9) {
        return (-abs(height - 0.75) + 0.15)*6;
    }
    return 0;
}

float snow_distrib(float height) {
    if(height >= 0.75) {
        return (-abs(height - 1) + 0.25)*4;
    }
    return 0;
}

float sand_distrib(float height) {
    if(height <= 0.5) {
        return (-abs(height - 0.25) + 0.25)*4;
    }
    return 0;
}

void main() {

    vec3 p = vpoint_mv.xyz;

    vec3 normal_mv = normalize(cross(dFdx(p), dFdy(p)));

    float nDotL = max(dot(normal_mv, light_dir), 0);

    float alpha1=grass_distrib(height),
          alpha2=rock_distrib(height),
          alpha3=snow_distrib(height),
          alpha4=sand_distrib(height);

    color =   alpha1 * texture(grass, (uv + offset)*20).rgb
            + alpha2 * texture(rock, (uv + offset)*10).rgb
            + alpha3 * texture(snow, (uv + offset)*30).rgb
            + alpha4 * texture(sand, (uv + offset)*60).rgb
            + kd * nDotL * Ld; //computation of the color : we use the height, and we add the diffuse component so that we have shadings
}
