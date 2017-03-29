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


--------

Alternative option: 1 FBO with 2 attachments approach

The idea of the code is to do ping ponging between two textures. Ping pong shading is extensively described for the purpose of post processing and multi-pass rendering; this is why we chose this approach.


- The FBO

We need 2 attachments in one FBO. 
To do so, we must modify the original FBO to make it return not one but two texture ids and bind two textures as well.
We must also make sure the FBO holds two attachments and not one, and that we draw both of them (in the Bind() method).
Finally, it is necessary to modify the Clear and Cleanup methods to clear both attachments.

With this, the FBO carries two attachments, and one texture per attachment.


- Storing the textures

To store textures in a framebuffer (instead of directly outputting it to the screen), the related
draw operations must be between framebuffer.Bind(){  and } framebuffer.Unbind().
This way, the program automatically will store every generated texture into one of the FBO attachments.
What we want for the ping-pong algorithm is:
1) Render the usual scene as a texture and store it in the FBO
2) Read the result from the FBO, blur it in one direction and store it again in the FBO in texture2
3) Read texture2 from FBO, blur it in the other direction and output it to the screen.

It means we have to do one framebuffer.Bind()/framebuffer.Unbind() around the draw of the scene.
We then have to do another one around the first call to screenquad.Draw(), to generate the 
temporary texture.

We then have to call screenquad.Draw() one last time, to do the final blur.

All these steps are implemented in the Display method of main.cpp.

Notice that, as is, the screenquad has no idea of the pass we're in! (either ping or pong).
To fix this, the Draw() method is modified to take one parameter: an int, that describes whether
we're in pass 1 (ping) or pass 0 (pong).


- The ping-ponging

In the fshader of screenquad, we'll use the value of the pass.
Here, we create two output colors: color and color2, and bind them to 0 and 1.
It means that color will be the output color, color2 will write to the "temporary" texture.
This is the job of the following lines:

	layout(location=0) out vec3 color;
	layout(location=1) out vec3 color2;

Let's recap. We have two textures, tex and tex2. tex comes from the first binding of the FBO,
ie the drawing of the quad and the cube. We'd like to (when pass = 1) read tex, blur it horizontally, and write
the result to tex2. Then, on the next pass, when pass = 0, we want to read tex2, blur it vertically,
and write it to the output, ie to color.

These lines do exactly the job:

    if(pass==1){
        color2 = monoblur(tex, pass);
    }else{
        color = monoblur(tex2, pass);
    }

Monoblur is a function that, given a texture and the pass, will decide the direction of the blur on the texture
and return the resulting color.


- The Gaussian kernel

The Gaussian kernel is a 1D array, since we apply the convolution on one direction at a time.
It is generated in the Draw() function of the screenquad, and then passed to the fshader.

---------


2 Screen Space Reflections

For this part, we closely followed the handout steps. This means we first generated an inverted camera which would look at a reversed cube (with respect to the the floor). We then render the cube with this camera inside a special framebuffer. This framebuffer is bound to the floor as a texture (tex_mirror), allowing us to manipulate it in the floor fragment shader. We then need to mix the regular floor texture with this framebuffer, then render the cube and floor with the usual camera (once the texture framebuffer has been unbound from our rendering).
As gl_FragCoord is between 0 and the dimensions of the window, we divide it by the dimensions of the window to get a vector with two components between 0 and 1, and as we want to have the cube going the other way for the mirror (as the mirrored camera films it spinning in the wrong direction), we flip the v coordinate by doing 1 - v.

3 Motion Blur

For this problem, in the quad_fshader, we take the difference between the two positions (x and y coordinates), each position being divided by w as we want to be on the screen.

Then, in the screenquad_fshader, we take the texture values of the actual position, and of nb_samples positions in the opposite direction of the velocity vector, as this vector is pointing the move, and we want previous positions. We do the directional convolution with these positions, and we get the new color which is output.
