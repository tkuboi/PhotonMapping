/*
  Toshihiro Kuboi
  CSC570 Project
  Photon Mapping
*/

#include "photonMapping.h"


using namespace std;

Outputlist Sphere::intersect(  Vec3f s, Vec3f d )
{
	float t1, t2, discrim;
        Outputlist output;
	discrim = (s - _center).dot(d)*(s - _center).dot(d) - ((s - _center).dot(s - _center) - (_radius)*(_radius));
	if (discrim == 0) {
		output.hit = HIT;
                output.t_hit = -1 * (s - _center).dot(d);
        }
	else {
		if(discrim > 0) {
		   t1 = -1 * (s - _center).dot(d) + sqrt(discrim);
	           t2 = -1 * (s - _center).dot(d) - sqrt(discrim);
		   if (t1 > 0 && t2 > 0) {
                           output.hit = HIT;
			   output.t_hit = t1 < t2 ? t1:t2;
                   }
		   else {
			   if (t1 > 0 && t2 <= 0) {
                              output.hit = HIT;
			      output.t_hit = t1;
                           }
                           else
                              output.hit = INPRIM;
                   }
		}
	}
        return output;
}

//get surface noraml
Vec3f Sphere::getNormal( Vec3f a_Intersection )
{
	Vec3f normal = (a_Intersection - _center);
        normal.normalize();
	return normal;
}

Outputlist Plane::intersect( Vec3f s, Vec3f d)
{
   Outputlist output;
   float denom = _n.dot(d);
   if (denom != 0) {
      output.t_hit = -1*(_n.dot(s)+_d)/denom;
      output.hit = (output.t_hit >= 0? HIT:INPRIM);
   }
   else
      output.t_hit = -1;

   return output;
}

/*void PhotonPointList::shadowPhoton(Vec3f origin, Vec3f dir){
   color_t shadow{-0.25, -0.25, -0.25};
   Vec3f start = origin + dir * 0.00001; // start from just off the hit point.
   vec3f p_hit;
   int hit = rayTrace(start, dir, p_hit);
   if (hit == HIT) {
      Photon photon(shadow);
      PhotonPoint ppoint(photon, p_hit, dir);
      add(ppoint);
   }
}*/

void PhotonPointList::merge(char axis, PhotonPoint* list, PhotonPoint* left, PhotonPoint* right, int begin, int end, int a_q, int a_r) {
   int q = 0;
   int r = 0;
   for (int i = begin; i <= end; i++) {
      //printf("left=%f, right=%f \n",left[q],right[r]);
      if (q < a_q && r < a_r) {
         switch (axis) {
            case 'x':
               if (left[q].getPoint().x < right[r].getPoint().x)
                  list[i] = left[q++];
               else
                  list[i] = right[r++];
               break;
            case 'y':
               if (left[q].getPoint().y < right[r].getPoint().y)
                  list[i] = left[q++];
               else
                  list[i] = right[r++];
               break;
            case 'z':
               if (left[q].getPoint().z < right[r].getPoint().z)
                  list[i] = left[q++];
               else
                  list[i] = right[r++];
               break;
         } //switch
      } //if    
      else
         if ( q < a_q )
            list[i] = left[q++];
         else
            if ( r < a_r )
               list[i] = right[r++];
   }
}


void PhotonPointList::msort(char axis, PhotonPoint* list, int begin, int end) {
   if ((end - begin) <= 0)
      return;
   //left
   int q = (end -begin + 1) / 2;
   PhotonPoint left[q];
   for (int i = 0; i < q; i++)
      left[i] = list[i];
   msort(axis, left, 0, q - 1);
   //right
   int r = (end -begin + 1) - q;
   PhotonPoint right[r];
   for (int i = 0; i < r; i++)
      right[i] = list[q+i];
   msort(axis, right, 0, r - 1);
   //merge!
   merge(axis, list, left, right, begin, end, q, r);
}


void Kdtree::split(PhotonPointList &points, int median, PhotonPointList &left, PhotonPointList &right) {
   //cout << "in split" << endl;
   for(int i = 0; i < median; i++)
      left.add(points.at(i));
   for(int i = median; i < points.getLength(); i++)
      right.add(points.at(i));
   //cout << "end of split" << endl;
}

void Kdtree::buildKDtree(PhotonPointList &points) {
   if (points.getLength() > 0)
      node = add(points, 0);
   //cout << "end of buildKDtree" << endl;
   //cout << node->getLeft()->getIsleaf() << endl;
}

Node* Kdtree::add(PhotonPointList &points, int depth) {
   Node* node = new Node();
   int n = points.getLength();
   int median = n / 2;
   //cout << "before if. n= " << n << " median=" << median << endl;
   //if (n < 1)
   //   return node;
   if (n == 1) {
      node->setPoint(points.at(n-1));
      node->setIsleaf(true);
      //cout << "in if, depth= " << depth << endl;
      //points.at(n-1).getPoint().toString();
      return node;
   }
   else
      if(depth % 3 == 0) 
         points.sort('x');
      else
         if(depth % 3 == 1)
            points.sort('y');
         else
            points.sort('z');
   //cout << "after if" << endl;
   PhotonPointList left(median + 1);
   PhotonPointList right(median + 1);
   split(points, median, left, right);
   Node* lnode = add(left, depth+1);
   Node* rnode = add(right, depth+1);
   //lnode = add(left, depth+1);
   //rnode = add(right, depth+1);
   node->setLeft(lnode);
   //cout << "just after setting Left: " << node.getLeft().getIsleaf() << endl;
   node->setRight(rnode);
   //cout << "just after setting Right: " << node.getRight().getIsleaf() << endl;
   Vec3f al(0.0f,0.0f,0.0f);  //initialize line
   if (depth % 3 == 0)
      al.x = points.at(median).getPoint().x;
   else
      if (depth % 3 == 1)
         al.y = points.at(median).getPoint().y;
      else
         al.z = points.at(median).getPoint().z;
   //cout << "before setLine" << endl;
   node->setLine(al);
   node->setIsleaf(false);
   //cout << "before return" << endl;
   //cout << "Left: " << node.getLeft().getIsleaf() << endl;
   return node;
}

PhotonPointList* Kdtree::rangeSearch(float x1, float x2, float y1, float y2, float z1, float z2) {
   cout << "begin rangeSearch" << endl;
   PhotonPointList* points = new PhotonPointList(30);
   cout << "init success!" << endl;
   //PhotonPointList* points = search(node, x1,x2,y1,y2,z1,z2);
   search(points, node, x1,x2,y1,y2,z1,z2);
   cout << "end rangeSearch" << endl;
   return points;
}

void Kdtree::search(PhotonPointList* points, Node* v, float x1, float x2, float y1, float y2, float z1, float z2) {
   //PhotonPointList* points = new PhotonPointList(10);
   cout << "in search" << endl;
   float lx1 = x1; float lx2 = x2; float ly1 = y1; float ly2 = y2; float lz1 = z1; float lz2 = z2;
   float rx1 = x1; float rx2 = x2; float ry1 = y1; float ry2 = y2; float rz1 = z1; float rz2 = z2;
   int contained = 0;
   if (v->getIsleaf()) {
      if (x1 <= v->getPoint().getPoint().x && v->getPoint().getPoint().x <= x2 &&
          y1 <= v->getPoint().getPoint().y && v->getPoint().getPoint().y <= y2 &&
          z1 <= v->getPoint().getPoint().z && v->getPoint().getPoint().z <= z2)
         cout << "We got leaf!" << endl;
         points->add(v->getPoint());
         v->getPoint().getPoint().toString();
   }
   else {
      //PhotonPointList* leftPoints;
      contained = testRange(0,v->getLine(),lx1,lx2,ly1,ly2,lz1,lz2);
      if (contained == 1) { //fully contained in left
         //reportSubtree(v->getLeft(),points);
         //leftPoints = search(points, v->getLeft(),lx1,lx2,ly1,ly2,lz1,lz2);
         search(points, v->getLeft(),lx1,lx2,ly1,ly2,lz1,lz2);
      }
      else {
         if (contained == 2) {
            //leftPoints = search(points, v->getLeft(),lx1,lx2,ly1,ly2,lz1,lz2);
            search(points, v->getLeft(),lx1,lx2,ly1,ly2,lz1,lz2);
         }
      }
      /*if (contained != 0) {
         cout << "length of left= " << leftPoints->getLength() << endl;
         for (int i = 0; i < leftPoints->getLength(); i++)
            points->add(leftPoints->at(i));
      }*/
      //PhotonPointList* rightPoints;
      contained = testRange(1,v->getLine(),rx1,rx2,ry1,ry2,rz1,rz2);
      if (contained == 1) { //fully contained in right
         //reportSubtree(v->getRight(),points);
         //rightPoints = search(points, v->getRight(),rx1,rx2,ry1,ry2,rz1,rz2);
         search(points, v->getRight(),rx1,rx2,ry1,ry2,rz1,rz2);
      }
      else {
         if (contained == 2) {
            //rightPoints = search(points, v->getRight(),rx1,rx2,ry1,ry2,rz1,rz2);
            search(points, v->getRight(),rx1,rx2,ry1,ry2,rz1,rz2);
         }
      }
      /*if (contained != 0) {
         cout << "length of right= " << rightPoints->getLength() << endl;
         for (int i = 0; i < rightPoints->getLength(); i++)
            points->add(rightPoints->at(i));
      }*/
   }
   //return points;
}

int Kdtree::testRange(int branch, Vec3f line, float& x1, float& x2, float& y1, float& y2, float& z1, float& z2) {
   int contained = 0; //0: not contained, 1: fully contained, 2: intersects
   if (branch == 0) {//left
      //line.toString();
      //cout << "(" << x1 << "," << x2 << "," << y1 << "," << y2 << "," << z1 << "," << z2 << ")" << endl;
      if (line.x != 0.0f) {
         //cout << "left line.x != 0.0f" << endl;
         if (x1 <= line.x && x2 <= line.x)
            contained = 1;
         else
            if (x1 <= line.x && line.x <= x2) {
               x2 = line.x;
               contained = 2;
            }
      }
      else {
         //line.toString();
         if (line.y != 0.0f) {
            if (y1 <= line.y && y2 <= line.y)
               contained = 1;
            else
               if (y1 <= line.y && line.y <= y2) {
                  y2 = line.y;
                  contained = 2;
               }
         }
         else {
            //cout << "testing for z.." << endl;
            if (z1 <= line.z && z2 <= line.z)
               contained = 1;
            else {
               if (z1 <= line.z && line.z <= z2) {
                  z2 = line.z;
                  contained = 2;
               }
            }
         }
      }
   }
   else {//right
      //line.toString();
      //cout << "Right" << endl;
      if (line.x != 0.0f) {
         if (line.x <= x1 && line.x <= x2)
            contained = 1;
         else {
            if (x1 <= line.x && line.x <= x2) {
               x1 = line.x;
               contained = 2;
            }
         }
      }
      else {
         if (line.y != 0.0f) {
            if (line.y <= y1 && line.y <= y2)
               contained = 1;
            else {
               if (y1 <= line.y && line.y <= y2) {
                  y1 = line.y;
                  contained = 2;
               }
            }
         }
         else {
            if (line.z <= z1 && line.z <= z2)
               contained = 1;
            else {
               if (z1 <= line.z && line.z <= z2) {
                  z1 = line.z;
                  contained = 2;
               }
            }
         }
      }
   }
   //cout << "contained= " << contained << endl;
   return contained;
}

void Kdtree::reportSubtree(Node* v, PhotonPointList &points) {
   if (v->getIsleaf()) {
      cout << "in report if" << endl;
      points.add(v->getPoint());
      v->getPoint().getPoint().toString();
      cout << "end report if" << endl;
   }
   else {
      cout << "begin report else" << endl;
      if (v->hasLeft())
         reportSubtree(v->getLeft(), points);
      if (v->hasRight())
         reportSubtree(v->getRight(), points);
      cout << "end report else" << endl;
   }
}

Vec3f Kdtree::getNearestNeighbor(Vec3f p) {
   Vec3f point = searchNeighbor(node, p);
   return point;
}

Vec3f Kdtree::searchNeighbor(Node* v, Vec3f p) {
   Vec3f point;

   return point;
}

Outputlist Scene::rayTrace(Vec3f origin, Vec3f dir) {
   int res = MISS;
   float min_t = 999999.9;
   float t_hit;
   Outputlist output, temp; 
   for (int i = 0; i < numSpheres; i++) {
      temp = spheres[i].intersect(origin,dir);
      if (temp.hit == HIT && temp.t_hit < min_t) {
         output.obj = SPHERE;
         output.idx = i;
         min_t = temp.t_hit;
         output.p_hit = origin + dir * min_t;
         output.normal = spheres[i].getNormal(output.p_hit);
         output.hit = HIT;
      }
   }
   
   for (int i = 0; i < numPlanes; i++) {
      temp = planes[i].intersect(origin,dir);
      if (temp.hit == HIT && temp.t_hit < min_t) {
         output.obj = PLANE;
         output.idx = i;
         min_t = temp.t_hit;
         output.p_hit = origin + dir * min_t;
         output.normal = planes[i].getNormal(dir);
         output.hit = HIT;
      }
   }
   return output;
}

color_t Scene::getPixelColor(Vec3f eye, Vec3f dir, int count) {
   color_t color(0.0f, 0.0f, 0.0f);
   color_t pixColor(0.0f, 0.0f, 0.0f);
   color_t gmapColor(0.0f, 0.0f, 0.0f);
   color_t cmapColor(0.0f, 0.0f, 0.0f);
   color_t shadowColor(-0.25f, -0.25f, -0.25f);
   Vec3f point, ln;

   Outputlist output = rayTrace(eye, dir);
   if (output.hit == HIT) {
      point = output.p_hit;
      ln = light - point;
      ln.normalize();
      float v_dot = output.normal.dot(ln);
      Vec3f ref = output.normal*2*v_dot - ln;
      float specular = pow(ref.dot(dir*(-1)),Ps);
      float intensity = Ia*Ka+(Id*Kd*v_dot);

      if (output.obj == SPHERE)
         pixColor = spheres[output.idx].getColor();
      else
         pixColor = planes[output.idx].getColor();

      pixColor = pixColor * intensity;

      if(!RAYTRACINGONLY) {
         gmapColor = lookupGmap(point, 100.0f, output.normal, output.obj, output.idx);
         cmapColor = lookupCmap(point, 100.0f, output.normal, output.obj, output.idx);
      }

      Vec3f start = point + ln * 0.01; // start from just off the hit point.
      Outputlist shadow = rayTrace(start, ln);
      float distance = (shadow.p_hit - start).getLength() - (light - start).getLength();
      if (shadow.hit == HIT && distance < 0) { //in shadow
         pixColor = pixColor + shadowColor * 1.0;
      }

      pixColor = pixColor + gmapColor * Kp + cmapColor * Kc + Is * Ks * specular;

      if(PHOTONMAPONLY) {
         pixColor = gmapColor+ cmapColor;
      }
    
      color = pixColor;

      if (count >= MAX_RECURC){
         return scaleColor(color);
      }

      float reflective = (output.obj == SPHERE ? spheres[output.idx].getReflective() : planes[output.idx].getReflective());
      if (reflective > 0.0f) {
         Vec3f refdir = reflect(dir, output.normal);
         color_t reflectColor = getPixelColor(start, refdir, ++count);
         color = pixColor * (1.0f - reflective) + reflectColor * reflective + Is * Ks * specular;
      }
      if (pixColor.f < 1.0f) {// transparent
         Vec3f rerdir = refract(dir, output.normal, false);
         start = output.p_hit + rerdir*0.01;
         output = rayTrace(start, rerdir);
         if (output.hit == MISS){
            return scaleColor(color);
         }
         rerdir = refract(rerdir, output.normal, true);
         start = output.p_hit + rerdir*0.01;
         color_t refractColor = getPixelColor(start, rerdir, ++count);
         color = pixColor * pixColor.f + refractColor * (1.0f - pixColor.f) + Is * Ks * specular;
      }
   }
  
   return scaleColor(color);
}

void Scene::shadowPhoton(Vec3f origin, Vec3f dir){
   color_t shadow(-0.25, -0.25, -0.25);
   Vec3f start = origin + dir * 0.00001; // start from just off the hit point.
   Vec3f p_hit, normal;
   int obj, idx;
   Outputlist output = rayTrace(start, dir);
   if (output.hit == HIT) {
      Photon photon(shadow);
      PhotonPoint ppoint(photon, output.p_hit, dir);
      //list.add(ppoint);
      storePhoton(0, output.obj, output.idx, ppoint); // store photon
   }
}

Vec3f Scene::reflect(Vec3f dir, Vec3f normal){
   float dot = dir.dot(normal);
   Vec3f res = dir - normal * dot * 2;
   res.normalize();
   return res;
}

Vec3f Scene::diffuse(Vec3f normal){
   float dot;
   Vec3f res;
   do {
     res.x = (float)rand() / (float)RAND_MAX * 2.0f - 1.0f; 
     res.y = (float)rand() / (float)RAND_MAX * 2.0f - 1.0f; 
     res.z = (float)rand() / (float)RAND_MAX * 2.0f - 1.0f;
     res.normalize();
     dot = res.dot(normal);
   } while(dot >= 0);
   return res;
}

void Scene::buildGlobalMap(int numBounces) {
   int num = 0;
   Vec3f pLight(1.0f,1.0f,1.0f);
   color_t power;
   power.r = pLight.x / (float)NUM_GPHOTONS * 100;
   power.g = pLight.y / (float)NUM_GPHOTONS * 100;
   power.b = pLight.z / (float)NUM_GPHOTONS * 100;
   while (num < NUM_GPHOTONS) {
      float x, y, z;
      do {
         x = -1.0f + (float)rand()/((float)RAND_MAX / 2.0f);
         y = -1.0f + (float)rand()/((float)RAND_MAX / 2.0f);
         z = -1.0f + (float)rand()/((float)RAND_MAX);
      } while(x*x + y*y + z*z > 1);
      Vec3f dir(x, y ,z);
      dir.normalize();
      Vec3f prevPoint = light;
      int bounces = 0;
      color_t color = power;
      Outputlist output = rayTrace(prevPoint, dir); //trace photons recurcively as the photons bounce several times.
      while (output.hit == HIT && bounces <= numBounces) {
         color = getFilteredColor(color, output.obj, output.idx, dir, output.normal, output.p_hit);
         Photon p(color*color.f);
         PhotonPoint ppoint(p, output.p_hit, dir);
         storePhoton(0, output.obj, output.idx, ppoint);
         prevPoint = output.p_hit;
         if (color.f == 1.0f) {
            float roulett = (float)rand() / (float)RAND_MAX; //russian roulett
            if (roulett < 0.3)
               break;
            if (roulett >= 0.3 && roulett < 0.7)
               dir = diffuse(output.normal);
            else
               dir = reflect(dir, output.normal);
         }
         else { // if transparent, let the photon travel through the object
            dir = refract(dir, output.normal, false);
            Vec3f start = output.p_hit + dir*0.001;
            output = rayTrace(start, dir);
            if (output.hit != HIT)
               break;
            dir = refract(dir, output.normal, true);
            prevPoint = output.p_hit + dir*0.001;
         }
         output = rayTrace(prevPoint, dir);
         bounces++;
      }
      num++;
   }
}

void Scene::buildCausticsMap(int numBounces) {
   int num = 0;
   Vec3f pLight(1.0f,1.0f,1.0f);
   color_t power;
   power.r = pLight.x / (float)NUM_CPHOTONS * 100;
   power.g = pLight.y / (float)NUM_CPHOTONS * 100;
   power.b = pLight.z / (float)NUM_CPHOTONS * 100;
   power.f = 1.0f;
   while (num < NUM_CPHOTONS) {
      float x, y, z, t_hit;
      Outputlist output;
      output.hit = MISS;
      do {
         x = -1.0f + (float)rand()/((float)RAND_MAX / 2.0f);
         y = -1.0f + (float)rand()/((float)RAND_MAX / 2.0f);
         z = -1.0f + (float)rand()/((float)RAND_MAX / 2.0f);
         Vec3f temp(x, y, z); temp.normalize();
         for (int i = 0; i < numSpheres; i++) {
            if (spheres[i].getColor().f < 1.0f){
               output = spheres[i].intersect(light, temp);
               if (output.hit == HIT) break;
            }
         }
      } while(output.hit != HIT);
      Vec3f dir(x, y ,z);
      dir.normalize();
      Vec3f prevPoint = light;
      int bounces = 0;
      color_t color = power;
      output = rayTrace(prevPoint, dir); //trace photons recurcively as the photons bounce several times
      if (output.hit == HIT && output.obj == SPHERE && spheres[output.idx].getColor().f < 1.0f) {
         dir = refract(dir, output.normal, false);
         Vec3f start = output.p_hit + dir*0.001;
         output = rayTrace(start, dir);
         if (output.hit == HIT){
            dir = refract(dir, output.normal, true);
            start = output.p_hit + dir*0.001;
            output = rayTrace(start, dir);
            if (output.hit == HIT) {
               Photon p(color);
               PhotonPoint ppoint(p, output.p_hit, dir);
               storePhoton(1, output.obj, output.idx, ppoint); // store photon
            }
         }
      }
      num++;
   }
}

void Scene::storePhoton(int type, int obj, int idx, PhotonPoint ppoint) {
   if (type == 0) {
      global[obj][idx][numGPhotons[obj][idx]] = ppoint;
      numGPhotons[obj][idx]++;
   }
   else {
      caustics[obj][idx][numCPhotons[obj][idx]] = ppoint;
      numCPhotons[obj][idx]++;
   }
}

color_t Scene::lookupGmap(Vec3f point, float radius, Vec3f normal, int obj, int idx) {
   color_t color(0.0f, 0.0f, 0.0f);
   
   for (int i = 0; i < numGPhotons[obj][idx]; i++) {
      Vec3f p = global[obj][idx][i].getPoint();
      float v = (p.x - point.x) * (p.x - point.x) + (p.y - point.y) * (p.y - point.y) + (p.z - point.z) * (p.z - point.z)
                 - radius * radius;
      if (v <= 0.0f) {
         Vec3f dir = global[obj][idx][i].getDir();
         float weight = max(0.0f, normal.dot(dir) * (-1));
         float distance = (point - p).getLength();
         weight *= (radius - distance) / radius;
         color = color + global[obj][idx][i].getPhoton().getPower() * weight;
      }
   }
   return color;
}

color_t Scene::lookupCmap(Vec3f point, float radius, Vec3f normal, int obj, int idx) {
   color_t color(0.0f, 0.0f, 0.0f);
   
   for (int i = 0; i < numCPhotons[obj][idx]; i++) {
      Vec3f p = caustics[obj][idx][i].getPoint();
      float v = (p.x - point.x) * (p.x - point.x) + (p.y - point.y) * (p.y - point.y) + (p.z - point.z) * (p.z - point.z)
                 - radius * radius;
      if (v <= 0.0f) {
         Vec3f dir = caustics[obj][idx][i].getDir();
         float weight = max(0.0f, normal.dot(dir) * (-1));
         float distance = (point - p).getLength();
         weight *= (radius - distance) / radius;
         color = color + caustics[obj][idx][i].getPhoton().getPower() * weight;
      }
   }
   return color;
}

color_t Scene::getFilteredColor(color_t rgbIn, int obj, int idx, Vec3f dir, Vec3f normal, Vec3f p_hit) {
   color_t color;
   if (obj == SPHERE)
      color = spheres[idx].getColor();
   else
      color = planes[idx].getColor();
   
   return filterColor(rgbIn, color);
}

color_t Scene::filterColor(color_t rgbIn, color_t filter) {
   color_t rgbOut = filter;
   if (rgbOut.r + rgbOut.g + rgbOut.b > 0) {
      rgbOut.r = (rgbIn.r + rgbIn.g + rgbIn.b)* rgbOut.r / (rgbOut.r + rgbOut.g + rgbOut.b);
      rgbOut.g = (rgbIn.r + rgbIn.g + rgbIn.b)* rgbOut.g / (rgbOut.r + rgbOut.g + rgbOut.b);
      rgbOut.b = (rgbIn.r + rgbIn.g + rgbIn.b)* rgbOut.b / (rgbOut.r + rgbOut.g + rgbOut.b);
   }
   else {
      rgbOut.r = min(rgbOut.r, rgbIn.r);
      rgbOut.g = min(rgbOut.g, rgbIn.g);
      rgbOut.b = min(rgbOut.b, rgbIn.b);
   }
   return rgbOut;
}

void Scene::init() {
   cout << "in init " << endl;
   for (int i = 0; i < 2; i++) {
      for (int j = 0; j < 5; j++) {
         numGPhotons[i][j] = 0;
         numCPhotons[i][j] = 0;
      }
   }
}

Vec3f Scene::refract(Vec3f dir, Vec3f normal, bool from) {
   float c = 0.85;
   if (from) 
      c = 1.0f / c;
   float cost2 = normal.dot(dir)*normal.dot(dir);
   cost2 = c*cost2 - c + 1.0;
   cost2 = sqrt(cost2);
   Vec3f res = dir * c + normal*(normal.dot(dir)*c-cost2);
   res.normalize();
   return res;      
}

color_t Scene::scaleColor(color_t clr) {
   float _min = 0.0f;
   float _max = 1.0f;
   _min = (clr.r < _min) ? clr.r : _min;
   _min = (clr.g < _min) ? clr.g : _min;
   _min = (clr.b < _min) ? clr.b : _min;
   if (_min < 0) {
      clr.r += -1*_min;
      clr.g += -1*_min;
      clr.b += -1*_min;
      _max += -1*_min;
   }

   _max = (clr.r > _max) ? clr.r : _max;
   _max = (clr.g > _max) ? clr.g : _max;
   _max = (clr.b > _max) ? clr.b : _max;
   clr.r = ((clr.r / _max));
   clr.g = ((clr.g / _max));
   clr.b = ((clr.b / _max));
   return clr;
}

