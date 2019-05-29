/*========================================================================
* COSC 363  Computer Graphics (2018)
* Ray tracer 
* See Lab07.pdf for details.
*=========================================================================
*/
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "Cylinder.h"
#include "SceneObject.h"
#include "Ray.h"
#include "Plane.h"
#include <GL/glut.h>
#include "TextureBMP.h"
using namespace std;

const float WIDTH = 20.0;  
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int NUMDIV = 500;
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;
TextureBMP texture;

vector<SceneObject*> sceneObjects;  //A global list containing pointers to objects in the scene


//---The most important function in a ray tracer! ---------------------------------- 
//   Computes the colour value obtained by tracing a ray and finding its 
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
	glm::vec3 backgroundCol(0);
	glm::vec3 light(10, 40, -3);
	glm::vec3 ambientCol(0.2);   //Ambient color of light
    int phong = 10;
    glm::vec3 colorSum;


    ray.closestPt(sceneObjects);		//Compute the closest point of intersetion of objects with the ray`

    if(ray.xindex == -1) return backgroundCol;      //If there is no intersection return background colour

    glm::vec3 materialCol = sceneObjects[ray.xindex]->getColor(); //else return object's colour

    glm::vec3 normalVector = sceneObjects[ray.xindex]->normal(ray.xpt); //normal vector of sphere

    glm::vec3 lightVector = light - ray.xpt; //light vector
    lightVector = glm::normalize(lightVector); //normal light vector


    //----------------------------------Textures-------------------------------------------------

    //Floor texture
    if(ray.xindex == 4) {
            int modx = (int)((ray.xpt.x + 100) /10) % 2;
            int modz = (int)((ray.xpt.z + 100) /10) % 2;

           if((modx && modz) || (!modx && !modz)){
               materialCol = glm::vec3(0.8,0,0.8);}
           else{
               materialCol = glm::vec3(0,1,1);}
    }


    //-------------------------------------------------------------------------------------------

    float lDotn = glm::dot(lightVector, normalVector);

    Ray shadow(ray.xpt, lightVector);
    shadow.closestPt(sceneObjects);
    float lightDist = glm::distance(light, ray.xpt);
    glm::vec3 viewVector = -ray.dir;
    glm::vec3 reflVector = glm::reflect(-lightVector, normalVector);
    float rDotv = glm::dot(reflVector, viewVector);
    float specularTerm;
    if (rDotv < 0) specularTerm = 0.0;
    else specularTerm = pow(rDotv, phong);


    //ambientCol += specularTerm;
    if (lDotn <= 0 || (shadow.xindex>-1 && shadow.xdist < lightDist) ) {
        colorSum = ambientCol * materialCol;
    } else {
        colorSum = materialCol*ambientCol + materialCol*lDotn + specularTerm*glm::vec3(1,1,1);
    }

    if (ray.xindex == 0 && step < MAX_STEPS) {
        glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVector);
        Ray reflectedRay(ray.xpt, reflectedDir);
        glm::vec3 reflectedCol = trace(reflectedRay, step+1); //recursions
        colorSum = colorSum + (0.8f*reflectedCol);
    }

    //------------------------------------Refraction----------------------------------------------

//    if (ray.xindex == 3 && step < MAX_STEPS) {
//        float eta = 1/1.01;
//        glm::vec3 g = glm::refract(ray.dir, normalVector, eta);
//        Ray refractionRay(ray.xpt, g);
//        refractionRay.closestPt(sceneObjects);
//        glm::vec3 m = sceneObjects[refractionRay.xindex]->normal(refractionRay.xpt);
//        glm::vec3 h = glm::refract(g, -m, 1.0f/eta);
//        Ray outRay(refractionRay.xpt, h);
//        glm::vec3 refractionColor = trace(outRay, step + 1);
//        colorSum += refractionColor;
//    }

    //---------------------------------------------------------------------------------------------


    return colorSum;
}

glm::vec3 anti_aliasing(glm::vec3 eye, float size, float x_pos, float y_pos) {
    float quarters = size/4;
    float threeQuarters = size * 0.75;
    glm::vec3 image(0);

    Ray ray = Ray(eye, glm::vec3(x_pos + quarters, y_pos + quarters, -EDIST));
    ray.normalize();
    image += trace(ray, 1);

    ray = Ray(eye, glm::vec3(x_pos + quarters, y_pos + threeQuarters, -EDIST));
    ray.normalize();
    image += trace(ray, 1);

    ray = Ray(eye, glm::vec3(x_pos + threeQuarters, y_pos + quarters, -EDIST));
    ray.normalize();
    image += trace(ray, 1);

    ray = Ray(eye, glm::vec3(x_pos + threeQuarters, y_pos + threeQuarters, -EDIST));
    ray.normalize();
    image += trace(ray, 1);

    image *= glm::vec3(0.25);
    return image;
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
	float xp, yp;  //grid point
	float cellX = (XMAX-XMIN)/NUMDIV;  //cell width
	float cellY = (YMAX-YMIN)/NUMDIV;  //cell height

    glm::vec3 eye(0., 20, 0.);  //The eye position (source of primary rays) is the origin

	glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glBegin(GL_QUADS);  //Each cell is a quad.

	for(int i = 0; i < NUMDIV; i++)  	//For each grid point xp, yp
	{
		xp = XMIN + i*cellX;
		for(int j = 0; j < NUMDIV; j++)
		{
			yp = YMIN + j*cellY;

		    glm::vec3 dir(xp+0.5*cellX, yp+0.5*cellY, -EDIST);	//direction of the primary ray

		    Ray ray = Ray(eye, dir);		//Create a ray originating from the camera in the direction 'dir'
            ray.normalize();				//Normalize the direction of the ray to a unit vector
            glm::vec3 col = anti_aliasing(eye, cellX, xp, yp);
			glColor3f(col.r, col.g, col.b);
			glVertex2f(xp, yp);				//Draw each cell with its color value
			glVertex2f(xp+cellX, yp);
			glVertex2f(xp+cellX, yp+cellY);
			glVertex2f(xp, yp+cellY);
        }
    }
    glEnd();
    glFlush();
}


//---This function initializes the scene -------------------------------------------
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
    glClearColor(0, 0, 0, 1);
//    texture = TextureBMP("floor_tiles.bmp");

	//-- Create a pointer to a sphere object
    Sphere *sphere1 = new Sphere(glm::vec3(-5, 15, -200.0), 15.0, glm::vec3(0, 0, 1));
    Sphere *sphere2 = new Sphere(glm::vec3(5.0, 25.0, -70.0), 6, glm::vec3(1, 0, 0));
    Sphere *sphere3 = new Sphere(glm::vec3(20.0, 24.0, -100.0), 3, glm::vec3(0, 1, 0));
    Cylinder *cylinder = new Cylinder(glm::vec3(10, 0, -50), 5, 15, glm::vec3(0,0,1));
    Plane *plane = new Plane(glm::vec3(-20., 0, -40),
                             glm::vec3(20., 0, -40),
                             glm::vec3(20., 0, -200),
                             glm::vec3(-20., 0, -200),
                             glm::vec3(0.5, 0.5, 0));
    int boxLeftX = -10;
    int boxRightX = -6;
    int boxTopY = 9;
    int boxBottomY = 5;
    int boxCloseZ = -60;
    int boxFarZ = -70;
    Plane *boxBottom = new Plane(glm::vec3(boxLeftX, boxBottomY, boxCloseZ),
                             glm::vec3(boxRightX, boxBottomY, boxCloseZ),
                             glm::vec3(boxRightX, boxBottomY, boxFarZ),
                             glm::vec3(boxLeftX, boxBottomY, boxFarZ),
                             glm::vec3(1, 0.5, 1));
    Plane *boxTop = new Plane(glm::vec3(boxLeftX, boxTopY, boxCloseZ),
                             glm::vec3(boxRightX, boxTopY, boxCloseZ),
                             glm::vec3(boxRightX, boxTopY, boxFarZ),
                             glm::vec3(boxLeftX, boxTopY, boxFarZ),
                             glm::vec3(1, 0.5, 1));
    Plane *boxLeft = new Plane(glm::vec3(boxLeftX, boxBottomY, boxFarZ),
                             glm::vec3(boxLeftX, boxBottomY, boxCloseZ),
                             glm::vec3(boxLeftX, boxTopY, boxCloseZ),
                             glm::vec3(boxLeftX, boxTopY, boxFarZ),
                             glm::vec3(1, 0.5, 1));
    Plane *boxRight = new Plane(glm::vec3(boxRightX, boxBottomY, boxFarZ),
                             glm::vec3(boxRightX, boxBottomY, boxCloseZ),
                             glm::vec3(boxRightX, boxTopY, boxCloseZ),
                             glm::vec3(boxRightX, boxTopY, boxFarZ),
                             glm::vec3(1, 0.5, 1));
    Plane *boxClose = new Plane(glm::vec3(boxLeftX, boxBottomY, boxCloseZ),
                             glm::vec3(boxRightX, boxBottomY, boxCloseZ),
                             glm::vec3(boxRightX, boxTopY, boxCloseZ),
                             glm::vec3(boxLeftX, boxTopY, boxCloseZ),
                             glm::vec3(1, 0.5, 1));
    Plane *boxFar = new Plane(glm::vec3(boxLeftX, boxBottomY, boxFarZ),
                             glm::vec3(boxRightX, boxBottomY, boxFarZ),
                             glm::vec3(boxRightX, boxTopY, boxFarZ),
                             glm::vec3(boxLeftX, boxTopY, boxFarZ),
                             glm::vec3(1, 0.5, 1));
    sceneObjects.push_back(sphere1);
    sceneObjects.push_back(sphere2);
    sceneObjects.push_back(sphere3);
    sceneObjects.push_back(cylinder);
    sceneObjects.push_back(plane);
    sceneObjects.push_back(boxBottom);
    sceneObjects.push_back(boxTop);
    sceneObjects.push_back(boxLeft);
    sceneObjects.push_back(boxRight);
    sceneObjects.push_back(boxClose);
    sceneObjects.push_back(boxFar);
}



int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracer");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
