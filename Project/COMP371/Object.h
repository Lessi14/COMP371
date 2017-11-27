#pragma once
#include "gtc/type_ptr.hpp"
#include <map>;
#include "camera.h"
#include "Triangle.h"
#include "objloader.hpp"
#include <glew.h>

using namespace std;
using namespace glm;
class Object
{
public:
	Object(int id, const char * type);

	///Constructor which takes  the world coordinate
	Object(int id,
		const char * type,
		vector<vec3> vertices,
		vector<vec3> normals,
		vector<vec2> uvs,
		map<int, Object*> obj,
		vec3 worldCoordinates);

	///Basic Construrctor
	Object(int id,
		const char * type,
		vector<vec3> vertices,
		vector<vec3> normals,
		vector<vec2> uvs,
		map<int, Object*>);

	void loadObjNoUVsToMap(map<int, Object*> obj);

	void loadObjToMap(map<int, Object*> obj);

	void loadObjBoxToMap(map<int, Object*> obj);

	//After each transformation update the object model.
	void translate(map<int, Object*>& objects, vec3 changes);

	void rotate(map<int, Object*>& objects, float angle, vec3 rotationAxe);

	void scale(map<int, Object*>& objects, vec3 changes);

	//Update the vertices after a transformation
	void UpdateVertices();

	///fills the vectors with the triangle object which can be used for intersection and normals.
	void setIntersectionTriangle();

	bool intersect(vec3 position, vec3 ray, float &distanceT);

	~Object();

	///Calculate the cube around the object
	void calculateBounderyBox();
	static int counter;
	static bool checkIdAvailability(int id);




	GLuint VAO, vertices_VBO, normals_VBO, uvs_VBO, texture_number;

	int id;
	static vector<int> ids;

	
	bool collides(vector<float> collidingObjectMaxandMin);
	bool isNextACollision(map<int, Object*> &objects, vec3 potentialTranlation, int min, int max);

	vector<float> getPostMaxMinBeforeTranslation(vec3 potentialNewPosition);

	vector<float> getListOfMaxAndMin();

	//Basic Architecture
	vector<vec3> vertices, normals;
	vector<vec3> defaultVertices, defaultNormals;
	vector<vec2> uvs;
	const char* type;

	//World position
	vec3 worldCoordinates, centerCoordinates;

	//Attributes for the  lighting or color
	glm::vec3 ambientColor, speculardColor, diffusecolor;
	float shininess;

	//Attributes for collision, raytracing possibly lighting
	vector<vec3> boundingbox;
	vector<Triangle> triangles, boundingBoxTriangles;

	//Models for the transformations
	glm::mat4 defaultObjectModel, objectModel;

private:
	float maxX, maxY, maxZ, minX, minY, minZ;
	vector<float> listOfMaxAndMin;
};

