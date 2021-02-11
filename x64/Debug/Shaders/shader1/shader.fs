#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 FragNormal;
in float dist;
in vec3 cam_pos;
in vec3 lightPos1;
in vec3 lightPos2;
in vec3 lightPos3;
in vec3 reflector_direction;
in float dist1;
in float dist2;
in float dist3;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform float part_of_day;
uniform vec3 s;

void main()
{    
	vec3 I_O = texture(texture_diffuse1, TexCoords).xyz;
	vec3 N;
	if(FragColor.x ==0 && FragColor.y ==0 && FragColor.z ==0)
		N = normalize(texture(texture_specular1, TexCoords).xyz);
	else
		N = normalize(FragNormal);	
	vec3 V = normalize(cam_pos - FragPos);
	vec3 K = reflector_direction;
	
	int m = 20;
	int a = 5;
	vec3 ambient = vec3(0.1, 0.1, 0.1) + part_of_day * 0.9;
	
	vec3 L1 = normalize(lightPos1 - FragPos);
	float NxL1 = max(dot(N,L1), 0.0);
	vec3 R1 = reflect(-L1, N);
	float VxR1 = max(dot(V, R1), 0.0);	
	vec3 I_L1 = (1 /(1+0.1*dist1*dist1))*vec3(1,1,1) ;
	
	vec3 L2 = normalize(lightPos2 - FragPos);
	float NxL2 = max(dot(N,L2), 0.0);
	vec3 R2 = reflect(-L2,N);
	float VxR2 = max(dot(V,R2), 0.0);
	vec3 I_L2 = (1 /(1+0.01*dist2*dist2))*vec3(1,1,1);	
	float KxL2 = max(dot(K,L2), 0.0);
	
	vec3 L3 = normalize(lightPos3 - FragPos);
	float NxL3 = max(dot(N,L3), 0.0);
	vec3 R3 = reflect(-L3,N);
	float VxR3 = max(dot(V,R3), 0.0);
	vec3 I_L3 = (1 /(1+0.01*dist3*dist3))*vec3(1,1,1);
	float KxL3 = max(dot(K,L3), 0.0);
	
	vec3 amb_col = I_O * ambient;
	vec3 col1 = I_O * I_L1 * (NxL1 + pow(VxR1, m));
	vec3 col2 = I_O * I_L2 * (NxL2 + pow(VxR2, m)) * pow(KxL2, a);
	vec3 col3 = I_O * I_L3 * (NxL3 + pow(VxR3, m)) * pow(KxL3, a);
	
	vec3 col = col1 + col2 + col3 + amb_col;
	
	vec3 noFogColor = col;
	
	float near = 0.1;
	float far = 100.0;
	vec3 finalColor = ((s-noFogColor)*dist + (noFogColor*far - s*near))/(far-near);
	FragColor = vec4(finalColor,1.0);
}