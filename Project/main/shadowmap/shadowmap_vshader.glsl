#version 330 core

// Input vertex data, different for all executions of this shader.
in vec3 position;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 VP;
uniform mat4 M;

void main(){
 gl_Position =  MVP * vec4(position, 1.0);
}
