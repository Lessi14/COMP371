#pragma once
#include "gtc/type_ptr.hpp"
#include <map>;
#include "camera.h"
#include "Triangle.h"
#include "objloader.hpp"

using namespace std;
using namespace glm;
class Object
{
public:
	Object(const char * name);
	
	///Constructor which takes  the world coordinate
	Object(const char * name,
		vector<vec3> vertices,
		vector<vec3> normals,
		vector<vec2> uvs,
		std::map<const char *, Object*>,
		vec3 worldCoordinates);	

	///Basic Construrctor
	Object(const char * name,
		vector<vec3> vertices,
		vector<vec3> normals,
		vector<vec2> uvs,
		std::map<const char *, Object*>);

	void loadObjNoUVsToMap(std::map<const char *, Object*>);

	void loadObjToMap(std::map<const char *, Object*>);

	void loadObjBoxToMap(std::map<const char *, Object*>);

	//After each transformation update the object model.
	void translate(map<const char *, Object*>& objects, vec3 changes);

	void rotate(map<const char *, Object*>& objects, float angle, vec3 rotationAxe);

	void scale(map<const char *, Object*>& objects, vec3 changes);

	//Update the vertices after a transformation
	void UpdateVertices();

	///fills the vectors with the triangle object which can be used for intersection and normals.
	void setIntersectionTriangle();

	bool intersect(vec3 position,vec3 ray, float &distanceT);

	~Object();
		
	///Calculate the cube around the object
	void calculateBounderyBox();
	
	bool collides(vector<float> collidingObjectMaxandMin);

	vector<float> getPostMaxMinBeforeTranslation(vec3 potentialNewPosition);

	vector<float> getListOfMaxAndMin();

	//Basic Architecture
	vector<vec3> vertices, normals;
	vector<vec3> defaultVertices,defaultNormals;
	vector<vec2> uvs;
	const char* name;

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

