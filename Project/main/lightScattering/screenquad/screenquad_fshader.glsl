#version 330


// This code comes from GPU Gems 3 for the shading part, as explained in the chapter about volumetric light scattering, page 280

in vec2 uv;

out vec4 color;

uniform sampler2D tex;
uniform sampler2D tex2;
uniform int pass;
uniform int RENDER_RES;

uniform vec2 light_position;

const float exposure = 0.0024f;
const float decay = 0.99;
const float density = 0.4f;
const float weight = 5.7f;



const int NUM_SAMPLES = 128;

void main()
{
    vec2 deltaTextCoord =uv/RENDER_RES - light_position;
    vec2 textCoord = uv/RENDER_RES;
    deltaTextCoord *= 1.0 /  float(NUM_SAMPLES) * density;
    float illuminationDecay = 1.0;

    vec3 c = texture(tex,uv/RENDER_RES).rgb;

    for(int i=0; i < NUM_SAMPLES; i++){
        textCoord -= deltaTextCoord;
        vec3 s;
        s = texture(tex, textCoord).rgb;
        s*= illuminationDecay*weight;
        c+=s;
        illuminationDecay *= decay;
    }

    //color = vec4(c*exposure, 1.0f);
    color =  mix(vec4(c*exposure, 1.0f), texture(tex2,uv),0.5);
}
