#version 330

in vec2 uv;


uniform sampler2D tex;
uniform sampler2D tex2;

uniform int pass;
uniform int size;

#define BOUND 256

layout(location=0) out vec3 color;
layout(location=1) out vec3 color2;

uniform float tex_width;
uniform float tex_height;
uniform float G[BOUND];


#define Type 2
float rgb_2_luma(vec3 c) {
    return 0.3*c[0] + 0.59*c[1] + 0.11*c[2];
}

vec3 monoblur(sampler2D t, int horizontal){
    vec3 color_tot = vec3(0,0,0);
    float weight_tot = 0;

    int s = 0;
    if(size > BOUND){
        s = BOUND/2;
    }else{
        s = size/2 ;
    }
    for(int i = -s; i<= s; i++){
        //int a = i - s/2;
        vec3 neigh_color = texture(t, uv + vec2(horizontal*i*1.0/tex_width, (1- horizontal)*i*1.0/tex_height)).rgb;
        color_tot += G[i+s]*neigh_color;
        weight_tot += G[i+s];
    }
    return color_tot / weight_tot;
}

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
    for(int i=-size; i<=size; i++){
        for(int j=-size; j<=size; j++){
            float w = exp(-(i*i+j*j)/(2.0*std*std*std*std));
            vec3 neigh_color = texture(tex, uv+vec2(i/tex_width,j/tex_height)).rgb;
            color_tot += w * neigh_color; 
            weight_tot += w;
        }
    }
    color = color_tot / weight_tot; // ensure \int w = 1
#else
    //opt gaussian convolution

    if(pass==1){
        color2 = monoblur(tex, pass);
    }else{
        color = monoblur(tex2, pass);
    }


#endif
}

