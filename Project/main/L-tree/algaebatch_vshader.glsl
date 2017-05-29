in vec2 position;

out vec4 vpoint_mv;
out vec3 vpoint;
out vec3 light_dir;
out float height;
out vec2 uv;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;

uniform sampler2D tex;

uniform bool clip;


void main() {
    uv = (position + vec2(1.0, 1.0)) * 0.5;

    height = texture(tex, uv).r;

    if(clip) {
        gl_ClipDistance[0] = height - 0.4;
    }

    vpoint = vec3(position.x, height, clip ? position.y : -position.y);

    mat4 MV = view * model;

    vpoint_mv = MV * vec4(vpoint, 1.0);
    gl_Position = projection * vpoint_mv;
    light_dir = normalize(mat3(MV) * light_pos - vec3(vpoint_mv));
}

