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

uniform vec2 offset;

const float MULT = 2.5;

float grass_distrib(float height, float n) {
    return mix(1, 0, clamp(abs(height - 0.4)*MULT, 0.2, 1));
}

float rock_distrib(float height, float n) {
    return clamp(1-n + mix(1, 0, clamp(abs(0.7-height)*MULT*2, 0, 1)), 0, 1);
}

float snow_distrib(float height, float n) {
    return mix(1, 0, clamp(abs(0.9-height)*MULT*3, 0, 1));
}

float sand_distrib(float height, float n) {
    return mix(2, 0, clamp(abs(height-0.1)*MULT, 0, 1));
}

float[4] distributions(float height, float n) {
    float[4] result;
    result[0] = rock_distrib(height, n);
    float other = 1-result[0];
    result[1] = grass_distrib(height, n);
    result[2] = snow_distrib(height, n);
    result[3] = sand_distrib(height, n);

    float tot = result[1] + result[2] + result[3];
    result[1] *= other/tot;
    result[2] *= other/tot;
    result[3] *= other/tot;
    return result;
}


/*
float grass_distrib(float height, float n) {
    if(height >= 0.3 && height <= 0.7) {
        return mix(n, 0, abs(height - 0.5) * 5);
    }
    return 0;
}

float rock_distrib(float height, float n) {
        if(height >= 0.6 && height <= 0.8) {
            return mix(1, -n + 1, abs(height - 0.7) * 10);
        }
        return 1-n;
}

float snow_distrib(float height, float n) {
    if(height >= 0.7) {
       float r = mod(n, 0.1)*2-0.1;
       return mix(0, 0.9, clamp((height-0.7)*10 + r, 0, 1));
    }
    return 0;
}

float sand_distrib(float height, float n) {
    if(height <= 0.5) {
        return mix(n, 0, clamp(height, 0, 1)*2);
    }
    return 0;
}
*/

uniform float fog_threshold;


void main() {

    vec3 p = vpoint_mv.xyz;

    vec3 normal_mv = normalize(cross(dFdx(p), dFdy(p)));
    vec3 normal = normalize(cross(dFdx(vpoint), dFdy(vpoint)));
    if(normal.y < 0) {
        normal = -normal;
    }

    float nDotL = max(dot(normal_mv, light_dir), 0);

    float normal_y = normal.y*2;//int(normal.y*4)/4.0f;

    //float alpha1=grass_distrib(height, normal_y),
    //      alpha2=rock_distrib(height, normal_y),
    //      alpha3=snow_distrib(height, normal_y),
    //      alpha4=sand_distrib(height, normal_y);

    float[4] distribs = distributions(height, normal_y);


    color =
            (
              distribs[1] * texture(grass, (uv + offset)*30).rgb
            + distribs[0] * texture(rock, (uv + offset)*20).rgb
            + distribs[2] * texture(snow, (uv + offset)*30).rgb
            + distribs[3] * texture(sand, (uv + offset)*60).rgb)
            + kd * nDotL * Ld - 0.1; //computation of the color : we use the height, and we add the diffuse component so that we have shadings

            //float distance = gl_FragCoord.z;
            //if (distance > fog_threshold) {
              //color.xyz = mix(color.xyz, vec3(0.9,0.9,0.9), (distance-fog_threshold)*9);
            //}


}
