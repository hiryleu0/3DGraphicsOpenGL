#version 330 core
out vec4 FragColor;

flat in vec3 FrontColor;

void main()
{   	
	FragColor = vec4(FrontColor, 1);
}