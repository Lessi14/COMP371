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
uniform sampler2D texture_menu_bed;
uniform sampler2D texture_menu_cabinet;
uniform sampler2D texture_menu_coffee_table;
uniform sampler2D texture_menu_toilet;
uniform sampler2D texture_menu_torchere;
uniform sampler2D texture_menu_wall_item;
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
		//DIFFUSE TEXTURES
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
		case 4: //red
			color = vec4(1,0,0,1) * vec4(resultantColour, 1.0f);
			break;
		case 5: //blue
			color = vec4(0, 1, 0, 1) * vec4(resultantColour, 1.0f);
			break;
		case 6: //green
			color = vec4(0, 0, 1, 1) * vec4(resultantColour, 1.0f);
			break;
		case 7: //yellow
			color = vec4(1, 1, 0, 1) * vec4(resultantColour, 1.0f);
			break;


		//NON-DIFFUSE TEXTURES
        case 14: //texture_menu_bed
			color = texture(texture_menu_bed, TexCoord);
			break;
        case 15: //texture_menu_cabinet
			color = texture(texture_menu_cabinet, TexCoord);
			break;
        case 16: //texture_menu_coffee_table
			color = texture(texture_menu_coffee_table, TexCoord);
			break;
        case 17: //texture_menu_toilet
			color = texture(texture_menu_toilet, TexCoord);
			break;
        case 18: //texture_menu_torchere
			color = texture(texture_menu_torchere, TexCoord);
			break;
        case 19: //texture_menu_wall_item
			color = texture(texture_menu_wall_item, TexCoord);
			break;
        case 20: //texture_menu_back
			color = texture(texture_menu_back, TexCoord);
			break;
		case 21: //texture_menu_furniture
			color = texture(texture_menu_furniture, TexCoord);
			break;
		case 22: //texture_menu_wallpaper
			color = texture(texture_menu_wallpaper, TexCoord);
			break;

		//NON-DIFFUSE VERSIONS OF DIFFUSE TEXTURES
		case 23: //metal1 texture
			color = texture(texture0, TexCoord);
			break;
		case 24: //metal2 texture
			color = texture(texture1, TexCoord);
			break;
		case 25: //wood1 texture
			color = texture(texture2, TexCoord);
			break;
		case 26: //wood2 texture
			color = texture(texture3, TexCoord);
			break;
		case 27: //red
			color = vec4(1, 0, 0, 1);
			break;
		case 28: //blue
			color = vec4(0, 1, 0, 1);
			break;
		case 29: //green
			color = vec4(0, 0, 1, 1);
			break;
		case 30: //yellow
			color = vec4(1, 1, 0, 1);
			break;
		default:
			color = vec4(resultantColour, 1.0f);
			break;
	}
} 