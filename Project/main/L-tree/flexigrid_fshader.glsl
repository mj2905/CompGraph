#version 330

out vec4 color;
in vec3 normal_mv;
in vec3 light_dir;
in vec3 view_dir;
uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

void main() {

    float nDotL = max(dot(normal_mv, light_dir), 0);

    vec3 r = normalize(reflect(- light_dir, normal_mv));
    float rDotV = max(dot(r, view_dir), 0);

    vec3 c = ka * La + kd * nDotL * Ld
                    + ks * pow(rDotV, alpha) * Ls;

    color = vec4(c.x,c.y,c.z, 1.0);
}
