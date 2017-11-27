#pragma once
#include "glm.hpp"
#include "iostream"
using namespace glm;
using namespace std;
class UtilClass
{
public:
	UtilClass();
	~UtilClass();
	static vec3 getCameraRay(float posX, float posY, float winHeight, float windWidth, mat4 projectionMat, mat4 viewMat);
};

