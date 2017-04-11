#version 410 core
//you need to specify the OpenGL version, of course
//specify the number of vertices per patch
layout (vertices = 3) out;

in vec4 vpoint_mv [];
patch out vec4 vpoint_mv_eva;

in vec3 view_dir [];
patch out vec3 view_dir_eva;

in vec3 light_dir [];
patch out vec3 light_dir_eva;

in float height [];
patch out float height_eva;

float level (vec4 poz1, vec4 poz2){
    float lod=1;
        float d=distance(poz1, poz2);
        if(d<10) lod=10;
        if(10<=d && d<30) lod=5;
        if(30<=d && d<50) lod=2;
        if(50<=d) lod=1;

        return lod;
}

void main(void){
    if (gl_InvocationID == 0){
        gl_TessLevelInner[0] = 2.0;
        gl_TessLevelOuter[0] = 2.0;
        gl_TessLevelOuter[1] = 2.0;
        gl_TessLevelOuter[2] = 2.0;

//in case of quad, you have to specify both gl_TessLevelInner[1] and //gl_TessLevelOuter[3]
    }

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    vpoint_mv_eva = vpoint_mv[gl_InvocationID];
    view_dir_eva = view_dir[gl_InvocationID];
    light_dir_eva = light_dir[gl_InvocationID];
    height_eva = height[gl_InvocationID];

    vec3 eye_position = vec3(10,0,0);




    if(gl_InvocationID == 0){
    vec3 d1=gl_in[1].gl_Position.xyz+(gl_in[2].gl_Position.xyz-gl_in[1].gl_Position.xyz)/2;
    vec3 d2=gl_in[0].gl_Position.xyz+(gl_in[2].gl_Position.xyz-gl_in[0].gl_Position.xyz)/2;
    vec3 d3=gl_in[0].gl_Position.xyz+(gl_in[1].gl_Position.xyz-gl_in[0].gl_Position.xyz)/2;

    float e0=level(vec4(d1,1.0),vec4(eye_position,1.0));
    float e1=level(vec4(d2,1.0),vec4(eye_position,1.0));
    float e2=level(vec4(d3,1.0),vec4(eye_position,1.0));
    float m=min(e0,min(e1,e2));

    gl_TessLevelInner[0] = floor((min(e0,min(e1,e2))+max(e0,max(e1,e2)))/2);
    gl_TessLevelOuter[0] = e0;
    gl_TessLevelOuter[1] = e1;
    gl_TessLevelOuter[2] = e2;
}
}
