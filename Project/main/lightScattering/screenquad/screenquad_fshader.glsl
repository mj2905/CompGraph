#version 330


// This code comes from GPU Gems 3 for the shading part, as explained in the chapter about volumetric light scattering, page 280

in vec2 uv;

out vec4 color;

uniform sampler2D tex;
uniform sampler2D tex2;
uniform int pass;

uniform vec2 light_position;

const float exposure = 0.0034f;
const float decay = 0.98;
const float density = 0.65f;
const float weight = 4.7f;



const int NUM_SAMPLES = 100;

void main()
{
    vec2 deltaTextCoord =uv - light_position;//vec2(0.5,0.5);
    vec2 textCoord = uv;
    deltaTextCoord *= 1.0 /  float(NUM_SAMPLES) * density;
    float illuminationDecay = 1.0;

    vec3 c = texture(tex,uv).rgb;

    for(int i=0; i < NUM_SAMPLES; i++){
        textCoord -= deltaTextCoord;
        vec3 s;
        s = texture(tex, textCoord).rgb;


        s*= illuminationDecay*weight;
        c+=s;

        illuminationDecay *= decay;
    }

    //color = vec4(c*exposure, 1.0f);
    color =  mix(vec4(c*exposure, 1.0f), texture(tex2,uv),0.65);
}
