#pragma once
#include "stdafx.h"
#include "..\glew\glew.h"	// include GL Extension Wrangler
#include "..\glfw\glfw3.h"	// include GLFW helper library
#include <stdio.h>
#include <iostream>
#include <string>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "objloader.hpp"  //include the object loader
#include <map>;
#include "camera.h"
#include "Triangle.h"

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
		map<const char*, vector<vec3>> &objectVertices,
		map<const char*, vector<vec3>> &objectNormals,
		map<const char*, vector<vec2>> &objectUVs,
		map<const char *, mat4> &objectModels,
		vec3 worldCoordinates);	

	///Basic Construrctor
	Object(const char * name,
		vector<vec3> vertices,
		vector<vec3> normals,
		vector<vec2> uvs,
		map<const char*, vector<vec3>> &objectVertices,
		map<const char*, vector<vec3>> &objectNormals,
		map<const char*, vector<vec2>> &objectUVs,
		map<const char *, mat4> &objectModels);

	void loadObjNoUVsToMap(map<const char*,	vector<vec3>> &objectVertices,
		map<const char*, vector<vec3>> &objectNormals,
		map<const char*, vector<vec2>> &objectUVs,
		map<const char *, mat4> &objectModels,
		map<const char *, vector<Triangle>>& objectTriangles);

	void loadObjToMap(map<const char*, vector<vec3>> &objectVertices,
		map<const char*,vector<vec3>> &objectNormals,
		map<const char*,vector<vec2>> &objectUVs,
		map<const char *, mat4> &objectModels,
		map<const char *, vector<Triangle>>& objectTriangles);

	void loadObjBoxToMap(map<const char*, vector<vec3>>& objectVertices,
		map<const char*, vector<vec3>>& objectNormals,
		map<const char*, vector<vec2>>& objectUVs,
		map<const char*, mat4>& objectModels,
		map<const char *, vector<Triangle>>& objectTriangles);

	//After each transformation update the object model.
	void translate(map<const char *, mat4> &objectModels, map<const char *, vector<Triangle>>& objectTriangles, vec3 changes);

	void rotate( map<const char *, mat4> &objectModels, map<const char *, vector<Triangle>>& objectTriangles, float angle, vec3 rotationAxe);

	void scale( map<const char *, mat4> &objectModels, map<const char *, vector<Triangle>>& objectTriangles, vec3 changes);

	//Update the vertices after a transformation
	void UpdateVertices();

	///fills the vectors with the triangle object which can be used for intersection and normals.
	void setIntersectionTriangle();

	bool intersect(vec3 position,vec3 ray);

	~Object();
		
	///Calculate the cube around the object
	void calculateBounderyBox();
	
	//Basic Architecture
	vector<vec3> vertices, normals;
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
};

