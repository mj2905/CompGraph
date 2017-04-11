#version 410

out vec3 color;

in vec3 light_dir_;
in vec3 view_dir_;
in vec4 vpoint_mv_;
in float height_;

uniform vec3 La, Ld;
uniform vec3 ka, kd;
uniform float alpha;

uniform sampler1D colormap;

void main() {

    vec3 p = vpoint_mv_.xyz;

    vec3 normal_mv = normalize(cross(dFdx(p), dFdy(p)));

    float nDotL = max(dot(normal_mv, light_dir_), 0);

    color = texture(colormap, height_).rgb * La + kd * nDotL * Ld; //computation of the color : we use the height, and we add the diffuse component so that we have shadings
}
