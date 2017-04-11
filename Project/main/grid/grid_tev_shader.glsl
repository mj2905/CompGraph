#version 410 core
layout (triangles, equal_spacing, cw) in;

patch in vec4 vpoint_mv_eva;
out vec4 vpoint_mv_;

patch in vec3 view_dir_eva;
out vec3 view_dir_;

patch in vec3 light_dir_eva;
out vec3 light_dir_;

patch in float height_eva;
out float height_;

void main(void){
    gl_Position=(gl_TessCoord.x*gl_in[0].gl_Position+gl_TessCoord.y*gl_in[1].gl_Position+gl_TessCoord.z*gl_in[2].gl_Position);

    vpoint_mv_ = vpoint_mv_eva;
    view_dir_ = view_dir_eva;
    light_dir_ = light_dir_eva;
    height_ = height_eva;
}
