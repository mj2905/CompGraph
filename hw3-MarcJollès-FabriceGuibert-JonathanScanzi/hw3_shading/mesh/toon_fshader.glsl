#version 330


out vec3 color;

uniform sampler1D tex1D;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

in vec3 normal_mv;
in vec3 light_dir;
in vec3 view_dir;

void main() {
    color = vec3(0.0,0.0,0.0);

    ///>>>>>>>>>> TODO >>>>>>>>>>>
    /// TODO 2.2: Toon shading.
    /// 1) compute ambient term.
    /// 2) compute diffuse term using the texture sampler tex.
    /// 3) compute specular term using the texture sampler tex.
    ///<<<<<<<<<< TODO <<<<<<<<<<<

    vec3 normal = normalize(normal_mv);
    vec3 light = normalize(light_dir);
    vec3 view = normalize(view_dir);

    float nDotL = texture(tex1D, max(dot(normal, light), 0.0f)).r;

    vec3 r = normalize(reflect(-light, normal));
    float rDotVPowAlpha = texture(tex1D, pow(max(dot(r, view), 0.0f), alpha)).r;

    color = ka * La + kd * nDotL * Ld
                    + ks * rDotVPowAlpha * Ls;
}
