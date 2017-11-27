#version 330 core

out vec4 color;

in vec3 col;
in vec2 TexCoord;
in vec3 norm;
in vec3 fragPosition;
in vec3 viewPos;

in vec3 lightColour;
in float specularStrength;
in vec3 lightPosition;
in float ambientStrength;

uniform int texture_number;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture_menu_back;
uniform sampler2D texture_menu_furniture;
uniform sampler2D texture_menu_wallpaper;

void main()
{	
	vec3 localLightColour = lightColour;
	vec3 cubeColour = col;	
	float localSpecularStrength = specularStrength;
	
	vec3 light_position = lightPosition;												  
	
	float localAmbientStrength = ambientStrength;
	vec3 ambient_contribution = localAmbientStrength * localLightColour;

	vec3 norm = normalize(norm);
	vec3 light_direction = normalize(light_position - fragPosition);
	float incident_degree = max(dot(norm, light_direction), 0.0f);
	vec3 diffuse_contribution = incident_degree * localLightColour;

	vec3 viewDir = normalize(viewPos - fragPosition);
	vec3 reflectDir = reflect(-light_direction, norm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
	vec3 specular = localSpecularStrength * spec * localLightColour;

	vec3 resultantColour = (ambient_contribution + diffuse_contribution + specular) * cubeColour;

	switch (texture_number) {
		case 0: //metal1 texture
			color = texture(texture0, TexCoord) * vec4(resultantColour, 1.0f);
			break;
		case 1: //metal2 texture
			color = texture(texture1, TexCoord) * vec4(resultantColour, 1.0f);
			break;
		case 2: //wood1 texture
			color = texture(texture2, TexCoord) * vec4(resultantColour, 1.0f);
			break;
		case 3: //wood2 texture
			color = texture(texture3, TexCoord) * vec4(resultantColour, 1.0f);
			break;

		case 20: //texture_menu_back
			color = texture(texture_menu_back, TexCoord) * vec4(resultantColour, 1.0f);
			break;
		case 21: //texture_menu_furniture
			color = texture(texture_menu_furniture, TexCoord) * vec4(resultantColour, 1.0f);
			break;
		case 22: //texture_menu_wallpaper
			color = texture(texture_menu_wallpaper, TexCoord) * vec4(resultantColour, 1.0f);
			break;
		default:
			color = vec4(resultantColour, 1.0f);
			break;
	}
} 