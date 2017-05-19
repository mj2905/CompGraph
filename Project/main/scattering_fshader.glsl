#version 330

// Comes from this website: http://fabiensanglard.net/lightScattering/

in vec3 vpoint;
uniform float exposure;
uniform float decay;
uniform float density;
uniform float weight;
uniform vec2 lightPositionOnScreen;
uniform sampler2D tex;
const int NUM_SAMPLES = 100 ;

out vec4 color;
void main()
{
        vec2 deltaTextCoord = vec2( vpoint.st - lightPositionOnScreen.xy );
        vec2 textCoo = vpoint.st;
        deltaTextCoord *= 1.0 /  float(NUM_SAMPLES) * density;
        float illuminationDecay = 1.0;


        for(int i=0; i < NUM_SAMPLES ; i++)
        {
                        textCoo -= deltaTextCoord;
                        vec4 sample = texture(tex, textCoo );

                        sample *= illuminationDecay * weight;

                        //color += sample;

                        illuminationDecay *= decay;
        }


        color = texture(tex, textCoo);
        //color *= exposure;
}
