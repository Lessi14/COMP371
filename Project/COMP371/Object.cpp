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

void Object::translate(map<const char*, mat4>& objectModels, map<const char *, vector<Triangle>>& objectTriangles, vec3 changes)
{
	this->objectModel *= glm::translate(mat4(1.0f), changes);
	objectModels[name] = this->objectModel;	
	UpdateVertices();
	objectTriangles[name] = this->triangles;
	//calculateBounderyBox();
}

void Object::rotate(map<const char*, mat4>& objectModels, map<const char *, vector<Triangle>>& objectTriangles, float angle, glm::vec3 rotationAxe)
{
	//reset to center
	this->objectModel = this->defaultObjectModel;
	//translate back to center
	this->objectModel *= glm::translate(mat4(1.0f), this->worldCoordinates);
	//rotate the object
	this->objectModel = glm::rotate(mat4(1.0f), angle, rotationAxe);	
	UpdateVertices();
	objectTriangles[name] = this->triangles;
	//calculateBounderyBox();
}


void Object::scale(map<const char*, mat4>& objectModels, map<const char *, vector<Triangle>>& objectTriangles, vec3 changes)
{
	this->objectModel *= glm::scale(this->objectModel, changes);
	objectModels[name] = this->objectModel;
	UpdateVertices();
	objectTriangles[name] = this->triangles;
	//calculateBounderyBox();
}

//Apply a 3d transformation to a vertex.
//https://gamedev.stackexchange.com/questions/28249/calculate-new-vertex-position-given-a-transform-matrix
//We need to apply all transformation to the vector of vertices.
//For translations for instance.
void Object::UpdateVertices()
{
	vector<vec3> transFormedVertices;
	float wComponent = 1.0f;
	for (vec3 vertex: vertices)
	{
		//Expand the vector to 4d
		vec4 expanded = vec4(vertex.x, vertex.y, vertex.z, wComponent);
		//Multiply by the transformation matrix. Which is the object model right ?
		vec4 result = this->objectModel * expanded;
		//tranforma back to 3d
		vec3 transformedVertex = vec3(result.x / wComponent, result.y / wComponent, result.z / wComponent);
		transFormedVertices.push_back(transformedVertex);
	}
	vertices.clear();
	vertices = transFormedVertices;
	setIntersectionTriangle();
}

void Object::setIntersectionTriangle()
{
	triangles.clear();
	boundingBoxTriangles.clear();
	//Create the triangles using the vertices of the object.
	for (auto incr = 0; incr < vertices.size() - 3; incr+=3)
	{
		auto vertex = vertices.at(incr);
		auto vertex2 = vertices.at(incr+1);
		auto vertex3 = vertices.at(incr+2);

		triangles.push_back(Triangle(vertex, vertex2, vertex3));		
	}

	calculateBounderyBox();
	//Create the triangles using the vertices of the bounding box.
	for (auto incr = 0; incr < boundingbox.size() - 3; incr += 3)
	{
		auto vertex = boundingbox.at(incr);
		auto vertex2 = boundingbox.at(incr + 1);
		auto vertex3 = boundingbox.at(incr + 2);

		boundingBoxTriangles.push_back(Triangle(vertex, vertex2, vertex3));
	}
}


///Method which test the intersection of a triangle and a ray.
///Implementation of the Moller Trumbone algo.
///https://en.wikipedia.org/wiki/Möller–Trumbore_intersection_algorithm
bool ray_intersect_triangle(glm::vec3 rayO, glm::vec3 rayDir, Triangle tri)
{
	const float EPSILON = 0.0000001;
	float a, f, u, v;
	const glm::vec3 edge1 = tri.vertex2 - tri.vertex1;
	const glm::vec3 edge2 = tri.vertex3 - tri.vertex1;
	//planeNormal = glm::cross(edge1, edge2);
	const glm::vec3 h = glm::cross(rayDir, edge2);
	a = glm::dot(edge1, h);
	if (a > -EPSILON && a < EPSILON)
		return false;
	f = 1 / a;
	glm::vec3 s = rayO - tri.vertex1;
	u = f * (glm::dot(s, h));
	if (u < 0.0 || u > 1.0)
		return false;
	const glm::vec3 q = glm::cross(s, edge1);
	v = f * glm::dot(rayDir, q);
	if (v < 0.0 || u + v > 1.0)
		return false;
	// At this stage we can compute t to find out where the intersection point is on the line.
	float t = f *  glm::dot(edge2, q);
	if (t > EPSILON) // ray intersection
	{
		//resultinPoint = rayO + rayDir * t;
		return true;
	}
	else // This means that there is a line intersection but not a ray intersection.
		return false;
}

///Given a a ray position and a direction. checks if the object is in the path of the ray.
bool Object::intersect(vec3 rayPosition, vec3 rayDir)
{
	bool hitBoundaryBox = false;

	///Check outer boundarybox first.	
	///Cull the triangles that are backfacing.
	vector<Triangle> reducedBoundaryBox;
	for (Triangle boundTriangles : triangles)
	{
		if (!(dot(boundTriangles.getNormal(), rayDir)> 0))
		{
			reducedBoundaryBox.push_back(boundTriangles);
		}
	}

	//check for intersection with each tringles of the boundary box
	for (Triangle localTriangleBound : triangles)
	{
		if (ray_intersect_triangle(rayPosition, rayDir, localTriangleBound))
		{
			hitBoundaryBox= true;
			break;
		}
	}

	///Precise comparison.
	/// if the outside box was hit then try expensive comparison.
	if(hitBoundaryBox)
	{
		///backaface only
		vector<Triangle> reduced;
///Cull the triangles that are backfacing.
for (Triangle element : triangles)
{
	if (!(dot(element.getNormal(), rayDir) > 0))
	{
		reduced.push_back(element);
	}
}

//check for intersection with each tringles.
for (Triangle localTriangle : triangles)
{
	if (ray_intersect_triangle(rayPosition, rayDir, localTriangle))
	{
		return true;
	}
}
	}

	return false;
}

Object::~Object()
{
}

///Credit https://www.gamedev.net/forums/topic/373547-calculating-a-bounding-box-with-vertices-given/
void Object::calculateBounderyBox()
{
	boundingbox.clear();
	vec3 maxVertex(-1000, -1000, -1000), minVertex(1000, 1000, 1000);
	maxX = 0, maxY = 0, maxZ = 0;
	minX = 0, minY = 0, minZ = 0;
	for (auto incr = 0; incr < vertices.size(); incr++)
	{
		auto vertex = vertices.at(incr);
		if (vertex.x > maxVertex.x)	maxVertex.x = vertex.x;
		if (vertex.y > maxVertex.y)	maxVertex.y = vertex.y;
		if (vertex.z > maxVertex.z)	maxVertex.z = vertex.z;

		if (vertex.x < minVertex.x)	minVertex.x = vertex.x;
		if (vertex.y < minVertex.y)	minVertex.y = vertex.y;
		if (vertex.z < minVertex.z)	minVertex.z = vertex.z;
	}

	maxX = maxVertex.x;
	maxY = maxVertex.y;
	maxZ = maxVertex.z;

	minX = minVertex.x;
	minY = minVertex.y;
	minZ = minVertex.z;

	listOfMaxAndMin = { minX, maxX, minY, maxY, minZ, maxZ };

	boundingbox.push_back(vec3(minVertex.x, minVertex.y, maxVertex.z));
	boundingbox.push_back(vec3(maxVertex.x, minVertex.y, maxVertex.z));
	boundingbox.push_back(vec3(maxVertex.x, maxVertex.y, maxVertex.z));
	boundingbox.push_back(vec3(minVertex.x, minVertex.y, maxVertex.z));
	boundingbox.push_back(vec3(maxVertex.x, maxVertex.y, maxVertex.z));
	boundingbox.push_back(vec3(minVertex.x, maxVertex.y, maxVertex.z));
	boundingbox.push_back(vec3(maxVertex.x, maxVertex.y, maxVertex.z));
	boundingbox.push_back(vec3(maxVertex.x, maxVertex.y, minVertex.z));
	boundingbox.push_back(vec3(maxVertex.x, minVertex.y, minVertex.z));
	boundingbox.push_back(vec3(maxVertex.x, maxVertex.y, maxVertex.z));
	boundingbox.push_back(vec3(maxVertex.x, minVertex.y, minVertex.z));
	boundingbox.push_back(vec3(maxVertex.x, minVertex.y, maxVertex.z));
	boundingbox.push_back(vec3(minVertex.x, minVertex.y, minVertex.z));
	boundingbox.push_back(vec3(maxVertex.x, minVertex.y, minVertex.z));
	boundingbox.push_back(vec3(maxVertex.x, maxVertex.y, minVertex.z));
	boundingbox.push_back(vec3(minVertex.x, minVertex.y, minVertex.z));
	boundingbox.push_back(vec3(maxVertex.x, maxVertex.y, minVertex.z));
	boundingbox.push_back(vec3(minVertex.x, maxVertex.y, minVertex.z));
	boundingbox.push_back(vec3(minVertex.x, minVertex.y, minVertex.z));
	boundingbox.push_back(vec3(minVertex.x, minVertex.y, maxVertex.z));
	boundingbox.push_back(vec3(minVertex.x, maxVertex.y, maxVertex.z));
	boundingbox.push_back(vec3(minVertex.x, minVertex.y, minVertex.z));
	boundingbox.push_back(vec3(minVertex.x, maxVertex.y, maxVertex.z));
	boundingbox.push_back(vec3(minVertex.x, maxVertex.y, minVertex.z));
	boundingbox.push_back(vec3(maxVertex.x, maxVertex.y, maxVertex.z));
	boundingbox.push_back(vec3(minVertex.x, maxVertex.y, maxVertex.z));
	boundingbox.push_back(vec3(minVertex.x, maxVertex.y, minVertex.z));
	boundingbox.push_back(vec3(maxVertex.x, maxVertex.y, maxVertex.z));
	boundingbox.push_back(vec3(minVertex.x, maxVertex.y, minVertex.z));
	boundingbox.push_back(vec3(maxVertex.x, maxVertex.y, minVertex.z));
	boundingbox.push_back(vec3(minVertex.x, minVertex.y, minVertex.z));
	boundingbox.push_back(vec3(maxVertex.x, minVertex.y, minVertex.z));
	boundingbox.push_back(vec3(maxVertex.x, minVertex.y, maxVertex.z));
	boundingbox.push_back(vec3(minVertex.x, minVertex.y, minVertex.z));
	boundingbox.push_back(vec3(maxVertex.x, minVertex.y, maxVertex.z));
	boundingbox.push_back(vec3(minVertex.x, minVertex.y, maxVertex.z));
}

bool Object::collides(vector<float> collidingObjectMaxandMin)
{
	bool withinX = false, withinY = false, withinZ = false;

	if ((minX <= collidingObjectMaxandMin.at(0) && collidingObjectMaxandMin.at(0) <= maxX) ||
		(minX <= collidingObjectMaxandMin.at(1) && collidingObjectMaxandMin.at(1) <= maxX) ||
		(collidingObjectMaxandMin.at(0) <= minX && minX <= collidingObjectMaxandMin.at(1)) ||
		(collidingObjectMaxandMin.at(0) <= maxX && maxX <= collidingObjectMaxandMin.at(1)))
		withinX = true;
	if ((minY <= collidingObjectMaxandMin.at(2) && collidingObjectMaxandMin.at(2) <= maxY) ||
		(minY <= collidingObjectMaxandMin.at(3) && collidingObjectMaxandMin.at(3) <= maxY) ||
		(collidingObjectMaxandMin.at(2) <= minY && minY <= collidingObjectMaxandMin.at(3)) ||
		(collidingObjectMaxandMin.at(2) <= maxY && maxY <= collidingObjectMaxandMin.at(3)))
		withinY = true;
	if ((minZ <= collidingObjectMaxandMin.at(4) && collidingObjectMaxandMin.at(4) <= maxZ) ||
		(minZ <= collidingObjectMaxandMin.at(5) && collidingObjectMaxandMin.at(5) <= maxZ) ||
		(collidingObjectMaxandMin.at(4) <= minZ && minZ <= collidingObjectMaxandMin.at(5)) ||
		(collidingObjectMaxandMin.at(4) <= maxZ && maxZ <= collidingObjectMaxandMin.at(5)))
		withinZ = true;
	if (withinX && withinY && withinZ) return true;
	
	return false;	
}

void Object::loadObjNoUVsToMap(map<const char*, vector<vec3>> &objectVertices,
	map<const char*, vector<vec3>> &objectNormals,
	map<const char*, vector<vec2>> &objectUVs,
	map<const char *, mat4> &objectModels,
	map<const char *, vector<Triangle>>& objectTriangles)
{
	loadOBJNoUV(name, vertices, normals);
	setIntersectionTriangle();
	calculateBounderyBox();
	objectVertices[name] = vertices;
	objectNormals[name] = normals;
	objectTriangles[name] = triangles;
}

void Object::loadObjToMap(map<const char*, vector<vec3>>& objectVertices,
	map<const char*, vector<vec3>>& objectNormals,
	map<const char*, vector<vec2>>& objectUVs,
	map<const char *, mat4> &objectModels,
	map<const char *, vector<Triangle>>& objectTriangles)
{
	loadOBJ(name, vertices, normals, uvs);
	setIntersectionTriangle();
	calculateBounderyBox();
	objectVertices[name] = vertices;
	objectNormals[name] = normals;
	objectUVs[name] = uvs;
	objectTriangles[name] = triangles;
}


void Object::loadObjBoxToMap(map<const char*, vector<vec3>>& objectVertices,
	map<const char*, vector<vec3>>& objectNormals,
	map<const char*, vector<vec2>>& objectUVs,
	map<const char *, mat4> &objectModels,
	map<const char *, vector<Triangle>>& objectTriangles)
{
	loadOBJ(name, vertices, normals, uvs);
	setIntersectionTriangle();
	calculateBounderyBox();
	objectVertices[name] = boundingbox;
	objectNormals[name] = normals;
	objectUVs[name] = uvs;
	objectTriangles[name] = triangles;
}

vector<float> Object::getListOfMaxAndMin() {
	return listOfMaxAndMin;
}

void Object::setBoxX(float translationOnX) {
	minX += translationOnX;
	maxX += translationOnX;
	listOfMaxAndMin.at(0) = minX;
	listOfMaxAndMin.at(1) = maxX;
}