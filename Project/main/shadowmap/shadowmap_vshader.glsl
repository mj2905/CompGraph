#version 330 core

// Input vertex data, different for all executions of this shader.
in vec3 vertexPosition_modelspace;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

void main(){
 gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
}
