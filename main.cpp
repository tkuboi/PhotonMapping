/*
  Toshihiro Kuboi
  CSC570 Project
  Photon Mapping
*/
#include <iostream>
#include "photonMapping.h"
#include "Image.h"


int window_width = 640;
int window_height = 480;

//PhotonPoint global[][][] = new PhotonPoint[2][5][10000];
//PhotonPoint caustics[][][] = new PhotonPoint[2][5][10000];


int main() {
   // set up the scene
   Vec3f eye(0.0f,0.0f,0.0f);
   Vec3f light(300.0f, 400.0f, -100.0f);
   Vec3f center(0.0f, -150.0f, -400.0f);
   color_t sphereColor(1.0f,1.0f,1.0f,0.1f);
   Sphere sphere(center, 100.0f, sphereColor, 0.0);
   Vec3f center2(-200.0f, 100.0f, -500.0f);
   color_t sphereColor2(1.0f,0.0f,0.0f,1.0f);
   Sphere sphere2(center2, 100.0f, sphereColor2, 0.5);
   Vec3f center3(200.0f, 0.0f, -600.0f);
   color_t sphereColor3(0.0f,0.0f,1.0f,1.0f);
   Sphere sphere3(center3, 100.0f, sphereColor3, 0.5);
   Vec3f planeNormal(0.0, 1.0, 0.0);
   color_t planeColor(1.0,1.0,1.0,1.0);
   Plane plane(planeNormal, 300.0, planeColor);
   Scene myScene(light);
   myScene.addSphere(sphere);
   myScene.addSphere(sphere2);
   myScene.addSphere(sphere3);
   myScene.addPlane(plane);
   Vec3f planeNormal2(0.0, 0.0, 1.0);
   color_t planeColor2 (1.0,1.0,1.0,1.0);
   Plane plane2(planeNormal2, 700.0, planeColor2);
   myScene.addPlane(plane2);
   Vec3f planeNormal3(0.0, -1.0, 0.0);
   color_t planeColor3(1.0,1.0,1.0,1.0);
   Plane plane3(planeNormal3, 500.0, planeColor3);
   myScene.addPlane(plane3);
   Vec3f planeNormal4(1.0, 0.0, 0.0);
   color_t planeColor4(1.0,0.0,0.0,1.0);
   Plane plane4(planeNormal4, 400.0, planeColor4);
   myScene.addPlane(plane4);
   Vec3f planeNormal5(-1.0, 0.0, 0.0);
   color_t planeColor5(1.0,0.0,1.0,1.0);
   Plane plane5(planeNormal5, 400.0, planeColor5);
   myScene.addPlane(plane5);

   // make a 640x480 image (allocates buffer on the heap)
   Image img(window_width, window_height);

   float t_hit, min_t, dl, intensity, att, v_dot, specular, denom;
   Vec3f p_hit, normal, l, ln, dir, ref;
   int idx, hit, obj;

   //build photon maps
   myScene.buildGlobalMap(3);
   myScene.buildCausticsMap(0);

   cout << "before ray tracing" << endl;
   //ray trace
   for (int i = 0; i < window_width; i++) {
      for (int j = 0; j < window_height; j++) {
         Vec3f dir((float)i - (float)window_width/2.0f, (float)j - (float)window_height/2.0f, -200.0f);
         dir = dir - eye;
         dir.normalize();
         color_t clr = myScene.getPixelColor(eye, dir, 0);
         img.pixel(i, j, clr);
      } // inner for loop
   } // Outer for loop
   cout << "after ray tracing" << endl;
   // write the targa file to disk
   img.WriteTga((char *)"ray.tga", true); 
   // true to scale to max color, false to clamp to 1.0

   return 0;
}

