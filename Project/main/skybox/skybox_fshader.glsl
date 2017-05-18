#version 330 core
in vec3 TexCoords;
out vec4 color;

uniform samplerCube skybox;

void main()
{
    color = texture(skybox, TexCoords) + vec4(0.25,0.1, 0, 0)*1.5;
}
