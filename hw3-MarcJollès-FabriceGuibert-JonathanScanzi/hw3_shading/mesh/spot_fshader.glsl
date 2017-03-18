#version 330


out vec3 color;


const float spot_cos_cutoff = 0.985; // cos 10 degrees
const float spot_exp = 150;

in vec3 normal_mv;
in vec3 light_dir;
in vec3 view_dir;
in vec3 vpoint_mv3;

uniform vec3 spot_dir;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

void main() {    
    /*
    const vec3 COLORS[6] = vec3[](
        vec3(1.0,0.0,0.0),
        vec3(0.0,1.0,0.0),
        vec3(0.0,0.0,1.0),
        vec3(1.0,1.0,0.0),
        vec3(0.0,1.0,1.0),
        vec3(1.0,0.0,1.0));
    int index = int( mod(gl_PrimitiveID,6) );
    color = COLORS[index];
    */

    //>>>>>>>>>> TODO >>>>>>>>>>>
    // TODO 5: Spot light.
    // Complete the shader to obtain spot light effect
    //<<<<<<<<<< TODO <<<<<<<<<<<

    float nDotL = max(dot(normal_mv, light_dir), 0);

    vec3 r = normalize(reflect(- light_dir, normal_mv));
    float rDotV = max(dot(r, view_dir), 0);

    color = ka * La + kd * nDotL * Ld
                    + ks * pow(rDotV, alpha) * Ls;

    float spot_effect = pow(dot(normalize(light_dir), normalize(spot_dir)), spot_exp);

    color = ((dot(normalize(-vpoint_mv3), normalize(spot_dir)) > spot_cos_cutoff) ? color*spot_effect : ka * La);

}
