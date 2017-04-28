#version 330

in vec2 uv;
out vec4 color;
in vec3 normal_mv;
in vec3 light_dir;
in vec3 view_dir;
uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

void main() {
    float x = uv.x;
    float y = uv.y;
    float a = (1.0/y - 0.3);
    color = vec4(1.0f,0,0, a);
}

/*

out vec3 color;

in vec3 normal_mv;
in vec3 light_dir;
in vec3 view_dir;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;


void main() {
    color = vec3(0.0,0.0,0.0);

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
    // TODO 1.2: Phong shading.
    // 1) compute ambient term.
    // 2) compute diffuse term.
    // 3) compute specular term.
    // To avoid GPU bug, remove
    // the code above after
    // implementing Phong shading.
    //<<<<<<<<<< TODO <<<<<<<<<<<*/

   /* float nDotL = max(dot(normal_mv, light_dir), 0);

    vec3 r = normalize(reflect(- light_dir, normal_mv));
    float rDotV = max(dot(r, view_dir), 0);

    color = ka * La + kd * nDotL * Ld
                    + ks * pow(rDotV, alpha) * Ls;
}*/
