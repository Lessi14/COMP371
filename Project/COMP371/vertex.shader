
#version 330 core
  
layout (location = 0) in vec3 position;
layout(location = 1) in vec3 normals;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

uniform vec3 view_pos;

out vec3 norm;
out vec3 col;
out vec3 fragPosition;
out vec3 viewPos;

void main()
{
	viewPos = view_pos;
	col = vec3(mod(position.y,1.0), mod(position.y, 1.0), mod(position.y, 1.0));
	norm = vec3(transpose(inverse(model_matrix)) * vec4(normals, 1.0));
	fragPosition = vec3(model_matrix * vec4(position, 1.0));
	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(position, 1.0);   
}