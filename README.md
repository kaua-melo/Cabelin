# Cabelin + Blob tracker + Perlin Noise

It creates a perlin noise field which should follow each blob on the scene and this perlin noise should interact with the possible hairs in the scene.

You need to include the follwing addons (included in the repo):
ofxOpenCV
ofxGui
ofxKinectV2

You should also delete the following folder otherwise you won't be able to import .jpg to the scene:
`addons/ofxKinectV2/libs/libturbojpeg/lib`

Before doing _\$ make_ and _\$ make RunRelease_ try _\$ make clean_
*openFrameworks version: of_v0.10.1_osx_release
*The blobs are detected using [ofxOpenCV](https://openframeworks.cc/documentation/ofxOpenCv/)
