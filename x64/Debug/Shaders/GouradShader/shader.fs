#version 330 core
out vec4 FragColor;

in Data
{
	vec3 FrontColor;
} data;


void main()
{   	
	FragColor = vec4(data.FrontColor, 1);
}