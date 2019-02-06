#pragma once
#include <glew.h>
#include <type_vec3.hpp>
#include <map>
#include "Furniture.h"

class Menu
{
public:
	Menu();
	static void add_button_vertices(float leftX, float rightX, float bottomY, float topY, vector<glm::vec3>* vertices, vector<glm::vec2>* uvs, map<int, Furniture*>* buttonObjects, int id);

	~Menu();
};

