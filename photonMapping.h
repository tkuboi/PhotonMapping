/*
  Toshihiro Kuboi
  CSC570 Project
  Photon Mapping
*/

#ifndef __photonMapping_H__
#define __photonMapping_H__

#define HIT      1		// Ray hit primitive
#define MISS	 0		// Ray missed primitive
#define INPRIM	-1		// Ray started inside primitive
#define SPHERE 0
#define PLANE 1
#define PI 3.14159f
#define NUM_GPHOTONS 20000

#define NUM_CPHOTONS 10000

#define MAX_RECURC 3
#define PHOTONMAPONLY false
#define RAYTRACINGONLY false
#define Ia 1.0 //ambient
#define Ka 0.2 //ambient
#define Id 1.0 //diffuse
#define Kd 0.5 //diffuse
#define Is 1.0 //specular
#define Ks 0.4 //specular
#define Ps 7.0 //specular
#define Kp 0.2 //Contribution from PhotonMapping
#define Kc 0.2


#include <stdio.h>      /* printf, scanf, NULL */
#include <stdlib.h>     /* malloc, free, rand */
#include <string>
#include <iostream> 

#include <cmath>

using namespace std;

class color_t {
public:
   float r;
   float g;
   float b;
   float f; // "filter" or "alpha"
   color_t() {r = 0.0f; g = 0.0f; b = 0.0f; f = 1.0f;}
   color_t(float fr, float fg, float fb) {r = fr; g = fg; b = fb; f = 1.0f;}
   color_t(float fr, float fg, float fb, float fl) {r = fr; g = fg; b = fb; f = fl;}
   void toString() {cout << "(" << r << "," << g << "," << b << "," << f << ")" << endl;}
   color_t operator + ( const color_t& v2 ) { color_t res( r + v2.r, g + v2.g, b + v2.b, min(f,v2.f)); return res;}
   color_t operator + ( float a ) { color_t res( r + a, g + a, b + a, f); return res;}
   color_t operator - ( const color_t& v2 ) { color_t res(r - v2.r, g - v2.g, b - v2.b, f); return res;}
   color_t operator * ( float a ) { color_t res( r * a, g * a, b * a, f); return res;}
};

class Vec3f {
public:
   float x;
   float y;
   float z;
   Vec3f() {x = 0; y = 0; z = 0;};
   Vec3f(float ax, float ay, float az) {x = ax; y = ay; z = az;};
   void toString() {cout << "(" << x << "," << y << "," << z << ")" << endl;};
   float getLength() { return (float)sqrt( x * x + y * y + z * z ); }
   void normalize() { float l = 1.0f / getLength(); x *= l, y *= l, z *= l;}
   float dot(Vec3f v2) { return x * v2.x + y * v2.y + z * v2.z; }
   Vec3f cross(Vec3f v2) { Vec3f res(y*v2.z - z*v2.y, z*v2.x - x*v2.z, x*v2.y - y*v2.x); return res; }
   Vec3f operator + ( const Vec3f& v2 ) { Vec3f res( x + v2.x, y + v2.y, z + v2.z ); return res;}
   Vec3f operator - ( const Vec3f& v2 ) { Vec3f res(x - v2.x, y - v2.y, z - v2.z ); return res;}
   Vec3f operator * ( float f ) { Vec3f res( x * f, y * f, z * f ); return res;}
   Vec3f operator * ( const Vec3f& v2 ) { Vec3f res( x * v2.x, y * v2.y, z * v2.z ); return res;}
};

class Outputlist {
public:
   int hit;
   float t_hit;
   Vec3f p_hit;
   Vec3f normal;
   int obj;
   int idx;
   Outputlist(){hit = MISS;};
   Outputlist(Vec3f p, Vec3f n, int o, int i){hit = MISS; t_hit = 0; p_hit = p; normal = n; obj = o; idx = i;};
   Outputlist(int h, Vec3f p, Vec3f n, int o, int i){hit = h; t_hit = 0; p_hit = p; normal = n; obj = o; idx = i;};
   void toString(){cout << " hit= " << hit << " t_hit= " << t_hit << " obj= " << obj << " idx= " << idx << endl;};
};

class Sphere {
public:
   Sphere() {};
   Sphere(Vec3f center, float r, color_t c, float ref) {_center = center; _radius = r; color = c; reflective = ref;};
   color_t getColor(){return color;};
   Vec3f getCenter() {return _center;};
   float getRadius() {return _radius;};
   float getReflective(){return reflective;};
   Outputlist intersect(Vec3f s, Vec3f d);
   Vec3f getNormal( Vec3f a_Intersection );
private:
   Vec3f _center;
   float _radius;
   color_t color;
   float reflective;
};

class Plane {
public:
   Plane() {};
   Plane(Vec3f n, float d, color_t c) {_n = n; _d = d; color = c; reflective = 0.0f;};
   color_t getColor(){return color;};
   float getReflective(){return reflective;};
   Outputlist intersect(Vec3f s, Vec3f d);
   Vec3f getNormal() {return _n;};
   Vec3f getNormal(float denom) {return (denom > 0 ? _n*(-1) : _n);};
   Vec3f getNormal(Vec3f d) {float denom = _n.dot(d);return (denom > 0 ? _n*(-1) : _n);};
private:
   Vec3f _n;
   float _d;
   color_t color;
   float reflective;
};

class Photon {
public:
   Photon() {};
   Photon(color_t pow) {power = pow;};
   color_t getPower() {return power;};
   Vec3f getPowerVec() {Vec3f pow(power.r, power.g, power.b); return pow;};
private:
   color_t power;
   Vec3f direction;
};

class PhotonPoint {
public:
   PhotonPoint() {};
   ~PhotonPoint() {};
   PhotonPoint(Photon ph, Vec3f p) {photon = ph; point = p;};
   PhotonPoint(Photon ph, Vec3f p, Vec3f d) {photon = ph; point = p; dir = d;};
   Vec3f getPoint() {return point;};
   Vec3f getDir() {return dir;};
   Photon getPhoton() {return photon;};
private:
   Photon photon;
   Vec3f point;
   Vec3f dir;
   short flag;
};

class PhotonPointList {
public:
   PhotonPointList() {};
   PhotonPointList(int n) {maxSize = n; init(); length=0;};
   ~PhotonPointList() {};
   void sort(char axis) {msort(axis, plist, 0, length - 1);};
   int getLength(){return length;};
   void add(PhotonPoint p) {plist[length] = p; length++;};
   PhotonPoint pop() {PhotonPoint p = plist[length - 1]; --length; return p;};
   PhotonPoint at(int idx) {return plist[idx];};
   void emitPhotons(int numBounces){}; // emit photons
   void shadowPhoton(Vec3f origin, Vec3f dir){};
   PhotonPoint* tracePhoton(Sphere s, Plane p){}; // trace photon
private:
   int maxSize;
   int length;
   PhotonPoint* plist;
   void init() {cout << "in init " << endl;plist = (PhotonPoint*)malloc(sizeof(PhotonPoint)*maxSize);};
   void msort(char axis, PhotonPoint* list, int begin, int end);
   void merge(char axis, PhotonPoint* list, PhotonPoint* left, PhotonPoint* right, int begin, int end, int a_q, int a_r);
};

class Node {
public:
   Node() {isleaf = false; left = NULL; right = NULL; /*cout << "Node constructor: " << getIsleaf() << endl;*/};
   void setLine(Vec3f al) {line = al;/*cout << "in setLine" << endl;*/};
   void setPoint(PhotonPoint ap) {point = ap;/*cout << "in setPoint" << endl;*/};
   void setLeft(Node* an) {left = an;/*cout << "in setLeft " << an->getIsleaf() << endl;*/};
   void setRight(Node* an) {right = an;/*cout << "in setRight " << an->getIsleaf() << endl;*/};
   void setIsleaf(bool b) {isleaf = b;};
   Vec3f getLine() {return line;};
   PhotonPoint getPoint() {return point;};
   Node* getLeft() {/*cout << "getLeft " << left << " " << left->getIsleaf() << endl;*/return left;};
   Node* getRight() {/*cout << "getRight " << right << " " << right->getIsleaf() << endl;*/return right;};
   bool getIsleaf() {return isleaf;};
   bool hasLeft() {return (left == NULL ? false: true);};
   bool hasRight() {return (right == NULL ? false: true);};
private:
   bool isleaf;
   Vec3f line;
   PhotonPoint point;
   Node* left;
   Node* right;
};

class Kdtree {
public:
   Kdtree() {};
   void buildKDtree(PhotonPointList &points);
   Node* createPhotonMap(){}; // create photon map
   PhotonPointList* rangeSearch(float x1, float x2, float y1, float y2, float z1, float z2);
   Vec3f getNearestNeighbor(Vec3f p);
   Node* getNode() {return node;};
private:
   Node* node;
   Node* add(PhotonPointList &points, int depth);
   void split(PhotonPointList &points, int median, PhotonPointList &left, PhotonPointList &right);
   void search(PhotonPointList* points, Node* v, float x1, float x2, float y1, float y2, float z1, float z2);
   int testRange(int branch, Vec3f line, float& x1, float& x2, float& y1, float& y2, float& z1, float& z2);
   void reportSubtree(Node* v, PhotonPointList &points);
   Vec3f searchNeighbor(Node* v, Vec3f p);
};

class Scene {
public:
   Scene(){numSpheres = 0; numPlanes = 0;init();};
   Scene(Vec3f l){light = l; numSpheres = 0; numPlanes = 0;init();};
   //Scene(Vec3f l,PhotonPoint* g, PhotonPoint* c){light = l; numSpheres = 0; numPlanes = 0;global = g; caustics = c; init();};
   int addSphere(Sphere sp){spheres[numSpheres++] = sp;return numSpheres;};
   int addPlane(Plane pl){planes[numPlanes++] = pl;return numPlanes;};
   Outputlist rayTrace(Vec3f origin, Vec3f dir);
   //int rayTrace(Vec3f origin, Vec3f dir, Vec3f& p_hit, Vec3f& normal, int& obj, int& idx);
   Vec3f reflect(Vec3f dir, Vec3f normal);
   Vec3f diffuse(Vec3f normal);
   color_t getPixelColor(Vec3f eye, Vec3f dir, int count);
   int getNumSpheres() {return numSpheres;};
   int getNumPlanes() {return numPlanes;};
   void buildGlobalMap(int numBounces); // emit photons
   void buildCausticsMap(int numBounces); // emit photons
   void shadowPhoton(Vec3f origin, Vec3f dir);
   color_t lookupGmap(Vec3f point, float radius, Vec3f normal, int obj, int idx);
   color_t lookupCmap(Vec3f point, float radius, Vec3f normal, int obj, int idx);
   color_t getFilteredColor(color_t rgbIn, int obj, int idx, Vec3f dir, Vec3f normal, Vec3f p_hit);
   color_t filterColor(color_t rgbIn, color_t filter); 
   void storePhoton(int type, int obj, int idx, PhotonPoint ppoint);
   void init();
   Vec3f refract(Vec3f dir, Vec3f normal, bool from); //if opacity is less than 100%
   color_t scaleColor(color_t clr);
private:
   Vec3f light;
   Sphere spheres[10];
   Plane planes[10];
   int numSpheres;
   int numPlanes;
   PhotonPoint global[2][5][5000];
   PhotonPoint caustics[2][5][5000];
   int numGPhotons[2][5];
   int numCPhotons[2][5];
};


#endif
