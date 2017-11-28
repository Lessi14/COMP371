#pragma once
#include "gtc/type_ptr.hpp"

using namespace glm;
class LightSource
{
public:
	LightSource(vec3 light_color, float specular_strength, vec3 light_position, float ambient_strength);
	~LightSource();	
	vec3 light_color;
	float specular_strength;
	vec3 light_position;
	float ambient_strength;
};

