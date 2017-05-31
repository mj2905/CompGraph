in sampler2D tex;
in vec2 uv;
in float height;

out vec3 color;

void main(void)
{

    // adding a control on height afterwards
    color = texture(tex, uv).rgb;
}
