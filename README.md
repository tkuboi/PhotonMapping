PhotonMapping
=============

CSC570 Winter 2013 Term Project
 by Toshihiro Kuboi

Introduction

For CSC570 Final Project, I created a relatively simple image by Photon Mapping.


As you can see, my scene has 3 spheres within a room bounded by 3 walls and a ceiling. The sphere in front of the camera is a crystal, thus transparent, and it is projecting caustics on the floor. The two spheres in the back are reflective. Thus you can see the scene is reflected on the surfaces.
Implementation

The image is built from two photon maps: one for global illumination and the other one for caustics. The global map contains more than 20000 photons. The photon maps are implemented as standard C/C++ arrays. Photons are allowed to bounce maximum of 3 times. The caustics map contains 5000 photons. The color of each pixel is determined from three sources: The color of object hit by a ray and colors sampled from the global and caustics maps. The color sampling from the maps are done by sampling photons residing within the radius of 100 from the location of the interset. The Shading is done by Phong shading.

The size of the image is 640 X 480, and it takes about 1 min and 17 seconds to render the image. The image is outputed as a TGA file (ray.tga).

You can download the source for the program here (only tested on Linux).

To run the program, type "make", and then type "./tga". The program will be compiled and run. It is going to take a few minutes to finish creating an image.
