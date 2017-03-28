1 Fast Gaussian Blur

For this part, we had to work on multiple files.

- The framebuffer :
We want here to deal with multiple attachments. Thus, now, in the Init, we return a tuple of texture ids : one for the first attachment and the other for the other attachment. Furthermore, in the Bind method, we now give an argument, corresponding to the attachment we want to write to.

- The main.cpp :
We work now with two screenquads : one creating the horizontal blur from the first attachment, outputting it to the second attachment, and the other doing the vertical blur from the second attachment, outputting it to the screen. When we get the tuple back from the frame buffer init, we assign then the first component to the first screen quad texture, and the second component to the second screen quad texture. These textures, for the screen quads, correspond to the input texture.

Then in Display, when we draw the plane and the quad, we do it in the first attachment of the framebuffer, and when we draw the first screenquad, we do it in the second attachment of the framebuffer. The second screenquad is directly drawn outside any Bind, thus in the screen. 

We compute also the G matrix, using the same formula as in the lab5, but we store it to a vector we give to the screenquads, so that they can give it to the shaders to compute the blur. We see that whenever a Q/W key is pressed, it calls the regeneration of the G matrix, which is then passed to the screen quads, so that we don't do it at every Display step.

- screenquad.h :

Here, we take one more parameter : is_horizontal. It's useful to know if we will use this screenquad to make an horizontal or vertical blur. It's directly given to the shader.

- screenquad_fshader :

Here we use the same formula as in the gaussian blur, but we don't do an inner loop, we only make one loop. We also use the G array given. Here, as we can see, there is a fixed maximum size (here 400). In the main.cpp there is are boundaries for the gaussian standard deviation so that there is no problem with this maximum size. As we can see, the is_horizontal given from the screen quad is useful to compute a direction vector, which is used to modify the formula if we are in the vertical or horizontal blur. 


2 Screen Space Reflections

For this part, we closely followed the handout steps. This means we first generated an inverted camera which would look at a reversed cube (with respect to the the floor). We then render the cube with this camera inside a special framebuffer. This framebuffer is bound to the floor as a texture (tex_mirror), allowing us to manipulate it in the floor fragment shader. We then need to mix the regular floor texture with this framebuffer, then render the cube and floor with the usual camera (once the texture framebuffer has been unbound from our rendering).
As gl_FragCoord is between 0 and the dimensions of the window, we divide it by the dimensions of the window to get a vector with two components between 0 and 1, and as we want to have the cube going the other way for the mirror (as the mirrored camera films it spinning in the wrong direction), we flip the v coordinate by doing 1 - v.

3 Motion Blur


