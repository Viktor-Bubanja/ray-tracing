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
#include "CylinderShear.h"
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
int EYE_X = 0;
int EYE_Y = 20;
int EYE_Z = 0;
TextureBMP graveltexture;
bool position = true;

vector<SceneObject*> sceneObjects;  //A global list containing pointers to objects in the scene


//---The most important function in a ray tracer! ---------------------------------- 
//   Computes the colour value obtained by tracing a ray and finding its 
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
	glm::vec3 backgroundCol(0);
    glm::vec3 light1(10, 10, -50);
    glm::vec3 light2(-10, 100, -50);
	glm::vec3 ambientCol(0.2);   //Ambient color of light
    int phong = 10;
    glm::vec3 colorSum;
    ray.closestPt(sceneObjects);		//Compute the closest point of intersection of objects with the ray`

    if(ray.xindex == -1) return backgroundCol;      //If there is no intersection return background colour

    glm::vec3 materialCol = sceneObjects[ray.xindex]->getColor(); //else return object's colour

    glm::vec3 normalVector = sceneObjects[ray.xindex]->normal(ray.xpt); //normal vector of sphere

    glm::vec3 lightVector1 = light1 - ray.xpt;
    glm::vec3 lightVector2 = light2 - ray.xpt;
    lightVector1 = glm::normalize(lightVector1);
    lightVector2 = glm::normalize(lightVector2);

    //----------------------------------Textures-------------------------------------------------

    //Floor procedural pattern
    if (ray.xindex == 5 && step < MAX_STEPS) {
        int modx = (int)((ray.xpt.x)/10) % 2;
        int modz = (int)((ray.xpt.z)/10) % 2;
        if ((modx && modz) || (!modx && !modz)) {
            materialCol = glm::vec3(0.5,0,0.5);
        } else {
            materialCol = glm::vec3(0.8, 0, 0);
        }
    }

    //Sphere procedural pattern
    if (ray.xindex == 6 && step < MAX_STEPS) {
        int modx = (int)((ray.xpt.x)*2) % 2;
        int modz = (int)((ray.xpt.z)*3) % 2;
        if ((modx && modz) || (!modx && !modz)) {
            materialCol = glm::vec3(1,0.5,0);
        } else {
            materialCol = glm::vec3(0,0,0);
        }
    }

    //Sphere texture
    if (ray.xindex == 3 && step < MAX_STEPS) {
        glm::vec3 center = glm::vec3(-15, 5, -90.0);
        glm::vec3 normal = glm::normalize(center - ray.xpt);
        float u = 0.5 + (atan2(normal.z, normal.x))/(2*M_PI);
        float v = 0.5 - (asin(normal.y)/M_PI);
        materialCol = graveltexture.getColorAt(u, v);
    }

    //-------------------------------------------------------------------------------------------

    float lDotn1 = glm::dot(lightVector1, normalVector);
    float lDotn2 = glm::dot(lightVector2, normalVector);

    Ray shadow1(ray.xpt, lightVector1);
    shadow1.closestPt(sceneObjects);
    Ray shadow2(ray.xpt, lightVector2);
    shadow2.closestPt(sceneObjects);
    float lightDist1 = glm::distance(light1, ray.xpt);
    float lightDist2 = glm::distance(light2, ray.xpt);
    glm::vec3 viewVector = -ray.dir;
    glm::vec3 reflVector1 = glm::reflect(-lightVector1, normalVector);
    glm::vec3 reflVector2 = glm::reflect(-lightVector2, normalVector);
    float rDotv1 = glm::dot(reflVector1, viewVector);
    float rDotv2 = glm::dot(reflVector2, viewVector);
    float specularTerm1;
    float specularTerm2;
    if (rDotv1 < 0) specularTerm1 = 0.0;
    else specularTerm1 = pow(rDotv1, phong);

    if (rDotv2 < 0) specularTerm2 = 0.0;
    else specularTerm2 = pow(rDotv2, phong);

    if (lDotn1 <= 0 || (shadow1.xindex>-1 && shadow1.xdist < lightDist1) ) {
        colorSum = ambientCol * materialCol;
    } else {
        colorSum = materialCol*ambientCol + materialCol*lDotn1 + specularTerm1*glm::vec3(1,1,1);
    }

    if (lDotn2 <= 0 || (shadow2.xindex>-1 && shadow2.xdist < lightDist2) ) {
        colorSum = ambientCol * materialCol;
    } else {
        colorSum = materialCol*ambientCol + materialCol*lDotn2 + specularTerm2*glm::vec3(1,1,1);
    }

    if (lDotn1 <= 0 || (shadow1.xindex>-1 && shadow1.xdist < lightDist1) ) {
        colorSum = ambientCol * materialCol;
    } else {
        colorSum = materialCol*ambientCol + materialCol*lDotn1 + specularTerm1*glm::vec3(1,1,1);
    }

    if (lDotn2 <= 0 || (shadow2.xindex>-1 && shadow2.xdist < lightDist2) ) {
        colorSum = ambientCol * materialCol;
    } else {
        colorSum = materialCol*ambientCol + materialCol*lDotn2 + specularTerm2*glm::vec3(1,1,1);
    }

    if (ray.xindex == 0 && step < MAX_STEPS) {
        glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVector);
        Ray reflectedRay(ray.xpt, reflectedDir);
        glm::vec3 reflectedCol = trace(reflectedRay, step+1);
        colorSum = colorSum + (0.95f*reflectedCol);
    }

    if (ray.xindex == 5 && step < MAX_STEPS) {
        glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVector);
        Ray reflectedRay(ray.xpt, reflectedDir);
        glm::vec3 reflectedCol = trace(reflectedRay, step+1);
        colorSum = colorSum + (0.4f*reflectedCol);
    }

    //------------------------------------Refraction----------------------------------------------

    if (ray.xindex == 2 && step < MAX_STEPS) {
        float eta = 1/1.03;
        glm::vec3 g = glm::refract(ray.dir, normalVector, eta);
        Ray refractionRay(ray.xpt, g);
        refractionRay.closestPt(sceneObjects);
        if (refractionRay.xindex != -1) {
            glm::vec3 m = sceneObjects[refractionRay.xindex]->normal(refractionRay.xpt);
            glm::vec3 h = glm::refract(g, -m, 1.0f/eta);
            Ray outRay(refractionRay.xpt, h);
            glm::vec3 refractionColor = trace(outRay, step + 1);
            colorSum += refractionColor;
        }
    }

    //------------------------------------Transparency----------------------------------------------

    if(ray.xindex == 1 && step < MAX_STEPS){
        Ray transparentRay(ray.xpt, ray.dir);
        transparentRay.closestPt(sceneObjects);
        glm::vec3 transparentColour = trace(transparentRay, step+1);
        colorSum = colorSum * 0.4f + transparentColour * 0.50f;
    }

    return colorSum;
}

glm::vec3 anti_aliasing(glm::vec3 eye, float size, float x_pos, float y_pos) {
    float quarters = size/4;
    float threeQuarters = size * 0.75;
    glm::vec3 image(0);

    Ray ray1 = Ray(eye, glm::vec3(x_pos + quarters, y_pos + quarters, -EDIST));
    ray1.normalize();
    image += trace(ray1, 1);

    Ray ray2 = Ray(eye, glm::vec3(x_pos + quarters, y_pos + threeQuarters, -EDIST));
    ray2.normalize();
    image += trace(ray2, 1);

    Ray ray3 = Ray(eye, glm::vec3(x_pos + threeQuarters, y_pos + quarters, -EDIST));
    ray3.normalize();
    image += trace(ray3, 1);

    Ray ray4 = Ray(eye, glm::vec3(x_pos + threeQuarters, y_pos + threeQuarters, -EDIST));
    ray4.normalize();
    image += trace(ray4, 1);

    image = image * glm::vec3(0.25);
    return image;
}

//---The main display module ------------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
	float xp, yp;  //grid point
	float cellX = (XMAX-XMIN)/NUMDIV;  //cell width
	float cellY = (YMAX-YMIN)/NUMDIV;  //cell height

    glm::vec3 eye(EYE_X, EYE_Y, EYE_Z);  //The eye position (source of primary rays) is the origin

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
//            glm::vec3 col = trace(ray, 0);
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
    graveltexture = TextureBMP("red_slime.BMP");

	//-- Create a pointer to a sphere object
    Sphere *reflectiveSphere = new Sphere(glm::vec3(-5, 15, -200.0), 15.0, glm::vec3(0.1, 0, 0.2));
    Sphere *transparentSphere = new Sphere(glm::vec3(7.0, 10, -70.0), 10, glm::vec3(1, 1, 1));
    Sphere *refractiveSphere = new Sphere(glm::vec3(-6, 8, -100.0), 8, glm::vec3(0.9, 0.8, 0.2));
    Sphere *texturedSphere = new Sphere(glm::vec3(-15, 5, -90.0), 5, glm::vec3(1, 0, 1));
    Sphere *proceduralPatternSphere = new Sphere(glm::vec3(25, 20, -190.0), 12, glm::vec3(1, 0, 1));
    Cylinder *cylinder = new Cylinder(glm::vec3(7, 0, -90), 2, 10, glm::vec3(0,0,1));
    Plane *floor = new Plane(glm::vec3(-30., 0, -40),
                             glm::vec3(30., 0, -40),
                             glm::vec3(30., 0, -220),
                             glm::vec3(-30., 0, -220),
                             glm::vec3(0.5, 0.5, 0));

    int boxX = 14;
    int boxY = 3;
    int boxZ = -100;
    int boxWidth = 3;
    int boxLeftX = boxX - boxWidth;
    int boxRightX = boxX + boxWidth;
    int boxTopY = boxY + boxWidth;
    int boxBottomY = boxY - boxWidth;
    int boxCloseZ = boxZ - boxWidth;
    int boxFarZ = boxZ + boxWidth;
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
    sceneObjects.push_back(reflectiveSphere);
    sceneObjects.push_back(transparentSphere);
    sceneObjects.push_back(refractiveSphere);
    sceneObjects.push_back(texturedSphere);
    sceneObjects.push_back(cylinder);
    sceneObjects.push_back(floor);
    sceneObjects.push_back(proceduralPatternSphere);
    sceneObjects.push_back(boxBottom);
    sceneObjects.push_back(boxTop);
    sceneObjects.push_back(boxLeft);
    sceneObjects.push_back(boxRight);
    sceneObjects.push_back(boxClose);
    sceneObjects.push_back(boxFar);
}

void keyboard(unsigned char key, int x, int y)
{
    float move_amount = 20.0;
    if (key == ' ') {
        if (position) {
            EYE_Y += move_amount;
            EYE_Z += 3 * move_amount;
        } else {
            EYE_Y -= move_amount;
            EYE_Z -= 3 * move_amount;
        }
        position = !position;
        glutPostRedisplay();
    }
}





int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracer");
    glutKeyboardFunc(keyboard);


    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
