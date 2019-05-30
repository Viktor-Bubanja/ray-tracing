/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The sphere class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#ifndef H_CYLINDER_SHEAR
#define H_CYLINDER_SHEAR
#include <glm/glm.hpp>
#include "SceneObject.h"

/**
 * Defines a simple Cylinder located at 'center'
 * with the specified radius
 */
class CylinderShear : public SceneObject
{

private:
    glm::vec3 center;
    float radius;
    float height;

public:
    CylinderShear()
        : center(glm::vec3(0)), radius(1), height(1)  //Default constructor creates a unit cylinder
	{
		color = glm::vec3(1);
	};

    CylinderShear(glm::vec3 c, float r, float h, glm::vec3 col)
        : center(c), radius(r), height(h)
	{
		color = col;
	};

	float intersect(glm::vec3 posn, glm::vec3 dir);
	glm::vec3 normal(glm::vec3 p);

};

#endif //!H_CYLINDER_SHEAR
