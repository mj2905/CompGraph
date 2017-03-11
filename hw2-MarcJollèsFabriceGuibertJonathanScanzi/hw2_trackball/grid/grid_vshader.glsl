#version 330

in vec2 position;

out vec2 uv;

uniform mat4 MVP;
uniform float time;

const float M_PI = 3.1415926535897932384626433832795;

#define Water (0)

void main() {
    uv = (position + vec2(1.0, 1.0)) * 0.5;

    // convert the 2D position into 3D positions that all lay in a horizontal
    // plane.
    // TODO 6: animate the height of the grid points as a sine function of the
    // 'time' and the position ('uv') within the grid.

#if(!Water)
    float frequency = 2.0f;
    float amplitude = 0.08f;

    float height = amplitude * sin(2 * M_PI * frequency * (uv.x + uv.y) + time);

#else
    float x = uv.x - 0.5;
    float y = uv.y - 0.5;
    float r = sqrt(x*x + y*y);

    float attenuation = 1/(1+mod(time/2.0f, 20));

    float frequency = 3.0f;
    float amplitude = attenuation*0.2f*exp(-3*r);

    float height =  amplitude*sin(2 * M_PI * frequency * r + 10*time);
#endif

    vec3 pos_3d = vec3(position.x, height, -position.y);

    gl_Position = MVP * vec4(pos_3d, 1.0);
}
