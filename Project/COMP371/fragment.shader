#version 330 core

out vec4 color;

in vec3 col;
in vec2 TexCoord;
in vec3 norm;
in vec3 fragPosition;
in vec3 viewPos;

uniform sampler2D texture1;
uniform sampler2D texture2;


void main()
{
	vec3 lightColour = vec3(1.0f, 1.0f, 1.0f);
	vec3 cubeColour = col;
	//vec3 cubeColour = vec3(1.0f, 0.0f, 0.0f);
	float specularStrength = 0.5;

	//light position
	vec3 light_position = vec3(0.0f, 5.0f, 0.0f); //world coords
												  //ambient lighting
	float ambientStrength = 0.15f;
	vec3 ambient_contribution = ambientStrength * lightColour;

	vec3 norm = normalize(norm);
	vec3 light_direction = normalize(light_position - fragPosition);
	float incident_degree = max(dot(norm, light_direction), 0.0f);
	vec3 diffuse_contribution = incident_degree * lightColour;

	vec3 viewDir = normalize(viewPos - fragPosition);
	vec3 reflectDir = reflect(-light_direction, norm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
	vec3 specular = specularStrength * spec * lightColour;

	vec3 resultantColour = (ambient_contribution + diffuse_contribution + specular) * cubeColour;
	//color = vec4(resultantColour, 1.0f);
	
	//Different texturing modes
	//Singular textures
	color = texture(texture1, TexCoord) * vec4(resultantColour, 1.0f);
	//color = texture(texture2, TexCoord);
} 