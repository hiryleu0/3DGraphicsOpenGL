#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out Data
{
	vec4 Position;
	vec3 Normal;
	vec2 TexCoord;
} data;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;



void main()
{
	data.Normal = mat3(transpose(inverse(model))) * aNormal;
	data.Position = model * vec4(aPos, 1);
	data.TexCoord = aTexCoords;
	
	gl_Position = projection * view * data.Position;
}