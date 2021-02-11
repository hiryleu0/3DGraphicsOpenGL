#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 FragNormal;
out float dist;
out vec3 cam_pos;
out vec3 lightPos1;
out vec3 lightPos2;
out vec3 lightPos3;
out vec3 reflector_direction;
out float dist1;
out float dist2;
out float dist3;

uniform mat4 model;
uniform mat4 modelT;
uniform mat4 modelTT;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 camCoords;

float distance(in vec3 v1, in vec3 v2)
{
	float dist = sqrt((v1.x-v2.x)*(v1.x-v2.x)+(v1.y-v2.y)*(v1.y-v2.y)+(v1.z-v2.z)*(v1.z-v2.z));
	return dist;
}

void main()
{
    TexCoords = aTexCoords; 
	FragPos = vec3(model * vec4(aPos,1.0));
	FragNormal = mat3(transpose(inverse(model))) * aNormal;
	dist = distance(camCoords, FragPos);
	cam_pos = camCoords;
	lightPos1 = vec3(-18.1,2.5,-8.5);
	lightPos2 = vec3(modelT*vec4(-4, 0.5, -4,1));
	lightPos3 = vec3(modelT*vec4(-4, 0.5, -2, 1));
	reflector_direction = normalize(vec3(transpose(inverse(modelTT)) * vec4(1, 0.3, 0, 1)));
	dist1 = distance(FragPos, lightPos1);
	dist2 = distance(FragPos, lightPos2);
	dist3 = distance(FragPos, lightPos3);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}