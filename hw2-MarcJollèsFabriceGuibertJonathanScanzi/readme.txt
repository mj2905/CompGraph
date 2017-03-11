1 Perspective Projection

For this part, we had to take the perspective projection matrix and to implement it in the PerspectiveProjection function.
We had to be careful about the angle, which is given in degrees and must be converted to radians. In addition, we had to put the matrix in a column-major order.

2 Trackball

For this part, we had to implement ProjectOntoSurface (with an x and a y, the aim is to define the z part, so that it's onto the trackball surface, or if it's outside the trackball, on a hyperbolic sheet). Thus, we only had to implement the formulas, according to if x^2 + y^2 is greater than r^2, or not.

We also had to implement Drag. From two vectors (the anchor and the current position), it defines the rotation which should be applied to the object.
To do this, we have to know the angle between the two vectors, which can be obtained by the dot product between these two vectors :
u . v = |u| * |v| * cos(angle_bet_them). To get the angle, we thus have to do :
angle_bet_them = acos( (u . v)/(|u| * |v|) )
We consider this angle as the magnitude of the rotation. Then, we can multiply the magnitude of the rotation by a SPEED factor (constant), so that we can have a quicker rotation. Then, we create the rotation matrix, which allows to turn around an axis (the cross product between the anchor and the current position) by the magnitude of the rotation.

Then, we allow the user to use the mouse to rotate the trackball : when the user clicks with the left mouse button, we have the x and y positions of the mouse. We transform them to normalized OpenGL coordinates, we give them to the Drag method of the trackball, and we apply this rotation to the old trackball matrix.

Finally, when the user uses the right mouse button, we want to have a zoom functionality. We do the same transformation of the x and y positions of the mouse to normalized coordinates. We then keep somewhere (in old_y) the previous coordinates of the mouse, so that we can translate the view matrix by the new y minus the old y. Thus, instead of zooming with respect to the position on the screen, we zoom with respect to the previous position on the screen.

3 Triangle Grid And Animation

For this part, we want to do a grid of 100x100 vertices. We have to use a loop to insert all vertices and indices to the vectors.
For the formulas, we will explain for a grid_dim of 3, but it can be easily generalized to a 100x100 grid.

In the loops, we have indices : x and y. They can be used to know which is the current point id using the formula x + (grid_dim)*y.

We know that these indices lie between 0 and grid_dim-1.

we want that for an index of 0, we get the coordinate -1, for the index grid_dim-1 a coordinate of 1 and homogeneously distributed coordinates between these two. 
We come to the formula (index - middle)/middle, with middle = (grid_dim-1)/2. 
A quick check tells us that these formulas work :

For a grid_dim of 3, we get middle = 1.
we then have the coordinates
(0-1)/1 = -1
(1-1)/1 = 0
(2-1)/1 = 1

We then can have for each new pair of indices (x,y) the coordinates (left, down) of a new and unique vertex.

Then, we must link these vertices between them, so that we have triangles.
If we do the following :
indices.push_back(x + (grid_dim)*y);
indices.push_back(x+1 + (grid_dim)*y);
indices.push_back(x+1 + (grid_dim)*(y+1));
We do link the point with its right-x-neighbor, and this neighbor with its up-y-neighbor. We thus have a triangle. 
We apply the same principle to the other triangle of the square.
As we know that the right-most and upper-most vertices don't have right-x and up-y neighbors, we don't do the triangle link (they were already linked to other triangles, and we don't need to have more triangles, we already have our complete grid).

For the animation part, all we have to do is to use uv.x + uv.y as t in A*sin(2*pi*f*t + phi). We then have to give values to the amplitude A and the frequency f. For phi, as it's the shifting of the function, we put time in there, so that we do have an animation with respect to the time.



4 Water animation

In the same file as before, if we put Water to 1 in the #define, we can see that we do have another animation.
In fact, instead of using uv.x + uv.y as t in the sinusoid, we use r, which is the polar distance between the center and (uv.x, uv.y).

For the amplitude, as we want the sinusoid to be less and less big until we reach the boundaries, we put an exp(-r) in it. 
We do put also an attenuation factor, so that after a certain amount of time, the water becomes more and more still. The animation loops every 40 seconds.









