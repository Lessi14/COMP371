#pragma once

#include "camera.h"
using namespace glm;
class Triangle
{
public:
	Triangle(glm::vec3 vertex1, vec3 vertex2, vec3 vertex3);
	Triangle(vec3 vector1, vec3 vector2);
	~Triangle();
	vec3 getNormal();	

	glm::vec3 vertex1, vertex2, vertex3;
	vec3 vector1, vector2;
	vec3 normal;
};

