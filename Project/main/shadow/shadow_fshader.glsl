#version 330

out float fragmentDepth;

void main() {
    fragmentDepth = gl_FragCoord.z;
}
