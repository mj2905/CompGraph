

part 2 - Screen Space Reflections

For this part, we closely followed the handout steps. This means we first generated an inverted camera which would look at a reversed cube (with respect to the the floor). We then render the cube with this camera inside a special framebuffer. This framebuffer is bound to the floor like a texture, allowing use to manipulate it on the fragment shader. All it takes is to mix the regular floor texture with this framebuffer, then render the cube and floor with the usual camera (once the texture framebuffer has been unbound from our rendering).
