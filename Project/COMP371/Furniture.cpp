#include "Furniture.h"
#include <iostream>
//Set the counter
int Furniture::counter = -1;
Furniture::Furniture(int id, const char * type)
{
	this->id = ++counter;
	this->type = type;
	texture_number = 1;
	ambientColor = vec3(0.4f, 0.4f, 0.4f);
}

Furniture::Furniture(int id,
	const char * type,
	vector<vec3> vertices,
	vector<vec3> normals,
	vector<vec2> uvs,
	map<int, Furniture*> objects,
	vec3 worldCoordinates)
{
	this->id = id;
	this->type = type;
	this->vertices = vertices;
	this->defaultVertices = vertices;
	this->defaultNormals = normals;
	this->normals = normals;
	this->uvs = uvs;
	objects[id] = this;
	this->worldCoordinates = worldCoordinates;
	setIntersectionTriangle();
	calculateBounderyBox();
	this->id = ++counter;
	texture_number = 1;
	objects[id] = this;
	ambientColor = vec3(0.4f, 0.4f, 0.4f);
}

Furniture::Furniture(int id,
	const char * type,
	std::vector<glm::vec3> vertices,
	std::vector<glm::vec3> normals,
	std::vector<glm::vec2> uvs,
	std::map<int, Furniture*> objects
)
{
	this->id = id;
	this->type = type;
	this->vertices = vertices;
	this->defaultVertices = vertices;
	this->defaultNormals = normals;
	this->normals = normals;
	this->uvs = uvs;
	objects[id] = this;

	setIntersectionTriangle();
	calculateBounderyBox();
	this->id = ++counter;
	texture_number = 1;
	objects[id] = this;
	ambientColor = vec3(0.4f, 0.4f, 0.4f);
}

//Buttons
Furniture::Furniture(int id,
	const char * type,
	std::vector<glm::vec3> vertices,
	std::vector<glm::vec2> uvs,
	std::map<int, Furniture*> &objects
)
{
	this->id = id;
	this->type = type;
	this->vertices = vertices;
	this->defaultVertices = vertices;
	this->uvs = uvs;

	setIntersectionTriangle();
	calculateBounderyBox();
	texture_number = 1;
	objects[id] = this;
}

void Furniture::translate(map<int, Furniture*>& objects, vec3 changes)
{
	this->centerCoordinates += changes;
	//this->objectModel *= glm::translate(mat4(1.0f), changes);
	this->objectModel = mat4(1.0f);
	this->objectModel *= glm::translate(mat4(1.0f), this->centerCoordinates);
	this->objectModel *= glm::rotate(mat4(1.0f), glm::radians(this->angle), glm::vec3(0, 1, 0));
	UpdateVertices();
	setIntersectionTriangle();
	objects[id] = this;
	cout << "translated to " << endl;
	cout << this->centerCoordinates.x << endl;
	cout << this->centerCoordinates.y << endl;
	cout << this->centerCoordinates.z << endl;
}

void Furniture::rotate(map<int, Furniture*>& objects, float angle, glm::vec3 rotationAxe)
{
	this->angle += angle;
	//vec3 previousPoint = this->centerCoordinates;
	cout << "rotating" << endl;
	//rotate the object
	//this->translate(objects, -this->centerCoordinates);
	this->objectModel = mat4(1.0f);
	this->objectModel *= glm::translate(mat4(1.0f), this->centerCoordinates);
	this->objectModel *= glm::rotate(mat4(1.0f), glm::radians(this->angle), rotationAxe);
	cout << "done" << endl;
	UpdateVertices();
	setIntersectionTriangle();
	objects[id] = this;
}


void Furniture::scale(map<int, Furniture*>& objects, vec3 changes)
{
	this->objectModel *= glm::scale(this->objectModel, changes);
	UpdateVertices();
	setIntersectionTriangle();
	objects[id] = this;
}

//Apply a 3d transformation to a vertex.
//https://gamedev.stackexchange.com/questions/28249/calculate-new-vertex-position-given-a-transform-matrix
//We need to apply all transformation to the vector of vertices.
//For translations for instance.
void Furniture::UpdateVertices()
{
	vector<vec3> transFormedVertices;
	float wComponent = 1.0f;
	for (vec3 vertex : this->defaultVertices)
	{
		//Expand the vector to 4d
		vec4 expanded = vec4(vertex.x, vertex.y, vertex.z, wComponent);
		//Multiply by the transformation matrix. Which is the object model right ?			
		vec4 result = this->objectModel * expanded;
		//tranforma back to 3d
		vec3 transformedVertex = vec3(result.x / wComponent, result.y / wComponent, result.z / wComponent);
		transFormedVertices.push_back(transformedVertex);
	}
	this->vertices.clear();
	this->vertices = transFormedVertices;
}

void Furniture::resetObjectModel(map<int, Furniture*>& objects)
{
	this->objectModel = mat4(1.0);
	objects[id] = this;
}

/// Clears the trianglevector and the triangle and boundingbox
void Furniture::setIntersectionTriangle()
{
	this->triangles.clear();
	this->boundingBoxTriangles.clear();
	//Create the triangles using the vertices of the object.
	for (auto incr = 0; incr < this->vertices.size() - 3; incr += 3)
	{
		auto vertex = this->vertices.at(incr);
		auto vertex2 = this->vertices.at(incr + 1);
		auto vertex3 = this->vertices.at(incr + 2);

		this->triangles.push_back(Triangle(vertex, vertex2, vertex3));
	}

	calculateBounderyBox();
	//Create the triangles using the vertices of the bounding box.
	for (auto incr = 0; incr < this->boundingbox.size() - 3; incr += 3)
	{
		auto vertex = this->boundingbox.at(incr);
		auto vertex2 = this->boundingbox.at(incr + 1);
		auto vertex3 = this->boundingbox.at(incr + 2);

		this->boundingBoxTriangles.push_back(Triangle(vertex, vertex2, vertex3));
	}
}


///Method which test the intersection of a triangle and a ray.
///Implementation of the Moller Trumbone algo.
///https://en.wikipedia.org/wiki/Möller–Trumbore_intersection_algorithm
bool ray_intersect_triangle(glm::vec3 rayO, glm::vec3 rayDir, Triangle tri, float &distanceT)
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
		distanceT = t;
		return true;
	}
	else // This means that there is a line intersection but not a ray intersection.
		return false;
}


///Check intersection for buttons
bool Furniture::intersectButtons(vec3 rayPosition, vec3 rayDir)
{
	float collisionDistance = 1000;

	//check for intersection with each tringles.
	for (Triangle localTriangle : triangles)
	{
		if (ray_intersect_triangle(rayPosition, rayDir, localTriangle, collisionDistance))
		{
			return true;
		}
	}

	return false;
}

///Given a a ray position and a direction. checks if the object is in the path of the ray.
bool Furniture::intersect(vec3 rayPosition, vec3 rayDir, float &distanceT)
{
	bool hitBoundaryBox = false;
	float collisionDistance = 1000;

	///Check outer boundarybox first.	
	///Cull the triangles that are backfacing.
	vector<Triangle> reducedBoundaryBox;
	for (Triangle boundTriangles : triangles)
	{
		if (!(dot(boundTriangles.normal, rayDir) > 0))
		{
			reducedBoundaryBox.push_back(boundTriangles);
		}
	}

	//check for intersection with each tringles of the boundary box
	for (Triangle localTriangleBound : boundingBoxTriangles)
	{
		if (ray_intersect_triangle(rayPosition, rayDir, localTriangleBound, collisionDistance))
		{
			hitBoundaryBox = true;
			break;
		}
	}

	///Precise comparison.
	/// if the outside box was hit then try expensive comparison.
	if (hitBoundaryBox)
	{
		///backaface only
		vector<Triangle> reduced;
		///Cull the triangles that are backfacing.
		for (Triangle element : triangles)
		{
			if (!(dot(element.normal, rayDir) > 0))
			{
				reduced.push_back(element);
			}
		}

		//check for intersection with each tringles.
		for (Triangle localTriangle : triangles)
		{
			if (ray_intersect_triangle(rayPosition, rayDir, localTriangle, collisionDistance))
			{
				distanceT = collisionDistance;
				return true;
			}
		}
	}

	return false;
}

Furniture::~Furniture()
{
}

///Credit https://www.gamedev.net/forums/topic/373547-calculating-a-bounding-box-with-vertices-given/
void Furniture::calculateBounderyBox()
{
	this->boundingbox.clear();
	vec3 maxVertex(-1000, -1000, -1000), minVertex(1000, 1000, 1000);
	maxX = 0, maxY = 0, maxZ = 0;
	minX = 0, minY = 0, minZ = 0;
	for (auto incr = 0; incr < this->vertices.size() - 1; incr++)
	{
		auto vertex = this->vertices.at(incr);
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

	listOfMaxAndMin = { minX, maxX, minY, maxY, minZ, maxZ }; //order matters!!!!!

	//Forgive the hardcode, too tired to figure out a better way.
	this->boundingbox.push_back(vec3(minVertex.x, minVertex.y, maxVertex.z));
	this->boundingbox.push_back(vec3(maxVertex.x, minVertex.y, maxVertex.z));
	this->boundingbox.push_back(vec3(maxVertex.x, maxVertex.y, maxVertex.z));
	this->boundingbox.push_back(vec3(minVertex.x, minVertex.y, maxVertex.z));
	this->boundingbox.push_back(vec3(maxVertex.x, maxVertex.y, maxVertex.z));
	this->boundingbox.push_back(vec3(minVertex.x, maxVertex.y, maxVertex.z));
	this->boundingbox.push_back(vec3(maxVertex.x, maxVertex.y, maxVertex.z));
	this->boundingbox.push_back(vec3(maxVertex.x, maxVertex.y, minVertex.z));
	this->boundingbox.push_back(vec3(maxVertex.x, minVertex.y, minVertex.z));
	this->boundingbox.push_back(vec3(maxVertex.x, maxVertex.y, maxVertex.z));
	this->boundingbox.push_back(vec3(maxVertex.x, minVertex.y, minVertex.z));
	this->boundingbox.push_back(vec3(maxVertex.x, minVertex.y, maxVertex.z));
	this->boundingbox.push_back(vec3(minVertex.x, minVertex.y, minVertex.z));
	this->boundingbox.push_back(vec3(maxVertex.x, minVertex.y, minVertex.z));
	this->boundingbox.push_back(vec3(maxVertex.x, maxVertex.y, minVertex.z));
	this->boundingbox.push_back(vec3(minVertex.x, minVertex.y, minVertex.z));
	this->boundingbox.push_back(vec3(maxVertex.x, maxVertex.y, minVertex.z));
	this->boundingbox.push_back(vec3(minVertex.x, maxVertex.y, minVertex.z));
	this->boundingbox.push_back(vec3(minVertex.x, minVertex.y, minVertex.z));
	this->boundingbox.push_back(vec3(minVertex.x, minVertex.y, maxVertex.z));
	this->boundingbox.push_back(vec3(minVertex.x, maxVertex.y, maxVertex.z));
	this->boundingbox.push_back(vec3(minVertex.x, minVertex.y, minVertex.z));
	this->boundingbox.push_back(vec3(minVertex.x, maxVertex.y, maxVertex.z));
	this->boundingbox.push_back(vec3(minVertex.x, maxVertex.y, minVertex.z));
	this->boundingbox.push_back(vec3(maxVertex.x, maxVertex.y, maxVertex.z));
	this->boundingbox.push_back(vec3(minVertex.x, maxVertex.y, maxVertex.z));
	this->boundingbox.push_back(vec3(minVertex.x, maxVertex.y, minVertex.z));
	this->boundingbox.push_back(vec3(maxVertex.x, maxVertex.y, maxVertex.z));
	this->boundingbox.push_back(vec3(minVertex.x, maxVertex.y, minVertex.z));
	this->boundingbox.push_back(vec3(maxVertex.x, maxVertex.y, minVertex.z));
	this->boundingbox.push_back(vec3(minVertex.x, minVertex.y, minVertex.z));
	this->boundingbox.push_back(vec3(maxVertex.x, minVertex.y, minVertex.z));
	this->boundingbox.push_back(vec3(maxVertex.x, minVertex.y, maxVertex.z));
	this->boundingbox.push_back(vec3(minVertex.x, minVertex.y, minVertex.z));
	this->boundingbox.push_back(vec3(maxVertex.x, minVertex.y, maxVertex.z));
	this->boundingbox.push_back(vec3(minVertex.x, minVertex.y, maxVertex.z));
}

bool Furniture::collides(vector<float> collidingObjectMaxandMin)
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

bool Furniture::isNextACollision(map<int, Furniture*> &objects, vec3 potentialTranlation, int min, int max)
{
	bool willItCollide = false;
	for (auto const &ent2 : objects)
	{
		if (ent2.first != 0 && ent2.first != this->id) //0 stands for inverted walls
		{
			willItCollide = objects[ent2.second->id]->collides(objects[id]->getPostMaxMinBeforeTranslation(potentialTranlation));
			if (willItCollide)
			{
				break;
			}

		}
	}

	vector<float> maxAndMinOfWalls = objects[0]->getListOfMaxAndMin();
	vector<float> maxAndMinOfCurrentObject = getPostMaxMinBeforeTranslation(potentialTranlation);

	if (!willItCollide &&
		maxAndMinOfWalls.at(min) < maxAndMinOfCurrentObject.at(min) &&
		maxAndMinOfWalls.at(min) < maxAndMinOfCurrentObject.at(max) &&
		maxAndMinOfCurrentObject.at(min) < maxAndMinOfWalls.at(max) &&
		maxAndMinOfCurrentObject.at(max) < maxAndMinOfWalls.at(max))
	{
		return false;
	}

	return true;
}

void Furniture::loadObjNoUVsToMap(std::map<int, Furniture*> objects)
{
	loadOBJNoUV(type, vertices, normals);
	this->defaultVertices = vertices;
	this->defaultNormals = normals;
	setIntersectionTriangle();
	calculateBounderyBox();
	objects[id] = this;
}

void Furniture::loadObjToMap(std::map<int, Furniture*> objects)
{
	loadOBJ(type, vertices, normals, uvs);
	this->defaultVertices = vertices;
	this->defaultNormals = normals;
	setIntersectionTriangle();
	calculateBounderyBox();
	objects[id] = this;
}


void Furniture::loadObjBoxToMap(std::map<int, Furniture*> objects)
{
	loadOBJ(type, vertices, normals, uvs);
	this->defaultVertices = vertices;
	this->defaultNormals = normals;
	setIntersectionTriangle();
	calculateBounderyBox();
	objects[id] = this;
}

vector<float> Furniture::getListOfMaxAndMin()
{
	return listOfMaxAndMin;
}

vector<float> Furniture::getPostMaxMinBeforeTranslation(vec3 potentialNewPosition)
{
	vector<float> tempList;

	float tempMinX = minX, tempMaxX = maxX;
	float tempMinY = minY, tempMaxY = maxY;
	float tempMinZ = minZ, tempMaxZ = maxZ;

	tempMinX += potentialNewPosition.x;
	tempMaxX += potentialNewPosition.x;
	tempMinY += potentialNewPosition.y;
	tempMaxY += potentialNewPosition.y;
	tempMinZ += potentialNewPosition.z;
	tempMaxZ += potentialNewPosition.z;

	tempList = { tempMinX, tempMaxX, tempMinY, tempMaxY, tempMinZ, tempMaxZ };

	return tempList;
}

//Move to furniture class
 bool Furniture::isUnique(int n, vector<int> list) {
	for (int i = 0; i < list.size(); i++) {
		if (n == list.at(i)) {
			return false;
		}
	}
	return true;
}

// move to furniture class
 vec3 Furniture::randomLocationGenerator(int objectId, map<int,Furniture*>& objects, glm::vec2& roomDimensions) {
	vector<int> randomXs;
	vector<int> randomYs;
	glm::vec3 randomLocation = vec3(-1000, -1000, 1000);

	int randomX = 0, randomY = 0;

	int startingPointX = ((roomDimensions.x / 2) - 0.1) * 10;
	int startingPointY = ((roomDimensions.y / 2) - 0.1) * 10;
	int endingPositionX = (roomDimensions.x + 0.1) * 10;
	int endingPositionY = (roomDimensions.y + 0.1) * 10;

	int overAllCounter = 0;

	srand(time(0));

	while (overAllCounter != 5000)
	{

		randomX = rand() % endingPositionX - startingPointX;

		while (!isUnique(randomX, randomXs))
		{
			randomX = rand() % endingPositionX - startingPointX;
		}


		randomY = rand() % endingPositionY - startingPointY;

		while (!isUnique(randomY, randomYs))
		{
			randomY = rand() % endingPositionY - startingPointY;
		}


		randomLocation = vec3(randomX*0.1, 0.01f, randomY*0.1);

		bool checkIfItCollidesX = objects[objectId]->isNextACollision(objects, randomLocation, 0, 1); //0 and 1 stands for minX and maxX
		bool checkIfItCollidesY = objects[objectId]->isNextACollision(objects, randomLocation, 2, 3); //2 and 3 stands for minY and maxY
		bool checkIfItCollidesZ = objects[objectId]->isNextACollision(objects, randomLocation, 4, 5); //4 and 5 stands for minZ and 

		if (!checkIfItCollidesX && !checkIfItCollidesY && !checkIfItCollidesZ)
		{
			randomXs.push_back(randomX);
			randomYs.push_back(randomY);
			break;
		}

		randomLocation = vec3(-1000, -1000, 1000);
		overAllCounter++;
	}

	return randomLocation;
}
