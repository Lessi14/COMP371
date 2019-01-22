#pragma once
#include <type_vec2.hpp>
#include <glm.hpp>
#include "Furniture.h"

class Room
{
public:
	Room(glm::vec2&  room_dimensions);
	Room();

	glm::vec2 room_dimensions;

	static glm::vec3 default_furniture_location;

	int add_furniture(const char* type, glm::vec3 position, map<int, Furniture*>& objects);

	void setIndividualBuffers(GLuint localVAO, GLuint verticesVBO, GLuint normalsVBO, GLuint uvsVBO, int id, map<int, Furniture*>& objects);

	void set_furniture(int texture_number, const char* furniture_name, map<int,Furniture*>& objects);


	~Room();
};

