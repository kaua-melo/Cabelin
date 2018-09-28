# Cabelin

CABELIN is an application developed in openFrameworks (version 0.9.8), a free, open-source C++ framework which is built on top of OpenGL. The app exploits the aesthetic possibilities of dynamically simulating hairs in a 2D space in real time. Through the usage of a GUI, the user can add hairs to the scene and modify their physical and aesthetical parameters such as the length of the hair, number of particles, damping and stiffness coeficient between the particles, friction with the medium, color, thickness, etc. A customizable perlin noise grid is also available in order to help designing how and where forces should be applied to the hairs, making it possible to generate more organic and fluid looking movements. The software also allows the user to export the frames of the scene in order to create an animation aftwards.

# Instructions

The first step is to have openFrameworks installed. You can find the instructions on how to install it on their [website](https://openframeworks.cc/download/) (the app was developed on version 0.9.8). Once you have OF installed and made sure you can run the examples, copy and paste the 'Cabelin' folder inside *apps/myApps*, type *$ make*, and then *$ make RunRelease*.


