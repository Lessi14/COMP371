#include "Object.h"

Object::Object(const char * name)
{
	this->name = name;
}

Object::Object(const char * name,
	vector<vec3> vertices,
	vector<vec3> normals,
	vector<vec2> uvs,
	map<const char*, vector<vec3>> &objectVertices,
	map<const char*, vector<vec3>> &objectNormals,
	map<const char*, vector<vec2>> &objectUVs,
	map<const char *, mat4> &objectModels,
	vec3 worldCoordinates)
{
	this->name = name;
	this->vertices = vertices;
	this->normals = normals;
	this->uvs = uvs;
	objectVertices[name] = vertices;
	objectNormals[name] = normals;
	objectUVs[name] = uvs;
	objectModels[name] = mat4(1.0f);
	this->worldCoordinates = worldCoordinates;
}

Object::Object(const char * name,
	vector<vec3> vertices,
	vector<vec3> normals,
	vector<vec2> uvs,
	map<const char*, vector<vec3>> &objectVertices,
	map<const char*, vector<vec3>> &objectNormals,
	map<const char*, vector<vec2>> &objectUVs,
	map<const char *, mat4> &objectModels,
	vec3 worldCoordinates,
	int verticeSkip)
{
	this->name = name;
	this->vertices = vertices;
	this->normals = normals;
	this->uvs = uvs;
	objectVertices[name] = vertices;
	objectNormals[name] = normals;
	objectUVs[name] = uvs;
	objectModels[name] = mat4(1.0f);
	this->worldCoordinates = worldCoordinates;
	this->verticeSkip = verticeSkip;
}

Object::Object(const char * name,
	std::vector<glm::vec3> vertices,
	std::vector<glm::vec3> normals,
	std::vector<glm::vec2> uvs,
	map<const char*, vector<vec3>> &objectVertices,
	map<const char*, vector<vec3>> &objectNormals,
	map<const char*, vector<vec2>> &objectUVs,
	map<const char *, mat4> &objectModels)
{
	this->name = name;
	this->vertices = vertices;
	this->normals = normals;
	this->uvs = uvs;
	objectVertices[name] = vertices;
	objectNormals[name] = normals;
	objectUVs[name] = uvs;
	objectModels[name] = mat4(1.0f);
}


Object::Object(const char * name,
	std::vector<glm::vec3> vertices,
	std::vector<glm::vec3> normals,
	std::vector<glm::vec2> uvs,
	map<const char*, vector<vec3>> &objectVertices,
	map<const char*, vector<vec3>> &objectNormals,
	map<const char*, vector<vec2>> &objectUVs,
	map<const char *, mat4> &objectModels,
	int verticeSkip)
{
	this->name = name;
	this->vertices = vertices;
	this->normals = normals;
	this->uvs = uvs;
	objectVertices[name] = vertices;
	objectNormals[name] = normals;
	objectUVs[name] = uvs;
	objectModels[name] = mat4(1.0f);
	this->verticeSkip = verticeSkip;
}

void Object::loadObjNoUVsToMap(map<const char*,	vector<vec3>> &objectVertices,
	map<const char*, vector<vec3>> &objectNormals,
	map<const char*, vector<vec2>> &objectUVs,
	map<const char *, mat4> &objectModels)
{
	loadOBJNoUV(name, vertices, normals);
	objectVertices[name] = vertices;
	objectNormals[name] = normals;
}

void Object::loadObjToMap(map<const char*,vector<vec3>>& objectVertices,
	map<const char*, vector<vec3>>& objectNormals,
	map<const char*, vector<vec2>>& objectUVs,
	map<const char *, mat4> &objectModels)
{
	loadOBJ(name, vertices, normals, uvs);
	objectVertices[name] = vertices;
	objectNormals[name] = normals;
	objectUVs[name] = uvs;
}

void Object::translate(map<const char*, mat4>& objectModels, vec3 changes)
{
	this->objectModel *= glm::translate(objectModels[name], changes);
	objectModels[name] = this->objectModel;
}

void Object::rotate(map<const char*, mat4>& objectModels,float angle, glm::vec3 rotationAxe)
{
	//reset to center
	this->objectModel = this->defaultObjectModel;
	//translate back to center
	this->objectModel *= glm::translate(mat4(1.0f), this->worldCoordinates);
	//rotate the object
	this->objectModel = glm::rotate(mat4(1.0f), angle, rotationAxe);	
}


void Object::scale(map<const char*, mat4>& objectModels, vec3 changes)
{
	this->objectModel *= glm::scale(this->objectModel, changes);
	objectModels[name] = this->objectModel;
}

Object::~Object()
{
}

void Object::calculateLowPoly(int verticeSkip)
{
	for (auto incr= 0; incr < vertices.size()-3; incr+=3)
	{
		if (incr%verticeSkip == 0)
		{
			lowPolyVertices.push_back(vertices.at(incr));
			lowPolyVertices.push_back(vertices.at(incr+1));
			lowPolyVertices.push_back(vertices.at(incr+2));
			//lowPolyVerticesNormals.push_back(normals.at(incr)) //deos that make sense ?
		}
	}
}

void Object::calculateBounderyBox()
{
}

void Object::calculateCenter()
{
}
