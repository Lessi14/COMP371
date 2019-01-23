#include "Menu.h"
#include <mat4x4.hpp>
#include <glew.h>
#include <type_ptr.hpp>
#include <vector>
#include <map>
#include "Furniture.h"


Menu::Menu(){
}

///todo why use a loop to insert them
void Menu::add_button_vertices(float leftX, float rightX, float bottomY, float topY, vector<glm::vec3> *vertices, vector<glm::vec2> *uvs, map<int, Furniture*> *buttonObjects, int id)
{
	vector<glm::vec3> tempVertices;
	tempVertices.push_back(glm::vec3(leftX, bottomY, 0.01f));
	tempVertices.push_back(glm::vec3(rightX, topY, 0.01f));
	tempVertices.push_back(glm::vec3(rightX, bottomY, 0.01f));
	tempVertices.push_back(glm::vec3(leftX, bottomY, 0.01f));
	tempVertices.push_back(glm::vec3(leftX, topY, 0.01f));
	tempVertices.push_back(glm::vec3(rightX, topY, 0.01f));
	for (int i = 0; i < tempVertices.size(); i++)
		(*vertices).push_back(tempVertices[i]);

	tempVertices.push_back(glm::vec3(rightX, bottomY, 0.01f));
	tempVertices.push_back(glm::vec3(rightX, topY, 0.01f));
	tempVertices.push_back(glm::vec3(leftX, bottomY, 0.01f));

	vector<glm::vec2> tempUVs;
	tempUVs.push_back(glm::vec2(1.0f, 1.0f));
	tempUVs.push_back(glm::vec2(0.0f, 0.0f));
	tempUVs.push_back(glm::vec2(0.0f, 1.0f));
	tempUVs.push_back(glm::vec2(1.0f, 1.0f));
	tempUVs.push_back(glm::vec2(1.0f, 0.0f));
	tempUVs.push_back(glm::vec2(0.0f, 0.0f));
	for (int i = 0; i < tempUVs.size(); i++)
		(*uvs).push_back(tempUVs[i]);
	const char *temp = "";
	Furniture *newButton = new Furniture(id, temp, tempVertices, tempUVs, *buttonObjects);
	(*buttonObjects)[id] = newButton;
}


Menu::~Menu()
{
}
