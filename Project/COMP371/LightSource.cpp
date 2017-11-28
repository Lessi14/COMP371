#include "LightSource.h"

LightSource::LightSource(vec3 light_color, float specular_strength, vec3 light_position, float ambient_strength)
{
	this->light_color = light_color;
	this->specular_strength = specular_strength;
	this->light_position = light_position;
	this->ambient_strength = ambient_strength;
}


LightSource::~LightSource()
{
}
