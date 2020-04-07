# Frogger
Using OpenGL and GLFW to recreate the road-crossing, water-crossing amphibian

The aim of the program is to emulate a decision making system for neural networks. A secondary goal is to keep the code as light as 
possible, how small can the code be while still being easy to extend? The development of this game presents a first step in this 
direction. This makes using C++ and Opengl quite ideal. 

As a 2-D game, it is quite simple- it uses arrow keys to move the frog in the environment. The graphics used is also just as simple. 
Basically a container is used to display images. Movement of the container simulates movement of various sprites in the game. The vertex 
shader simply takes in the coordinates of the container and the texture mapping (passes it to fragment shader). The fragment shader 
maps the required image to the container. 

The core idea of the game is to control a frog so that it can safely cross the road and river to reach the safe-zones (water on the 
uppermost level). With increasing levels, the difficulty in crossing increases. This can be simulated as faster vehicles, faster 
boats (turtles, logs and crocs, lets simply them boats), boosts, sinking-resurfacing boats and so on. The interaction between the frog 
and the remaining complex environment has to be calculated at each frame as well. This necessitates easy to extend code- developed 
using classes. I use a separate class for the frog, one for the vehicles, one for the boats and so on. 

The Visual Studio environment is used to set up the game. Learnopengl.com provides a good description on how this can be done. Once
everything is linked, simply run the source.cpp file and have fun :)  

Finally, there are still quite a few things that would be nice to implement to improve the game-
1.  Add multiple cars/row, similar to the logs implementation
2.  Add animations
	- for when the frog falls off
	- when becoming roadkill (just image)
	- top row criteria
	- when the frog moves (add a different image for the auto-move duration?)
3.  Add turtles, crocodiles
4.  Add female frog and snake on the logs
5.  Implement stable level-up changes 
     - only one level present now and it loops over
6.  Implement number of lives (increase and decrease)
7.  Display score, #lives
8.  Use a more consistent version of the frog images when it is oriented in different directions
9.  Perhaps, use a single drawing class and make everyone else friends. Then a single draw call can be implemented for everybody instead
    a separate draw in each class.
10. Game is based on internal clock timing. Account for what happens in-case of system overload and the game freezes for a moment. 
    Bascially, sync-desync clock.
11. Use a divider-less road texture for the highest road level
12. Resolution-based check on textures for better viewing experience, size check on the .png files

![Frogger-snapshot](https://github.com/AvidDrawer/Frogger/blob/master/Frogger-images/Frogger.png)

[Screen capture](http://google.com) of the game in operation. Given the first implementation more or less captures the essence of the
game, the updates to transform it into a true game will be left on the backburner- I'll eventually get to it, just not yet.

