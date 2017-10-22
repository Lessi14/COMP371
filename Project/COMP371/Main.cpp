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

using namespace std;

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

glm::vec3 camera_position;
glm::vec3 triangle_scale;

glm::mat4 projection_matrix;
glm::mat4 view_matrix;
glm::mat4 model_matrix;

//Which mode to render in between point, lines, and triangles
int objRenderMode = GL_TRIANGLES;

//Last location of the cursor
double last_cursor_x;
double last_cursor_y;

const char* INVERTED_CUBE_NAME = "Objects/inverted_normal_cube1.obj";
const char* BED1_NAME = "Objects/bed1.obj";
const char* CABINET3_NAME = "Objects/cabinet3.obj";
const char* COFFE_TABLE1_NAME = "Objects/coffee_table1.obj";
const char* TOILET_NAME = "Objects/toilet.obj";
const char* TORCHERE1_NAME = "Objects/torchere1.obj";

// Constant vectors
const glm::vec3 center(0.0f, 0.0f, 0.0f);
const glm::vec3 up(0.0f, 1.0f, 0.0f);
const glm::vec3 eye(0.0f, 0.0f, 30.0f);

//Is called whenever the mouse moves on the window
//While certain mouse buttons are pressed, this method makes it so that the camera will move
void mouse_motion_callback(GLFWwindow* window, double xpos, double ypos)
{
	/*if (last_cursor_x != NULL && last_cursor_y != NULL)
	{
		int leftState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
		int rightState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
		int middleState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
		if (leftState == GLFW_PRESS)
		{
			glm::mat4 trans;
			if (xpos > last_cursor_x)
			{
				trans = glm::rotate(trans, glm::radians(2.0f), glm::vec3(1.0, 0.0, 0.0));
				view_matrix = view_matrix * trans;
			}
			else if (xpos < last_cursor_x)
			{
				trans = glm::rotate(trans, glm::radians(2.0f), glm::vec3(-1.0, 0.0, 0.0));
				view_matrix = view_matrix * trans;
			}
		}
		else if (rightState == GLFW_PRESS)
		{
			glm::mat4 trans;
			if (ypos > last_cursor_y)
			{
				trans = glm::rotate(trans, glm::radians(2.0f), glm::vec3(0.0, 1.0, 0.0));
				view_matrix = view_matrix * trans;
			}
			else if (ypos < last_cursor_y)
			{
				trans = glm::rotate(trans, glm::radians(2.0f), glm::vec3(0.0, -1.0, 0.0));
				view_matrix = view_matrix * trans;
			}
		}
		else if (middleState == GLFW_PRESS)
		{
			glm::mat4 trans;
			if (ypos > last_cursor_y)
			{
				trans = glm::rotate(trans, glm::radians(2.0f), glm::vec3(0.0, 0.0, 1.0));
				view_matrix = view_matrix * trans;
			}
			else if (ypos < last_cursor_y)
			{
				trans = glm::rotate(trans, glm::radians(2.0f), glm::vec3(0.0, 0.0, -1.0));
				view_matrix = view_matrix * trans;
			}
		}
	}
	//update last cursor position
	last_cursor_x = xpos;
	last_cursor_y = ypos;*/
}

//This method will be called when the window is resized and will ensure the application displays properly
void window_resize_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);

	projection_matrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 100.0f, -0.01f);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	glm::mat4 trans;
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
		//WASD moves the camera left, right, up, and down
		case GLFW_KEY_W:
			trans = glm::translate(trans, glm::vec3(0.0, 0.0, 3.0));
			view_matrix = view_matrix * trans;
			break;
		case GLFW_KEY_A:
			trans = glm::translate(trans, glm::vec3(3.0, 0.0, 0.0));
			view_matrix = view_matrix * trans;
			break;
		case GLFW_KEY_S:
			trans = glm::translate(trans, glm::vec3(0.0, 0.0, -3.0));
			view_matrix = view_matrix * trans;
			break;
		case GLFW_KEY_D:
			trans = glm::translate(trans, glm::vec3(-3.0, 0.0, 0.0));
			view_matrix = view_matrix * trans;
			break;
			//Q moves up
		case GLFW_KEY_Q:
			trans = glm::translate(trans, glm::vec3(0.0, -3.0, 0.0));
			view_matrix = view_matrix * trans;
			break;
			//E moves down
		case GLFW_KEY_E:
			trans = glm::translate(trans, glm::vec3(0.0, 3.0, 0.0));
			view_matrix = view_matrix * trans;
			break;
			//Arrow keys rotate the camera
		case GLFW_KEY_LEFT:
			trans = glm::rotate(trans, glm::radians(2.0f), glm::vec3(0.0, 1.0, 0.0));
			view_matrix = view_matrix * trans;
			break;
		case GLFW_KEY_RIGHT:
			trans = glm::rotate(trans, glm::radians(2.0f), glm::vec3(0.0, -1.0, 0.0));
			view_matrix = view_matrix * trans;
			break;
		case GLFW_KEY_UP:
			trans = glm::rotate(trans, glm::radians(2.0f), glm::vec3(1.0, 0.0, 0.0));
			view_matrix = view_matrix * trans;
			break;
		case GLFW_KEY_DOWN:
			trans = glm::rotate(trans, glm::radians(2.0f), glm::vec3(-1.0, 0.0, 0.0));
			view_matrix = view_matrix * trans;
			break;
			//Home key resets the view
		case GLFW_KEY_HOME:
			view_matrix = glm::lookAt(eye, center, up);
			break;
			//P, L, and T set the render mode to points, lines, and triagles
		case GLFW_KEY_P:
			objRenderMode = GL_POINTS;
			break;
		case GLFW_KEY_L:
			objRenderMode = GL_LINES;
			break;
		case GLFW_KEY_T:
			objRenderMode = GL_TRIANGLES;
			break;
		}
	}
}

// The MAIN function, from here we start the application and run the game loop
int main()
{
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Load one cube", nullptr, nullptr);
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

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// Define the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glViewport(0, 0, width, height);

	projection_matrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 100.0f, -0.01f);

	// Set depth buffer
	/*glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);*/

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

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
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
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
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
	GLuint shaderProgram = glCreateProgram();
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

	glUseProgram(shaderProgram);

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> UVs;
	loadOBJNoUV("Objects/pacman.obj", vertices, normals);
	//loadOBJ(BED1_NAME, vertices, normals, UVs);

	GLuint VAO, VBO,EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	GLuint vertices_VBO, normals_VBO, uvs_VBO;

	glGenBuffers(1, &vertices_VBO);
	glGenBuffers(1, &normals_VBO);
	glGenBuffers(1, &uvs_VBO);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	/*glBindBuffer(GL_ARRAY_BUFFER, uvs_VBO);
	glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec3), &UVs.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(2);*/

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

	triangle_scale = glm::vec3(1.0f);

	GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection_matrix");
	GLuint viewMatrixLoc = glGetUniformLocation(shaderProgram, "view_matrix");
	GLuint transformLoc = glGetUniformLocation(shaderProgram, "model_matrix");

	view_matrix = glm::lookAt(eye, center, up);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		model_matrix = glm::scale(model_matrix, triangle_scale);

		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
		glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

		glBindVertexArray(VAO);
		glDrawArrays(objRenderMode, 0, vertices.size());
		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}
