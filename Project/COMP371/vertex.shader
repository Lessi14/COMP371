
#version 330 core
  
layout (location = 0) in vec3 position;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform vec3 view_pos;

uniform vec3 light_colour;
uniform float specular_strength;
uniform vec3 light_position;
uniform float ambient_strength;

out vec3 norm;
out vec3 col;
out vec3 fragPosition;
out vec3 viewPos;
out vec2 TexCoord;

out vec3 lightColour;
out float specularStrength;
out vec3 lightPosition;
out float ambientStrength;

void main()
{
	viewPos = view_pos;
	lightColour = light_colour;
	specularStrength = specular_strength;
	lightPosition = light_position;
	ambientStrength = ambient_strength;

	//col = vec3(mod(position.y,1.0), mod(position.y, 1.0), mod(position.y, 1.0));
	col = vec3(0.4,0.4,0.4);
	norm = vec3(transpose(inverse(model_matrix)) * vec4(normals, 1.0));
	fragPosition = vec3(model_matrix * vec4(position, 1.0));
	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(position, 1.0);  
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}