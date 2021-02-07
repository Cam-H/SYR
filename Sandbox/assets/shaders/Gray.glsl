#type vertex
#version 330 core

#define MAX_MATERIALS 16

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shine;
};
  
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in float material;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;
uniform mat3 u_NormalMatrix;
uniform vec3 u_ViewPosition;
uniform Material u_Materials[MAX_MATERIALS];

out vec3 Normal;

out vec3 FragPos;
out vec3 ViewPos;
flat out Material FragMat;

void main(){
	gl_Position = u_ViewProjection * u_Transform * vec4(position, 1.0);

	Normal = u_NormalMatrix * normal;

	FragPos = vec3(u_Transform * vec4(position, 1.0));
	ViewPos = u_ViewPosition;
	
	FragMat = u_Materials[int(material)];
}

#type fragment
#version 330 core

#define MAX_LIGHTS 8

struct Light {
	vec3 color;

	vec3 position;
	vec3 direction;

	float linearAttenuation;
	float quadraticAttenuation;

	float innerCutoff;
	float outerCutoff;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shine;
}; 
  
uniform Light u_Lights[MAX_LIGHTS];
uniform int u_ActiveLightCount;
uniform vec3 u_AmbientLight;

in vec3 Normal;

in vec3 FragPos;
in vec3 ViewPos;
flat in Material FragMat;

out vec4 outColor;

vec3 CalculateLightContribution(Light light, Material material, vec3 normal, vec3 fragPos, vec3 viewPos);

void main(){
	vec3 normal = normalize(Normal);

	vec3 result = vec3(0, 0, 0);
	for(int i = 0; i < u_ActiveLightCount; i++) {
		result += CalculateLightContribution(u_Lights[i], FragMat, normal, FragPos, ViewPos);
	}

	outColor = vec4(result + u_AmbientLight * FragMat.ambient, 1.0);
}

vec3 CalculateLightContribution(Light light, Material material, vec3 normal, vec3 fragPos, vec3 viewPos){
	
	vec3 lightDirection = normalize(light.position - fragPos);
	//vec3 lightDirection = normalize(vec3(1, 10, 0) - fragPos);

	vec3 viewDirection = normalize(viewPos - fragPos);
	vec3 reflectDirection = reflect(-lightDirection, normal);

	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (1 + (light.linearAttenuation + light.quadraticAttenuation * distance) * distance);


	float diff = max(dot(normal, lightDirection), 0.0);
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shine);

	vec3 diffuse = material.diffuse * diff * light.color;
	vec3 specular = material.specular * spec * light.color;


	float theta = dot(lightDirection, -light.direction);
	float epsilon = light.innerCutoff - light.outerCutoff;
	float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

	return (diffuse + specular) * intensity * attenuation;
}