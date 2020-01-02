# Cabelin

CABELIN is an application developed in openFrameworks (version 0.10.1), a free, open-source C++ framework which is built on top of OpenGL. The app exploits the aesthetic possibilities of dynamically simulating hairs in a 2D space in real time. Through the usage of a GUI, the user can add hairs to the scene and modify their physical and aesthetical parameters such as the length of the hair, number of particles, damping and stiffness coeficient between the particles, friction with the medium, color, thickness, etc. A customizable perlin noise grid is also available in order to help designing how and where forces should be applied to the hairs, making it possible to generate more organic and fluid looking movements. The software also allows the user to export the frames of the scene in order to create an animation aftwards.

You can use the camera, pre-recorded video, or kinect V2 to interact with the hairs. Saving and opening a project is also possible.

# Instructions

The first step is to have openFrameworks installed. You can find the instructions on how to install it on their [website](https://openframeworks.cc/download/) (the app was developed on version 0.9.8). Once you have OF installed and made sure you can run the examples, copy and paste the 'Cabelin' folder inside _apps/myApps_, type _\$ make_, and then _\$ make RunRelease_.

You need to include the following addons (included in the repo):
ofxOpenCV
ofxGui
ofxKinectV2

You should also delete the following folder otherwise you won't be able to import ".jpg" to the scene:
`addons/ofxKinectV2/libs/libturbojpeg/lib`

Before doing _\$ make_ and _\$ make RunRelease_ try _\$ make clean_
*openFrameworks version: of_v0.10.1_osx_release
*The blobs are detected using [ofxOpenCV](https://openframeworks.cc/documentation/ofxOpenCv/)
