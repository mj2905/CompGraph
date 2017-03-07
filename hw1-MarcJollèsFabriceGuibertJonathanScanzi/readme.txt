1.1 Triangle Spirals

For this exercise, we had to draw multiple triangles on the screen, with different coordinates, sizes and rotations, to form a spiral (simple, or Fermat model). Thus, what we do is to draw the same triangle at different places, by using the Draw method multiple times, with different matrices for the model.

Thus, we have to use a loop, and for each iteration, we have different constants (defined according to the Spiral or the Fermat model). We use these constants to create the model matrix, to draw the triangle.

First, we choose to scale the triangle, so that it's scaling according to the window center (and there is no hidden translation or rotation). Then, we translate the scaled triangle to a position r in the x axis, so that we only need to rotate the result by an angle theta.

1.2 Checkerboard

For this exercise, we only had to modify the fragment shader. We have uv and colormap as inputs. In the "color = texture(colormap, value).rgb;" instruction, if the value is 0 we obtain red, if it's 0.5 we have yellow and if it's 1 we have green. Thus, we need to have a value which is between 0 and 1.
Then, we have to create some squares, given the uv.x and uv.y coordinates. 

Thus, if we do sin(uv.x * 2 * M_PI * nb_carreaux), we have a function which does nb_carreaux periods of the sinusoid in the uv.x interval [0, 1]. The same for sin(uv.y * 2 * M_PI * nb_carreaux) which does nb_carreaux periods of the sinusoid in the uv.y interval [0, 1]. 

If we multiply these two functions, we will get a 2D function which does the checkerboard, but with values between -1 and 1. We have to normalize it to the interval [0, 1], by dividing the result of the function by 2, and adding 0.5 to the result. We then have our value variable.

1.3 Solar System

For this exercise, we use 3 quads : one for the sun, one for the earth and one for the moon.

We define some angles (sun_angle, earth_angle, moon_angle), depending on the time, in the Display() function.
We define too some x and y coordinates, for the earth, for the ellipsoidal trajectory around the sun.
Then, we create some matrices for the sun, earth and moon, so that we can create a matrix which depends on the other matrices (if the sun moves, the earth and the moon move too for example).

When we have created the matrices, we draw the quads using the matrices (multiplied by some other matrices as the scale for example, as it would complicate the previous matrices if we had to deal with them previously).

1.4 Arkanoid

(The skeleton for this part was the same as for the hw2p3_planets. The files added are all files in the arkanoid directory, so ball.h, element.h, paddle.h, position.h, position_ball.h, position_paddle.h. The main.cpp was of course also modified. quad.h has been modified, circle_fshader and circle_vshader have been added for the ball).

For this exercise, we created a lot of different classes, for the different existing objects.
We have the class element (which is the base of balls and paddles) which has a quad as visual representation, a position and some direction vectors for moving.

The paddle can move by "step" (a constant defined in the paddle class) on the right or on the left, with some boundaries in -1 and 1.

The ball can be Updated, so that it will move in the direction (vecx, vecy) and bounce against the walls. It can also tell if it's under a fixed limit (for example under the paddle), and it can be reset to the initial position (if for example the ball is under the paddle).

Position_Ball and Position_Paddle, subclasses of Position, allow us to define the behaviors of moving (given constraints), bounce against walls, and bounce against the paddle.

In the main, we added the Update function, called after the Display, so that at each iteration of the game loop, we can update the state of the game (to move the ball, make it bounce against the paddle and check if it's the end of the game, to reset/restart it right after).