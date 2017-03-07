1.1 Triangle Spirals

For this exercise, we had to draw multiple triangles on the screen, with different coordinates, sizes and rotations, to form a spiral (simple, or Fermat model). Thus, what we did was to draw the same triangle at different places, by using the Draw method multiple times, with different matrices for the model.

Since the Draw is called each frame, the use of a loop was deemed necessary to create a "static" spiral at each frame. Every spiral had different parameters, according to the two spiral definitions we had. The values of said parameters were determined empirically, by inspection of the reference screenshot and incremental correction of the code and the resulting images.

First, we choose to scale the triangle, so that it's scaling according to the window center (and there is no hidden translation or rotation). Then, we translate the scaled triangle to a position r in the x axis, so that we only need to rotate the result by an angle theta.

1.2 Checkerboard

For this exercise, we only had to modify the fragment shader. We had uv and colormap as inputs. In the "color = texture(colormap, value).rgb;" instruction, the value went from 0 to 1, corresponding to a scale going from red to green, with 0.5 being yellow.
Given the x and y coordinates of uv, we needed to find an implicit function that would periodically create squares of values 0, 1 and transition (shortly) through 0.5, to obtain a checkerboard.

The combination of a periodic function on the x axis with another periodic function of the y axis was intuitively supposed to give a periodic behaviour that would fit for our checkerboard. We went with a sin function: sin(coordinate * 2PI) is periodic.
The implicit function is simply f(x,y, n) = sin(x * 2PI*n) * sin(y * 2PI*n), where n defines the number of squares of a given color we have per row.
Now, we have just a small problem. Sin functions oscillate between -1 and 1, and our color value oscillates between 0 and 1.
We just have to switch our function f(x,y,n) by 1, but then the value oscillates between 0 and 2, so we half it!

The last function we get is then g(x, y, n) = (f(x,y,n)+1)/2 = (sin(x * 2PI * n) * sin(y * 2PI * n))/2 + 0.5

Note that doing a sum of sins (sin(2*PI*x*n+coeff) + sin(Y*2PI*n + coeff)) would also give a board, but with an angle of 45° on the left.


1.3 Solar System

For this exercise, we use 3 quads : one for the sun, one for the earth and one for the moon.

We define some angles (sun_angle, earth_angle, moon_angle), depending on the time, in the Display() function.
We define some x and y coordinates too, for the earth, for the ellipsoidal trajectory around the sun.
Then, we create some matrices for the sun, earth and moon, so that we can create a matrix which depends on the other matrices (if the sun moves, the earth and the moon move too for example). Thus, one transformation ripples on every other, representing the entwined state of the three objects.

When we have created the matrices, we draw the quads using the matrices (multiplied by some other matrices as the scale for example, as it would complicate the previous matrices if we had to deal with them previously).

Note that this time, as opposed to 1.1, there is no need for a while loop. This time, the objects and their positions are updated as a function of the current time (with periodic sin functions once again), and will naturally move on the screen.

1.4 Arkanoid

(The skeleton for this part was the same as for the hw2p3_planets. The files added are all files in the quad directory, so ball.h, element.h, paddle.h, position.h, position_ball.h, position_paddle.h. The main.cpp was of course also modified).

For this exercise, we created a lot of different classes, for the different existing objects.
We have an element (which is the base of balls and paddles) which has a quad as representation, a position and some direction vectors for moving.

The paddle can move by "step" (a constant defined in the paddle class) on the right or on the left, with some boundaries in -1 and 1.

The ball can be Updated, so that it will move in the direction (vecx, vecy) and bounce against the walls. It can also tell if it's under a fixed limit (for example under the paddle), and it can be reset to the initial position (if for example the ball is under the paddle).

Position_Ball and Position_Paddle, subclasses of Position, allow us to define the behaviors of moving (given constraints), bounce against walls, and bounce against the paddle.

In the main, we added the Update function, called after the Display, so that at each iteration of the game loop, we can update the state of the game (to move the ball, make it bounce against the paddle and check if it's the end of the game, to restart it right after).
