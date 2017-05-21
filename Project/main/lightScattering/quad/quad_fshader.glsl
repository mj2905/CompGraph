#version 330

in vec2 uv;

out vec3 color;

uniform sampler2D tex;

uniform int drawBlack;

void main() {
    if(drawBlack == 0){
        color = vec3(0.0,0.0,0.0);
    }else{
        color = texture(tex,uv).rgb;
    }

}
