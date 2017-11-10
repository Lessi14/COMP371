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

void Object::loadObjNoUVsToMap(map<const char*, vector<vec3>> &objectVertices,
	map<const char*, vector<vec3>> &objectNormals,
	map<const char*, vector<vec2>> &objectUVs,
	map<const char *, mat4> &objectModels)
{
	loadOBJNoUV(name, vertices, normals);
	objectVertices[name] = vertices;
	objectNormals[name] = normals;
}

void Object::loadObjToMap(map<const char*, vector<vec3>>& objectVertices,
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

void Object::rotate(map<const char*, mat4>& objectModels, float angle, glm::vec3 rotationAxe)
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
	for (auto incr = 0; incr < vertices.size() - 3; incr += 3)
	{
		if (incr%verticeSkip == 0)
		{
			lowPolyVertices.push_back(vertices.at(incr));
		}
	}
}


///Credit https://www.gamedev.net/forums/topic/373547-calculating-a-bounding-box-with-vertices-given/
void Object::calculateBounderyBox()
{
	vec3 maxVertex(0, 0, 0), minVertex(0, 0, 0);
	float maxX = 0, maxY = 0, maxZ = 0;
	float minX = 0, minY = 0, minZ = 0;
	for (auto incr = 0; incr < vertices.size() - 1; incr++)
	{
		auto vertex = vertices.at(incr);
		if (vertex.x > maxVertex.x)	maxVertex.x = vertex.x;
		if (vertex.y > maxVertex.y)	maxVertex.y = vertex.y;
		if (vertex.z > maxVertex.z)	maxVertex.z = vertex.z;

		if (vertex.x < minVertex.x)	minVertex.x = vertex.x;
		if (vertex.y < minVertex.y)	minVertex.y = vertex.y;
		if (vertex.z < minVertex.z)	minVertex.z = vertex.z;
	}	
	//Forgive the hardcode, too tired to figure out a better way.
	lowPolyVertices.push_back(vec3(minVertex.x, minVertex.y, maxVertex.z));
	lowPolyVertices.push_back(vec3(maxVertex.x, minVertex.y, maxVertex.z));
	lowPolyVertices.push_back(vec3(maxVertex.x, maxVertex.y, maxVertex.z));
	lowPolyVertices.push_back(vec3(minVertex.x, minVertex.y, maxVertex.z));
	lowPolyVertices.push_back(vec3(maxVertex.x, maxVertex.y, maxVertex.z));
	lowPolyVertices.push_back(vec3(minVertex.x, maxVertex.y, maxVertex.z));
	lowPolyVertices.push_back(vec3(maxVertex.x, maxVertex.y, maxVertex.z));
	lowPolyVertices.push_back(vec3(maxVertex.x, maxVertex.y, minVertex.z));
	lowPolyVertices.push_back(vec3(maxVertex.x, minVertex.y, minVertex.z));
	lowPolyVertices.push_back(vec3(maxVertex.x, maxVertex.y, maxVertex.z));
	lowPolyVertices.push_back(vec3(maxVertex.x, minVertex.y, minVertex.z));
	lowPolyVertices.push_back(vec3(maxVertex.x, minVertex.y, maxVertex.z));
	lowPolyVertices.push_back(vec3(minVertex.x, minVertex.y, minVertex.z));
	lowPolyVertices.push_back(vec3(maxVertex.x, minVertex.y, minVertex.z));
	lowPolyVertices.push_back(vec3(maxVertex.x, maxVertex.y, minVertex.z));
	lowPolyVertices.push_back(vec3(minVertex.x, minVertex.y, minVertex.z));
	lowPolyVertices.push_back(vec3(maxVertex.x, maxVertex.y, minVertex.z));
	lowPolyVertices.push_back(vec3(minVertex.x, maxVertex.y, minVertex.z));
	lowPolyVertices.push_back(vec3(minVertex.x, minVertex.y, minVertex.z));
	lowPolyVertices.push_back(vec3(minVertex.x, minVertex.y, maxVertex.z));
	lowPolyVertices.push_back(vec3(minVertex.x, maxVertex.y, maxVertex.z));
	lowPolyVertices.push_back(vec3(minVertex.x, minVertex.y, minVertex.z));
	lowPolyVertices.push_back(vec3(minVertex.x, maxVertex.y, maxVertex.z));
	lowPolyVertices.push_back(vec3(minVertex.x, maxVertex.y, minVertex.z));
	lowPolyVertices.push_back(vec3(maxVertex.x, maxVertex.y, maxVertex.z));
	lowPolyVertices.push_back(vec3(minVertex.x, maxVertex.y, maxVertex.z));
	lowPolyVertices.push_back(vec3(minVertex.x, maxVertex.y, minVertex.z));
	lowPolyVertices.push_back(vec3(maxVertex.x, maxVertex.y, maxVertex.z));
	lowPolyVertices.push_back(vec3(minVertex.x, maxVertex.y, minVertex.z));
	lowPolyVertices.push_back(vec3(maxVertex.x, maxVertex.y, minVertex.z));
	lowPolyVertices.push_back(vec3(minVertex.x, minVertex.y, minVertex.z));
	lowPolyVertices.push_back(vec3(maxVertex.x, minVertex.y, minVertex.z));
	lowPolyVertices.push_back(vec3(maxVertex.x, minVertex.y, maxVertex.z));
	lowPolyVertices.push_back(vec3(minVertex.x, minVertex.y, minVertex.z));
	lowPolyVertices.push_back(vec3(maxVertex.x, minVertex.y, maxVertex.z));
	lowPolyVertices.push_back(vec3(minVertex.x, minVertex.y, maxVertex.z));
}

