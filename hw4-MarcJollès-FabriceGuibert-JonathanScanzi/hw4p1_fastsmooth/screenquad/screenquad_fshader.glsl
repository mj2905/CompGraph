#version 330

in vec2 uv;

out vec3 color;

const int MAX_SIZE = 400; // /!\ modify too in main if modified

uniform int SIZE_OPT;
uniform float G[MAX_SIZE];

uniform bool is_horizontal;

uniform sampler2D tex;
uniform float tex_width;
uniform float tex_height;

float rgb_2_luma(vec3 c) {
    return 0.3*c[0] + 0.59*c[1] + 0.11*c[2];
}

//Type 0 : edge detection
//Type 1 : gaussian blur
//Type 2 : gaussian blur optimized
#define Type 2

void main() {
#if Type == 0
    // x=-1
    float t_00 = rgb_2_luma( textureOffset(tex, uv, ivec2(-1, -1)).rgb );
    float t_01 = rgb_2_luma( textureOffset(tex, uv, ivec2(-1,  0)).rgb );
    float t_02 = rgb_2_luma( textureOffset(tex, uv, ivec2(-1, +1)).rgb );
    // x=0
    float t_10 = rgb_2_luma( textureOffset(tex, uv, ivec2( 0, -1)).rgb );
    float t_12 = rgb_2_luma( textureOffset(tex, uv, ivec2( 0, +1)).rgb );
    // x=+1
    float t_20 = rgb_2_luma( textureOffset(tex, uv, ivec2(+1, -1)).rgb );
    float t_21 = rgb_2_luma( textureOffset(tex, uv, ivec2(+1,  0)).rgb );
    float t_22 = rgb_2_luma( textureOffset(tex, uv, ivec2(+1, +1)).rgb );
    // gradients
    float sx = -1*t_00 -2*t_01 -1*t_02    +1*t_20 +2*t_21 +1*t_22;
    float sy = -1*t_00 -2*t_10 -1*t_20    +1*t_02 +2*t_12 +1*t_22;
    float g = 1-sqrt(sx*sx + sy*sy);
    
    // composite
    // color = texture(tex,uv).rgb; ///< passthrough shading
    // color = abs( vec3(sx, sx, sx) ); ///< derivatives x
    // color = abs( vec3(sy, sy, sy) ); ///< derivatives y
    color = vec3(g, g, g);    

#elif Type == 1
    // gaussian convolution
    float std = 2; // standard deviation (<.3 disable)
    // float std = .1; // standard deviation (<.3 disable)
    vec3 color_tot = vec3(0,0,0);
    float weight_tot = 0;
    int SIZE = 1 + 2 * 3 * int(ceil(std));
    for(int i=-SIZE; i<=SIZE; i++){
        for(int j=-SIZE; j<=SIZE; j++){
            float w = exp(-(i*i+j*j)/(2.0*std*std*std*std));
            vec3 neigh_color = texture(tex, uv+vec2(i/tex_width,j/tex_height)).rgb;
            color_tot += w * neigh_color; 
            weight_tot += w;
        }
    }
    color = color_tot / weight_tot; // ensure \int w = 1

#else

    vec2 direction;
    if(is_horizontal) {
        direction = vec2(1, 0);
    }
    else {
        direction = vec2(0, 1);
    }

    vec3 color_tot = vec3(0);
    float weight_tot = 0;
    //We choose these boundaries, as they are the same as for basic gaussian blur
    for(int i = -SIZE_OPT; i <= SIZE_OPT; ++i){
        float w = G[i + SIZE_OPT];
        vec3 neigh_color = texture(tex, uv + direction*vec2(i/tex_width,i/tex_height)).rgb; //direction is (0, 1) or (1, 0)
        color_tot += w * neigh_color;
        weight_tot += w;
    }
    color = color_tot / weight_tot; // ensure \int w = 1


#endif
}

