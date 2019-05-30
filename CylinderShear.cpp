/*----------------------------------------------------------
*
*  The cylinder class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "CylinderShear.h"
#include <math.h>

/**
* Cylinders's intersection method.  The input is a ray (pos, dir).
*/
float CylinderShear::intersect(glm::vec3 posn, glm::vec3 dir)
{

    float k = 0;

    float A = posn.x - center.x - k*posn.y;
    float B = posn.z - center.z;
    float C = dir.x - k*dir.y;

    float a = pow(C, 2) + pow(dir.z , 2);
    float b = 2*A*C + 2*B*dir.x;
    float c = pow(A, 2) + pow(B, 2) - pow(radius, 2);
    float delta = pow(b, 2) - 4*a*c;

    if(fabs(delta) < 0.001) return -1.0;
    if(delta < 0.0) return -1.0;

    float t1 = (-b - sqrt(delta))/(2*a);
    float t2 = (-b + sqrt(delta))/(2*a);
    if ((posn.y + t1*dir.y) > (center.y + height)) {
        if ((posn.y + t2*dir.y) > (center.y + height)) return -1;
        return (center.y + height - posn.y)/dir.y;
    }
    return (t1 < t2)? t1: t2;
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the cylinder.
*/
glm::vec3 CylinderShear::normal(glm::vec3 p)
{
    float k = 0;
    glm::vec3 n = glm::vec3(p.x - center.x - k*p.y, 0, p.z - center.z);
    n = glm::normalize(n);
    return n;
}
