#pragma once
#include <type_vec2.hpp>
#include <glm.hpp>
#include "Furniture.h"

class Room
{
public:
	Room(glm::vec2& room_dimensions, map<int, Furniture*>& objects);

	glm::vec2& room_dimensions;

	map<int, Furniture*>& objects;

	static glm::vec3 default_furniture_location;

	void set_room_size();

	int add_furniture(const char* type, glm::vec3 const& position);

	void setIndividualBuffers(GLuint localVAO, GLuint verticesVBO, GLuint normalsVBO, GLuint uvsVBO, int id);

	void set_furniture(int texture_number, const char* furniture_name);


	~Room();
};

