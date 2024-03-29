#version 330 core
out vec3 color;
in vec2 uv;
uniform sampler2D tex;
uniform sampler2D tex_mirror;
in vec4 gl_FragCoord;

void main() {
    /// TODO: query window_width/height using the textureSize(..) function on tex_mirror
    /// TODO: use gl_FragCoord to build a new [_u,_v] coordinate to query the framebuffer
    /// NOTE: make sure you normalize gl_FragCoord by window_width/height
    /// NOTE: you will have to flip the "v" coordinate as framebuffer is upside/down
    /// TODO: mix the texture(tex,uv).rgb with the value you fetch by texture(tex_mirror,vec2(_u,_v)).rgb
    //color = texture(tex,uv).rgb;

    ivec2 window_dim = textureSize(tex_mirror, 0);
    vec2 window_rel = vec2(gl_FragCoord.x / window_dim.x, 1 - gl_FragCoord.y / window_dim.y);
    color = mix(texture(tex,uv).rgb, texture(tex_mirror, window_rel).rgb, vec3(.15));
}
