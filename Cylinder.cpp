/*----------------------------------------------------------
*
*  The cylinder class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cylinder.h"
#include <math.h>

/**
* Cylinders's intersection method.  The input is a ray (pos, dir).
*/
float Cylinder::intersect(glm::vec3 posn, glm::vec3 dir)
{
    float a = pow(dir.x, 2) + pow(dir.z, 2);
    float b = 2*(dir.x*(posn.x - center.x) + dir.z*(posn.z - center.z));
    float c = pow(posn.x - center.x, 2) + pow(posn.z - center.z, 2) - pow(radius, 2);
    float delta = pow(b, 2) - 4*a*c;

    if (delta < 0.0) return -1.0; // No solutions to quadratic equation

    float t1 = (-b - sqrt(delta))/(2*a);
    float t2 = (-b + sqrt(delta))/(2*a);
    // Drawing cap:
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
glm::vec3 Cylinder::normal(glm::vec3 p)
{
    glm::vec3 n = glm::vec3(p.x - center.x, 0, p.z - center.z);
    n = glm::normalize(n);
    return n;
}
