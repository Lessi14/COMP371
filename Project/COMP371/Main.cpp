//example skeleton code
//modified from http://learnopengl.com/

#include "stdafx.h"

#include "..\glew\glew.h"	// include GL Extension Wrangler
#include "..\glfw\glfw3.h"	// include GLFW helper library
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "objloader.hpp"  //include the object loader
#include "stb_image.h"
#include <map>;
#include "camera.h"
#include "Object.h"
#include "UtilClass.h"

using namespace std;

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

glm::vec3 camera_position;
glm::vec3 triangle_scale;

glm::vec3 camera_pos = vec3(0, 0, 0);
glm::vec3 light_color = vec3(0, 0, 0);
float specular_strength = 0.0f;
glm::vec3 light_position = vec3(0, 0, 0);
float ambient_strength = 0.0f;

glm::mat4 projection_matrix;
glm::mat4 view_matrix;
glm::mat4 model_matrix;

std::vector<glm::vec3> menuVertices[3];
std::vector<glm::vec2> menuUVs[3];

//load and create a texture
unsigned int texture0, texture1, texture2, texture3, texture_menu_back, texture_menu_furniture, texture_menu_wallpaper;

map<int, Object*> objects;
map<int, Object*> buttonObjects[3];

//Which mode to render in between point, lines, and triangles
int objRenderMode = GL_TRIANGLES;

//If the menu is open
bool menu_open = false;
int menu_mode = 0;

//Mouse
double lastClickX = 0;
double lastClickY = 0;
double last_cursor_x = 0;
double last_cursor_y = 0;
GLuint currentButton = -1;
//Global variable for the shaders
GLuint vertexShader;
GLuint fragmentShader;
GLuint shaderProgram;

GLuint projectionLoc;
GLuint viewMatrixLoc;
GLuint transformLoc;
GLuint camera_pos_addr;
GLuint texture_number;

GLuint light_colour_loc;
GLuint specular_strength_loc;
GLuint light_position_loc;
GLuint ambient_strength_loc;

//Global variable for the window
GLFWwindow* window;

const char* INVERTED_WALLS_NAME = "Objects/inverted_normal_walls.obj";
const char* BED1_NAME = "Objects/bed1.obj";
const char* BED1BOX_NAME = "Objects/bed2.obj";
const char* CABINET3_NAME = "Objects/cabinet3.obj";
const char* COFFEE_TABLE1_NAME = "Objects/coffee_table1.obj";
const char* TOILET_NAME = "Objects/toilet.obj";
const char* TORCHERE1_NAME = "Objects/torchere1.obj";
const char* WALL = "Objects/wall.obj";

int selectedObject = -1;
GLuint menuVAOs[3], menuVBOs[3], menuUVVBOs[3];

GLuint VAO, VBO, EBO;
GLuint vertices_VBO, normals_VBO, uvs_VBO;
GLuint VAOWall, verticesWall, normalsWall, uvsWall;
GLuint VAOBEDBOX, vertices_BedBox_VBO, normals_BedBox_VBO, uvs_BedBox_VBO;

GLuint VAOINVERTEDWALLS, vertices_inverted_walls_VBO, normals_inverted_walls_VBO, uvs_inverted_walls_VBO;
glm::vec2 roomDimensions;

GLuint axes_VBO, axesColorsVBO;
GLuint axes_VAO;

std::vector<glm::vec3> axesVertices;
std::vector<glm::vec3> axesColors;

// Camera from object class and attributes
//Camera camera(glm::vec3(2.1f, 1.4f, -2.5f));
Camera camera(glm::vec3(2.25f, 3.29f, -4.01f));

float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


//Is called whenever the mouse moves on the window
///While certain mouse buttons are pressed, this method makes it so that the camera will move
void mouse_motion_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		cout << "x: " << xpos << endl;
		cout << "y: " << ypos << endl;
		if (last_cursor_x != NULL && last_cursor_y != NULL)
		{
			if (firstMouse)
			{
				lastX = xpos;
				lastY = ypos;
				firstMouse = false;
			}

			float xoffset = xpos - lastX;
			float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

			lastX = xpos;
			lastY = ypos;

			camera.ProcessMouseMovement(xoffset, yoffset);
		}
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
		firstMouse = false;
		lastX = xpos;
		lastY = ypos;
	}
	//update last cursor position
	last_cursor_x = xpos;
	last_cursor_y = ypos;

	float diffY = lastClickY - ypos;
	float diffX = lastClickX - xpos;
	float dempener = 0.0012;
	float modifier = diffY * dempener;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		objects[selectedObject]->translate(objects, vec3(modifier, 0.0f, 0.0f));
		cout << modifier << endl;
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		objects[selectedObject]->translate(objects, vec3(0.0f, modifier, 0.0f));
		cout << modifier << endl;
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		objects[selectedObject]->translate(objects, vec3(0.0f, 0.0f, modifier));
		cout << modifier << endl;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		if (menu_open)
		{
			int selectedButtonID = -1;
			lastClickX = last_cursor_x;
			lastClickY = last_cursor_y;
			glm::mat4 menuViewMatrix = lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
			vec3 castedRay = UtilClass::getCameraRay(last_cursor_x, last_cursor_y, HEIGHT, WIDTH, projection_matrix, menuViewMatrix);
			float distanceT = 0;
			float currentClosest = 1000;
			for (auto const &ent : buttonObjects[menu_mode])
			{
				if (ent.second->intersect(glm::vec3(0, 0, 0), castedRay, distanceT) && distanceT < currentClosest)
				{
					//Ray Interacted with button vertices
					currentClosest = distanceT;
					selectedButtonID = ent.second->id;
					cout << selectedButtonID << endl;
				}
			}
		}
		else
		{
			selectedObject = -1;
			lastClickX = last_cursor_x;
			lastClickY = last_cursor_y;
			vec3 castedRay = UtilClass::getCameraRay(last_cursor_x, last_cursor_y, HEIGHT, WIDTH, projection_matrix, view_matrix);
			float distanceT = 0;
			float currentClosest = 1000;
			for (auto const &ent2 : objects)
			{
				if (ent2.second->intersect(camera.Position, castedRay, distanceT) && distanceT < currentClosest)
				{
					//Object Selected
					currentClosest = distanceT;
					selectedObject = ent2.second->id;
					cout << selectedObject << endl;
				}
			}
		}
	}
}

///Determines the position of the mouse.
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	//last click for x and y
	lastClickX = xpos;
	lastClickY = ypos;
	double diffY = lastClickX - ypos;
	double diffX = lastClickY - xpos;

	switch (currentButton)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		currentButton = GLFW_MOUSE_BUTTON_LEFT;
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		currentButton = GLFW_MOUSE_BUTTON_RIGHT;
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		currentButton = GLFW_MOUSE_BUTTON_MIDDLE;
		break;
	default:
		break;
	}

}

//This method will be called when the window is resized and will ensure the application displays properly
void window_resize_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	projection_matrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 100.0f);
}

///Process input from the keyboard.
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS)
		camera.Reset();
}

///Key callabck
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout << key << std::endl;
	//Pressed
	if (GLFW_PRESS == action) {
		switch (key)
		{
		case GLFW_KEY_C:
			//Change the camera			
			break;
		case GLFW_KEY_S:
			//Editing camera
			break;
		case GLFW_KEY_F:
			//Flying  or viewing camera
			break;
		case GLFW_KEY_ENTER:
			menu_open = !menu_open;
			if (menu_open)
			{
				glm::mat4 menuViewMatrix = lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
				glm::mat4 inverseViewMatrix = glm::inverse(menuViewMatrix);
				glm::vec3 cameraPositionWorldSpace = glm::vec3(inverseViewMatrix[3][0], inverseViewMatrix[3][1], inverseViewMatrix[3][2]);
				//cameraPositionWorldSpace += glm::normalize(glm::vec3(0, 0, 1)) * glm::vec3(8);
				for (int i = 0; i < 3; i++)
				{
					buttonObjects[0][i]->translate(buttonObjects[0], cameraPositionWorldSpace);
					buttonObjects[0][i]->translate(buttonObjects[0], glm::normalize(glm::vec3(0, 0, 1)) * glm::vec3(8));
				}
				for (int i = 0; i < 8; i++)
				{
					buttonObjects[1][i]->translate(buttonObjects[1], cameraPositionWorldSpace);
					buttonObjects[1][i]->translate(buttonObjects[1], glm::normalize(glm::vec3(0, 0, 1)) * glm::vec3(8));
				}
				for (int i = 0; i < 6; i++)
				{
					buttonObjects[2][i]->translate(buttonObjects[2], cameraPositionWorldSpace);
					buttonObjects[2][i]->translate(buttonObjects[2], glm::normalize(glm::vec3(0, 0, 1)) * glm::vec3(8));
				}
			}
		default:
			break;
		}
	}
}

///Set the size of the room.
void setRoomSize() {
	while (roomDimensions.x < 4) {
		std::cout << "Enter room width(x): " << std::endl;
		std::cin >> roomDimensions.x;
		if (roomDimensions.x < 4)
			std::cout << "Minimum accepted value is 4.0" << std::endl;
	}
	while (roomDimensions.y < 4) {
		std::cout << "Enter room length(z): " << std::endl;
		std::cin >> roomDimensions.y;
		if (roomDimensions.y < 4)
			std::cout << "Minimum accepted value is 4.0" << std::endl;
	}
}

///Read the files and create the shaders. Create main  shader program.
void setShaders()
{
	std::cout << "Setting Shaders..." << std::endl;
	// Build and compile our shader program
	// Vertex shader
	// Read the Vertex Shader code from the file
	string vertex_shader_path = "vertex.shader";
	string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_shader_path, ios::in);

	if (VertexShaderStream.is_open()) {
		string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ?\n", vertex_shader_path.c_str());
		getchar();
		exit(-1);
	}

	// Read the Fragment Shader code from the file
	string fragment_shader_path = "fragment.shader";
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_shader_path, std::ios::in);

	if (FragmentShaderStream.is_open()) {
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory?\n", fragment_shader_path.c_str());
		getchar();
		exit(-1);
	}

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(vertexShader, 1, &VertexSourcePointer, NULL);
	glCompileShader(vertexShader);
	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Fragment shader
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(fragmentShader, 1, &FragmentSourcePointer, NULL);
	glCompileShader(fragmentShader);
	// Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Link shaders
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader); //free up memory
	glDeleteShader(fragmentShader);
	std::cout << "Shaders Set." << std::endl;
}

///Set teh window component. Including height and width.
int windowSetup()
{
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	window = glfwCreateWindow(WIDTH, HEIGHT, "Interior Furniture Layout Design - Team Catchphrase", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_motion_callback);
	glfwSetFramebufferSizeCallback(window, window_resize_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// tell GLFW to capture our mouse
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); ----------------------------------------------------------------
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);


	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}
}

///Extractedd the method which creates the vbos.
void setIndividualBuffers(GLuint localVAO, GLuint verticesVBO, GLuint normalsVBO, GLuint uvsVBO, int id)
{
	glGenBuffers(1, &verticesVBO);
	glGenBuffers(1, &normalsVBO);
	glGenBuffers(1, &uvsVBO);

	glBindVertexArray(localVAO);

	glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
	glBufferData(GL_ARRAY_BUFFER, objects[id]->vertices.size() * sizeof(glm::vec3), &objects[id]->vertices.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
	glBufferData(GL_ARRAY_BUFFER, objects[id]->normals.size() * sizeof(glm::vec3), &objects[id]->normals.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, uvsVBO);
	glBufferData(GL_ARRAY_BUFFER, objects[id]->uvs.size() * sizeof(glm::vec3), &objects[id]->uvs.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void addButtonVertices(float leftX, float rightX, float bottomY, float topY, vector<glm::vec3> *vertices, vector<glm::vec2> *uvs, map<int, Object*> *buttonObjects, int id)
{
	vector<glm::vec3> tempVertices;
	tempVertices.push_back(glm::vec3(leftX, bottomY, 0.01f));
	tempVertices.push_back(glm::vec3(rightX, topY, 0.01f));
	tempVertices.push_back(glm::vec3(rightX, bottomY, 0.01f));
	tempVertices.push_back(glm::vec3(leftX, bottomY, 0.01f));
	tempVertices.push_back(glm::vec3(leftX, topY, 0.01f));
	tempVertices.push_back(glm::vec3(rightX, topY, 0.01f));
	for (int i = 0; i<tempVertices.size(); i++)
		(*vertices).push_back(tempVertices[i]);
	
	vector<glm::vec2> tempUVs;
	tempUVs.push_back(glm::vec2(1.0f, 1.0f));
	tempUVs.push_back(glm::vec2(0.0f, 0.0f));
	tempUVs.push_back(glm::vec2(0.0f, 1.0f));
	tempUVs.push_back(glm::vec2(1.0f, 1.0f));
	tempUVs.push_back(glm::vec2(1.0f, 0.0f));
	tempUVs.push_back(glm::vec2(0.0f, 0.0f));
	for (int i = 0; i<tempUVs.size(); i++)
		(*uvs).push_back(tempUVs[i]);
	const char *temp = "";
	vector<glm::vec3> normals;
	Object *newButton = new Object(id, temp, tempVertices, normals, tempUVs, *buttonObjects);
	(*buttonObjects)[id] = newButton;
}

void createMenuVertices()
{
	//First Menu
	//Furniture
	addButtonVertices(-3.0f, 3.0f, 2.0f, 4.0f, &menuVertices[0], &menuUVs[0], &buttonObjects[0], 0);
	//Wallpaper
	addButtonVertices(-3.0f, 3.0f, -1.0f, 1.0f, &menuVertices[0], &menuUVs[0], &buttonObjects[0], 1);
	//Back
	addButtonVertices(-3.0f, 3.0f, -4.0f, -2.0f, &menuVertices[0], &menuUVs[0], &buttonObjects[0], 2);

	//Texture Menu
	addButtonVertices(-5.0f, -1.0f, 3.0f, 4.0f, &menuVertices[1], &menuUVs[1], &buttonObjects[1], 0);
	addButtonVertices(1.0f, 5.0f, 3.0f, 4.0f, &menuVertices[1], &menuUVs[1], &buttonObjects[1], 1);
	addButtonVertices(-5.0f, -1.0f, 1.0f, 2.0f, &menuVertices[1], &menuUVs[1], &buttonObjects[1], 2);
	addButtonVertices(1.0f, 5.0f, 1.0f, 2.0f, &menuVertices[1], &menuUVs[1], &buttonObjects[1], 3);
	addButtonVertices(-5.0f, -1.0f, -1.0f, 0.0f, &menuVertices[1], &menuUVs[1], &buttonObjects[1], 4);
	addButtonVertices(1.0f, 5.0f, -1.0f, 0.0f, &menuVertices[1], &menuUVs[1], &buttonObjects[1], 5);
	addButtonVertices(-5.0f, -1.0f, -3.0f, -2.0f, &menuVertices[1], &menuUVs[1], &buttonObjects[1], 6);
	addButtonVertices(1.0f, 5.0f, -3.0f, -2.0f, &menuVertices[1], &menuUVs[1], &buttonObjects[1], 7);

	//Furniture Menu
	addButtonVertices(-5.0f, -1.0f, 2.0f, 4.0f, &menuVertices[2], &menuUVs[2], &buttonObjects[2], 0);
	addButtonVertices(1.0f, 5.0f, 2.0f, 4.0f, &menuVertices[2], &menuUVs[2], &buttonObjects[2], 1);
	addButtonVertices(-5.0f, -1.0f, -1.0f, 1.0f, &menuVertices[2], &menuUVs[2], &buttonObjects[2], 2);
	addButtonVertices(1.0f, 5.0f, -1.0f, 1.0f, &menuVertices[2], &menuUVs[2], &buttonObjects[2], 3);
	addButtonVertices(-5.0f, -1.0f, -4.0f, -2.0f, &menuVertices[2], &menuUVs[2], &buttonObjects[2], 4);
	addButtonVertices(1.0f, 5.0f, -4.0f, -2.0f, &menuVertices[2], &menuUVs[2], &buttonObjects[2], 5);
}

///Set the VAO, VBOS for the vertices, UVs and the normals.
void setVBOs()
{
	//start axes
	axesVertices.push_back({ 0.0f, 0.0f, 0.01f });
	axesVertices.push_back({ 5.0f, 0.0f, 0.01f });
	axesVertices.push_back({ 0.0f, 0.0f, 0.01f });
	axesVertices.push_back({ 0.0f, 5.0f, 0.01f });
	axesVertices.push_back({ 0.0f, 0.0f, 0.0f });
	axesVertices.push_back({ 0.0f, 0.0f, 5.0f });

	axesColors.push_back({ 0.0, 1.0, 0.0, });
	axesColors.push_back({ 0.0, 1.0, 0.0, });
	axesColors.push_back({ 1.0, 0.0, 0.0, });
	axesColors.push_back({ 1.0, 0.0, 0.0, });
	axesColors.push_back({ 0.0, 0.0, 1.0, });
	axesColors.push_back({ 0.0, 0.0, 1.0, });

	glGenVertexArrays(1, &axes_VAO);
	glBindVertexArray(axes_VAO);

	glGenBuffers(1, &axes_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, axes_VBO);
	glBufferData(GL_ARRAY_BUFFER, axesVertices.size() * sizeof(glm::vec3), &axesVertices.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &axesColorsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, axesColorsVBO);
	glBufferData(GL_ARRAY_BUFFER, axesColors.size() * sizeof(glm::vec3), &axesColors.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//Menus
	createMenuVertices();
	glGenVertexArrays(3, menuVAOs);
	glGenBuffers(3, menuVBOs);
	glGenBuffers(3, menuUVVBOs);

	glBindVertexArray(menuVAOs[0]);
	glBindBuffer(GL_ARRAY_BUFFER, menuVBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, menuVertices[0].size() * sizeof(glm::vec3), &menuVertices[0].front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, menuUVVBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, menuUVs[0].size() * sizeof(glm::vec3), &menuUVs[0].front(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(menuVAOs[1]);
	glBindBuffer(GL_ARRAY_BUFFER, menuVBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, menuVertices[1].size() * sizeof(glm::vec3), &menuVertices[1].front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, menuUVVBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, menuUVs[1].size() * sizeof(glm::vec3), &menuUVs[1].front(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(menuVAOs[2]);
	glBindBuffer(GL_ARRAY_BUFFER, menuVBOs[2]);
	glBufferData(GL_ARRAY_BUFFER, menuVertices[2].size() * sizeof(glm::vec3), &menuVertices[2].front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, menuUVVBOs[2]);
	glBufferData(GL_ARRAY_BUFFER, menuUVs[2].size() * sizeof(glm::vec3), &menuUVs[2].front(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	///----------------
	glGenVertexArrays(1, &VAO);

	glGenBuffers(1, &vertices_VBO);
	glGenBuffers(1, &normals_VBO);
	glGenBuffers(1, &uvs_VBO);

	//Bed
	setIndividualBuffers(VAO, vertices_VBO, normals_VBO, uvs_VBO, 1);
	///--------	

	//Inverted Cube
	glGenVertexArrays(1, &VAOINVERTEDWALLS);

	glGenBuffers(1, &vertices_inverted_walls_VBO);
	glGenBuffers(1, &normals_inverted_walls_VBO);
	glGenBuffers(1, &uvs_inverted_walls_VBO);
	setIndividualBuffers(VAOINVERTEDWALLS, vertices_inverted_walls_VBO, normals_inverted_walls_VBO, uvs_inverted_walls_VBO, 0);

	//Wall
	glGenVertexArrays(1, &VAOWall);

	glGenBuffers(1, &verticesWall);
	glGenBuffers(1, &normalsWall);
	glGenBuffers(1, &uvsWall);

	setIndividualBuffers(VAOWall, verticesWall, normalsWall, uvsWall, 2);

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
}

//Set the textures
void setTexture()
{
	//texture 1
	glGenTextures(1, &texture0);
	glBindTexture(GL_TEXTURE_2D, texture0);
	//set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//load image, create texture and generate mipmaps
	int twidth, theight, tnrChannels;
	//stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load("Textures/metal1.jpg", &twidth, &theight, &tnrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		std::cout << "Loaded texture0" << std::endl;
	}
	else {
		std::cout << "Failed to load texture0" << std::endl;
	}
	stbi_image_free(data);

	//texture 2
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	//set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//load image, create texture and generate mipmaps
	data = stbi_load("Textures/metal2.jpg", &twidth, &theight, &tnrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		std::cout << "Loaded texture1" << std::endl;
	}
	else {
		std::cout << "Failed to load texture1" << std::endl;
	}
	stbi_image_free(data);

	//texture 3
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	//set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//load image, create texture and generate mipmaps
	data = stbi_load("Textures/wood1.jpg", &twidth, &theight, &tnrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		std::cout << "Loaded texture2" << std::endl;
	}
	else {
		std::cout << "Failed to load texture2" << std::endl;
	}
	stbi_image_free(data);

	//texture 4
	glGenTextures(1, &texture3);
	glBindTexture(GL_TEXTURE_2D, texture3);
	//set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//load image, create texture and generate mipmaps
	data = stbi_load("Textures/wood2.jpg", &twidth, &theight, &tnrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		std::cout << "Loaded texture3" << std::endl;
	}
	else {
		std::cout << "Failed to load texture3" << std::endl;
	}
	stbi_image_free(data);

	//texture_menu_back
	glGenTextures(1, &texture_menu_back);
	glBindTexture(GL_TEXTURE_2D, texture_menu_back);
	//set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//load image, create texture and generate mipmaps
	data = stbi_load("Textures/menu_back.jpg", &twidth, &theight, &tnrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		std::cout << "Loaded texture_menu_back" << std::endl;
	}
	else {
		std::cout << "Failed to load texture_menu_back" << std::endl;
	}
	stbi_image_free(data);

	//texture_menu_furniture
	glGenTextures(1, &texture_menu_furniture);
	glBindTexture(GL_TEXTURE_2D, texture_menu_furniture);
	//set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//load image, create texture and generate mipmaps
	data = stbi_load("Textures/menu_furniture.jpg", &twidth, &theight, &tnrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		std::cout << "Loaded texture_menu_furniture" << std::endl;
	}
	else {
		std::cout << "Failed to load texture_menu_furniture" << std::endl;
	}
	stbi_image_free(data);

	//texture_menu_wallpaper
	glGenTextures(1, &texture_menu_wallpaper);
	glBindTexture(GL_TEXTURE_2D, texture_menu_wallpaper);
	//set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//load image, create texture and generate mipmaps
	/*data = stbi_load("Textures/menu_wallpaper.jpg", &twidth, &theight, &tnrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		std::cout << "Loaded texture_menu_wallpaper" << std::endl;
	}
	else {
		std::cout << "Failed to load texture_menu_wallpaper" << std::endl;
	}
	stbi_image_free(data);*/
}

///Renders the objects inside the main loop.
void render(int id, vec3 camera_pos, GLuint VAO, GLuint tex_num)
{
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(objects[id]->objectModel));
	glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));
	glUniform3fv(camera_pos_addr, 1, glm::value_ptr(camera_pos));
	glUniform1i(texture_number, tex_num);

	glUniform3fv(light_colour_loc, 1, glm::value_ptr(light_color));
	glUniform1f(specular_strength_loc, specular_strength);
	glUniform3fv(light_position_loc, 1, glm::value_ptr(light_position));
	glUniform1f(ambient_strength_loc, ambient_strength);

	glBindVertexArray(VAO);
	glDrawArrays(objRenderMode, 0, objects[id]->vertices.size());
	glBindVertexArray(0);
}

//Second version of the render button for the axes.
void render(mat4 model, vec3 camera_pos, GLuint VAO, vector<vec3> vertices)
{
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));
	glUniform3fv(camera_pos_addr, 1, glm::value_ptr(camera_pos));
	glUniform3fv(light_colour_loc, 1, glm::value_ptr(light_color));
	glUniform1f(specular_strength_loc, specular_strength);
	glUniform3fv(light_position_loc, 1, glm::value_ptr(light_position));
	glUniform1f(ambient_strength_loc, ambient_strength);

	glBindVertexArray(VAO);
	glDrawArrays(GL_LINES, 0, vertices.size());
	glBindVertexArray(0);
}

/// The MAIN function, from here we start the application and run the game loop
int main()
{
	roomDimensions.x = 0;
	roomDimensions.y = 0;
	setRoomSize();

	if (-1 == windowSetup()) {
		return -1;
	}

	// Define the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glViewport(0, 0, width, height);

	projection_matrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 1.00f, 100.0f);

	// Set depth buffer
	glEnable(GL_DEPTH_TEST);
	//glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	setShaders();
	setTexture();

	glUseProgram(shaderProgram);
	glUniform1i(glGetUniformLocation(shaderProgram, "texture0"), 0);
	glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 1);
	glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 2);
	glUniform1i(glGetUniformLocation(shaderProgram, "texture3"), 3);
	glUniform1i(glGetUniformLocation(shaderProgram, "texture_menu_back"), 20);
	glUniform1i(glGetUniformLocation(shaderProgram, "texture_menu_furniture"), 21);
	glUniform1i(glGetUniformLocation(shaderProgram, "texture_menu_wallpaper"), 22);

	Object *invWalls = new Object(0, INVERTED_WALLS_NAME);
	Object *bed = new Object(1, BED1_NAME);
	Object *wall = new Object(2, WALL);

	/*Object *cabinet = new Object(3, CABINET3_NAME);
	Object *coffee = new Object(4, COFFEE_TABLE1_NAME);
	Object *toilet = new Object(5, TOILET_NAME);
	Object *torchere = new Object(6, TORCHERE1_NAME);*/

	invWalls->loadObjToMap(objects);
	objects[invWalls->id] = invWalls;

	bed->loadObjToMap(objects);
	objects[bed->id] = bed;

	wall->loadObjToMap(objects);
	objects[wall->id] = wall;

	//objectTriangles[bed->name] = bed->triangles;

	//cabinet->loadObjToMap(objects);	
	//objects[cabinet->name] = cabinet;

	//coffee->loadObjToMap(objects);	
	//objects[coffee->name] = coffee;

	//toilet->loadObjToMap(objects);
	//objects[toilet->name] = toilet;

	//torchere->loadObjToMap(objects);	
	//objects[torchere->name] = torchere;

	setVBOs();

	triangle_scale = glm::vec3(1.0f);

	camera_pos = camera.Position;
	light_color = vec3(1.0f, 1.0f, 1.0f);
	specular_strength = 0.5f;
	light_position = vec3(0.0f, 3.0f, 0.0f);
	ambient_strength = 0.15f;

	invWalls->scale(objects, vec3(roomDimensions.x, 2, roomDimensions.y));
	wall->scale(objects, vec3(1, 0.5, 1));
	wall->translate(objects, vec3(0.5, 1, 5));
	//floor->translate(objectModels, objectTriangles, vec3(0, 0, 0));
	//bed->translate(objectModels,objectTriangles, vec3(0, 0.5, 0));
	bed->translate(objects, vec3(-2.5, 0.5, 0));

	projectionLoc = glGetUniformLocation(shaderProgram, "projection_matrix");
	viewMatrixLoc = glGetUniformLocation(shaderProgram, "view_matrix");
	transformLoc = glGetUniformLocation(shaderProgram, "model_matrix");
	camera_pos_addr = glGetUniformLocation(shaderProgram, "view_pos");
	texture_number = glGetUniformLocation(shaderProgram, "texture_number");

	//Necessary for lighthing
	light_colour_loc = glGetUniformLocation(shaderProgram, "light_colour");
	specular_strength_loc = glGetUniformLocation(shaderProgram, "specular_strength");
	light_position_loc = glGetUniformLocation(shaderProgram, "light_position");
	ambient_strength_loc = glGetUniformLocation(shaderProgram, "ambient_strength");

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		processInput(window);
		glfwPollEvents();

		camera_pos = camera.Position;
		light_color = vec3(1.0f, 1.0f, 1.0f);
		specular_strength = 0.5f;
		light_position = vec3(0.0f, 3.0f, 0.0f);
		ambient_strength = 0.15f;

		// Render
		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		view_matrix = camera.GetViewMatrix();
		model_matrix = glm::scale(model_matrix, triangle_scale);

		objects[bed->id]->translate(objects, vec3(0.01f, 0.0f, 0.0f));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, texture3);
		glActiveTexture(GL_TEXTURE20);
		glBindTexture(GL_TEXTURE_2D, texture_menu_back);
		glActiveTexture(GL_TEXTURE21);
		glBindTexture(GL_TEXTURE_2D, texture_menu_furniture);
		glActiveTexture(GL_TEXTURE22);
		glBindTexture(GL_TEXTURE_2D, texture_menu_wallpaper);

		if (!menu_open)
		{
			//walls
			render(0, camera_pos, VAOINVERTEDWALLS, 3);

			//Bed
			render(1, camera_pos, VAO, 1);

			//start axes
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(mat4(1.0f)));
			glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));
			glUniform3fv(camera_pos_addr, 1, glm::value_ptr(camera_pos));
			//Wall
			render(2, camera_pos, VAOWall, 0);
			//axes		
			render(mat4(1.0f), camera_pos, axes_VAO, axesVertices);

		}
		else
		{
			glm::mat4 menuViewMatrix = lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
			glm::mat4 inverseViewMatrix = glm::inverse(menuViewMatrix);
			glm::vec3 cameraPositionWorldSpace = glm::vec3(inverseViewMatrix[3][0], inverseViewMatrix[3][1], inverseViewMatrix[3][2]);
			glm::mat4 menu_model_matrix = mat4(1.0f);
			menu_model_matrix = glm::translate(menu_model_matrix, cameraPositionWorldSpace);
			menu_model_matrix = glm::translate(menu_model_matrix, glm::normalize(glm::vec3(0, 0, 1)) * glm::vec3(8));
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(menu_model_matrix));
			glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(menuViewMatrix));
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));
			glUniform3fv(camera_pos_addr, 1, glm::value_ptr(camera_pos));


			switch (menu_mode)
			{
				//Main Menu
			case 0:
				glBindVertexArray(menuVAOs[0]);
				glUniform1i(texture_number, 21);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glUniform1i(texture_number, 22);
				glDrawArrays(GL_TRIANGLES, 6, 6);
				glUniform1i(texture_number, 20);
				glDrawArrays(GL_TRIANGLES, 12, 6);
				break;
				//Texture Menu
			case 1:
				glBindVertexArray(menuVAOs[1]);
				glUniform1i(texture_number, 0);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glUniform1i(texture_number, 1);
				glDrawArrays(GL_TRIANGLES, 6, 6);
				glUniform1i(texture_number, 2);
				glDrawArrays(GL_TRIANGLES, 12, 6);
				glUniform1i(texture_number, 3);
				glDrawArrays(GL_TRIANGLES, 18, 6);
				glUniform1i(texture_number, 20);
				glDrawArrays(GL_TRIANGLES, 24, 6);
				glUniform1i(texture_number, 21);
				glDrawArrays(GL_TRIANGLES, 30, 6);
				glUniform1i(texture_number, 22);
				glDrawArrays(GL_TRIANGLES, 36, 6);
				glUniform1i(texture_number, 20);
				glDrawArrays(GL_TRIANGLES, 42, 6);
				break;
				//Furniture Menu
			case 2:
				glBindVertexArray(menuVAOs[2]);
				glUniform1i(texture_number, 0);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glUniform1i(texture_number, 1);
				glDrawArrays(GL_TRIANGLES, 6, 6);
				glUniform1i(texture_number, 2);
				glDrawArrays(GL_TRIANGLES, 12, 6);
				glUniform1i(texture_number, 3);
				glDrawArrays(GL_TRIANGLES, 18, 6);
				glUniform1i(texture_number, 20);
				glDrawArrays(GL_TRIANGLES, 24, 6);
				glUniform1i(texture_number, 21);
				glDrawArrays(GL_TRIANGLES, 30, 6);
				break;
			}
			glBindVertexArray(0);
		}
		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	bed = nullptr;
	//cabinet = nullptr;
	//coffee = nullptr;
	//toilet = nullptr;
	//torchere = nullptr;
	//floor = nullptr;
	wall = nullptr;
	delete bed;
	//delete cabinet;
	//delete coffee;
	//delete toilet;
	//delete torchere;
	//delete floor;
	delete wall;

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}
