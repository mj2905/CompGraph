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

