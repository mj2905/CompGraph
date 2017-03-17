#version 330


out vec3 color;

in vec3 light_dir;
in vec3 view_dir;
in vec4 vpoint_mv;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

void main() {
    color = vec3(0.0,0.0,0.0);
    const vec3 COLORS[6] = vec3[](
        vec3(1.0,0.0,0.0),
        vec3(0.0,1.0,0.0),
        vec3(0.0,0.0,1.0),
        vec3(1.0,1.0,0.0),
        vec3(0.0,1.0,1.0),
        vec3(1.0,0.0,1.0));
    int index = int( mod(gl_PrimitiveID,6) );
    color = COLORS[index];

    ///>>>>>>>>>> TODO >>>>>>>>>>>
    /// TODO 4.2: Flat shading.
    /// 1) compute triangle normal using dFdx and dFdy
    /// 1) compute ambient term.
    /// 2) compute diffuse term.
    /// 3) compute specular term.
    ///<<<<<<<<<< TODO <<<<<<<<<<<

    vec3 p = vpoint_mv.xyz;

    vec3 normal_mv = normalize(cross(dFdx(p), dFdy(p)));

    float nDotL = max(dot(normal_mv, light_dir), 0);

    vec3 r = normalize(reflect(- light_dir, normal_mv));
    float rDotV = max(dot(r, view_dir), 0);

    color = ka * La + kd * nDotL * Ld
                    + ks * pow(rDotV, alpha) * Ls;
}
