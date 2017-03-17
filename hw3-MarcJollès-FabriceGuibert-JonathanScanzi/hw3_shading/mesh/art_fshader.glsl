#version 330


out vec3 color;

uniform float alpha;
uniform sampler2D tex2D;

in vec3 normal_mv;
in vec3 light_dir;
in vec3 view_dir;

void main() {
    color = vec3(0.0, 0.0, 0.0);

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
    // TODO 3.2: Artistic shading.
    // 1) compute the output color by doing a look-up in the texture using the
    //    texture sampler tex.
    //<<<<<<<<<< TODO <<<<<<<<<<<

    vec3 normal = normalize(normal_mv);
    vec3 light = normalize(light_dir);
    vec3 view = normalize(view_dir);

    vec3 r = normalize(reflect(-light, normal));

    color = texture(tex2D, vec2(max(dot(normal, light), 0.0f), pow(max(dot(r, view), 0.0f), alpha))).rgb;

}
