#version 330 core
in vec2 uv;
uniform sampler2D colorTex;
uniform sampler2D velocityTex;
out vec4 color;

int nb_samples = 30;

void main() {
    /// TODO: use the velocity vector stored in velocityTex to compute the line integral
    /// TODO: use a constant number of samples for integral (what happens if you take too few?)
    /// HINT: you can scale the velocity vector to make the motion blur effect more prominent
    /// HINT: to debug integration don't use the velocityTex, simply assume velocity is constant, e.g. vec2(1,0)

    vec2 velocity = 100*texture(velocityTex, uv).xy;

    ivec2 window_dim = textureSize(colorTex, 0);

    vec2 xy = uv;
    vec3 c = vec3(0);
    for(int i = 0; i < nb_samples; ++i) {
        c += texture(colorTex, xy).rgb;
        xy -= velocity/window_dim;
    }
    c /= nb_samples;

    color = vec4(c, 1);
}
