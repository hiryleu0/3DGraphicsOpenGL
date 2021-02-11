#version 330 core
out vec4 FragColor;

in Data
{
	vec4 Position;
	vec3 Normal;
	vec2 TexCoord;
} data;

uniform vec3 camCoords;
uniform float part_of_day;
uniform sampler2D texture_diffuse1;
uniform vec3 s;
uniform mat4 modelT;
uniform mat4 modelTT;

float distance(in vec3 v1, in vec3 v2)
{
	float dist = sqrt((v1.x-v2.x)*(v1.x-v2.x)+(v1.y-v2.y)*(v1.y-v2.y)+(v1.z-v2.z)*(v1.z-v2.z));
	return dist;
}

void main()
{   	
	
	float near = 0.1;
	float far = 100.0;
	
	vec3 lightPos1 = vec3(-18.1,2.5,-8.5);
	vec3 lightPos2 = vec3(modelT*vec4(-4, 0.5, -4,1));
	vec3 lightPos3 = vec3(modelT*vec4(-4, 0.5, -2, 1));
	vec3 reflector_direction = normalize(vec3(transpose(inverse(modelTT)) * vec4(1, 0.3, 0, 1)));
	
	vec3 Pos = data.Position.xyz;
	vec3 Normal = data.Normal;
	vec3 I_O = texture(texture_diffuse1, data.TexCoord).xyz;
	
	
	float dist = distance(camCoords, Pos);
	float dist1 = distance(Pos, lightPos1);
	float dist2 = distance(Pos, lightPos2);
	float dist3 = distance(Pos, lightPos3);
	
	vec3 N;
	N = normalize(Normal);	
	vec3 V = normalize(camCoords - Pos);
	vec3 K = reflector_direction;
	
	int m = 10;
	int a = 10;
	vec3 ambient = vec3(0.1, 0.1, 0.1);
	
	vec3 L1 = normalize(lightPos1 - Pos);
	float NxL1 = max(dot(N,L1), 0.0);
	vec3 R1 = reflect(-L1, N);
	float VxR1 = max(dot(V, R1), 0.0);	
	vec3 I_L1 = (1 /(1+0.1*dist1*dist1))*vec3(1,1,1) ;
	
	vec3 L2 = normalize(lightPos2 - Pos);
	float NxL2 = max(dot(N,L2), 0.0);
	vec3 R2 = reflect(-L2,N);
	float VxR2 = max(dot(V,R2), 0.0);
	vec3 I_L2 = (1 /(1+0.01*dist2*dist2))*vec3(1,1,1);	
	float KxL2 = max(dot(K,L2), 0.0);
	
	vec3 L3 = normalize(lightPos3 - Pos);
	float NxL3 = max(dot(N,L3), 0.0);
	vec3 R3 = reflect(-L3,N);
	float VxR3 = max(dot(V,R3), 0.0);
	vec3 I_L3 = (1 /(1+0.01*dist3*dist3))*vec3(1,1,1);
	float KxL3 = max(dot(K,L3), 0.0);
	
	vec3 col1 =  I_L1 * (NxL1 + pow(VxR1, m));
	vec3 col2 =  I_L2 * (NxL2 + pow(VxR2, m)) * pow(KxL2, a);
	vec3 col3 =  I_L3 * (NxL3 + pow(VxR3, m)) * pow(KxL3, a);
	
	vec3 col = col1 + col2 + col3  + ambient * ( 1 + 8 * part_of_day);
	vec3 noFogColor  = col * I_O;
	vec3 finalColor = ((s-noFogColor)*dist + (noFogColor*far - s*near))/(far-near);
	FragColor = vec4(finalColor, 1.0);
}