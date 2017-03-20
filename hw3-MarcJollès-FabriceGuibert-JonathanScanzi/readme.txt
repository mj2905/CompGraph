1 Phong Shading
	
In the vertex shader, we need to compute the inverse of the transpose of the model_view matrix, to correctly compute the normals for each vertex. Then, for the light, as we want to point toward the light, we do light_pos - vpoint_mv.
For the vire_dir, it's the same, however, as vpoint_mv is already in the camera space, we only do -vpoint_mv, as the camera is at (0,0,0).

We pay attention to normalize the vectors at each step.

In the fragment shader, we just use the formula given the interpolation of what was previously computed in the vertex shader. 

We pay attention to clamp the dot products to positive values.


2 Toon Shading

For the vertex shader, we have exactly the same shader as in the phong one.

For the binding, we just have to retrieve the uniform location tex1D in mesh.h, and assign the texture n°0 to it.

For the fragment shader, as we are given a sampler, we use it with texture(tex1D, value).r
For the values, we use the two dot products, and we get as output the dot products sampled.
Then, we just apply the formula, as in the phong fragment shader.


3 Artistic Shading

For the vertex shader, we have exactly the same shader as in the phong one.

For the binding, we just have to retrieve the uniform location tex2D in mesh.h, and assign the texture n°1 to it.

For the fragment shader, as we are given a sampler, we use it with texture(tex1D, value).rgb
To generate the color, we just use for the value a combination of the dot product n.l for the x component, and the (r.v)^alpha for the y component of the vec2.


4 Flat Shading

For the vertex shader, we have exactly the same shader as in the phong one, without the normal, as we want to compute the normal per fragment.

We want to use dFdx and dFdy to get the two side vectors, and by doing the cross product of these vectors, we get the normal vector. For this, we take an input value which changes for each fragment, so we take the vpoint_mv input.

Then, we do the same calculus as in the phong model.


5 Spot Light Shading

For the vertex shader, we use the same as in the phong vertex shader, but we add as output the vpoint_mv, as we need it to know if we are inside the cone.

In the fragment shader, we do the same computations as in the phong fragment shader (without the ambient term), but in addition, we need to compute the spot effect using the formula in the statement (but instead of using the light dir, we use the view direction), and to know if we are in the cone, we do the dot product between the view direction and the spot direction (normalized) to get the cos(angle between them), and we compare it with the cos_cutoff.

We add the ambient term in the end, as whenever we are or not in the cone, we have this component.