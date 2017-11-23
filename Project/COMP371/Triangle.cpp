#include "Triangle.h"


Triangle::Triangle(glm::vec3 v1, vec3 v2, vec3 v3)
{
	vertex1 = v1; vertex2 = v2; vertex3 = v3;
	vector1 = v2 - v1;
	vector2 = v3 - v1;
	normal = cross(vector1, vector2);
}

Triangle::Triangle(vec3 v1, vec3 v2)
{
	vector1 = v1;
	vector2 = v2;
	normal = cross(vector1, vector2);
}

Triangle::~Triangle()
{
}
