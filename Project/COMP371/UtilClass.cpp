#include "UtilClass.h"



UtilClass::UtilClass()
{
}


UtilClass::~UtilClass()
{
}


glm::vec3 UtilClass::getCameraRay(float posX, float posY, float winHeight, float windWidth, mat4 projectionMat, mat4 viewMat)
{
	//cout << posX << endl;
	//cout << posY << endl;
	//cout << "end" << endl;
	//cout << " " << endl;

	//Get the position of the Mouses
	double mouseX = posX;
	double mouseY = posY;

	//Step 1 Get the normalizedCoordinates
	float xNorm = (2.0f*mouseX) / windWidth - 1.0f;
	float yNorm = ((2.0f*mouseY) / winHeight - 1.0f)*-1.0f; //--------------------------------

															//Step 2 Get clipCoords
	glm::vec4 clipCoord(xNorm, yNorm, -1.0f, 1.0f);

	//Step 3 Get Eye coordinates //get an inverse projection matrix
	//Get the projection matrix
	glm::mat4 local_projection_matrix = projectionMat;
	//Get the inverted matrix
	glm::mat4 invertedMatrix = glm::inverse(local_projection_matrix);
	//get the eye coordinates 4d ?
	glm::vec4 temp = invertedMatrix * clipCoord;
	glm::vec4 eyeCoord(temp.x, temp.y, -1.0f, 0.0f);

	//Step 4 get world way
	//get local view matrix
	glm::mat4 local_view_matrix = viewMat;
	//get the inverse
	glm::mat4 inverted_view_matrix = glm::inverse(local_view_matrix);
	// get the ray
	glm::vec4 temp2 = inverted_view_matrix * eyeCoord;
	glm::vec3 worldRay(temp2.x, temp2.y, temp2.z);
	//glm::vec3 worldRayNorm = glm::normalize(worldRay);
	glm::vec3 worldRayNorm = (worldRay);

	//std::cout << "x: " << worldRayNorm.x << "y: " << worldRayNorm.y << "z: " << worldRayNorm.z << std::endl;
	return worldRayNorm;
}