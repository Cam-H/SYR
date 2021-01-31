#type vertex
#version 330 core
			
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;
uniform mat3 u_NormalMatrix;
uniform vec3 u_ViewPosition;

out vec4 Color;
out vec3 Normal;

out vec3 FragPos;
out vec3 ViewPos;

void main(){
	gl_Position = u_ViewProjection * u_Transform * vec4(position, 1.0);

	Color = vec4(0.8, 0.8, 0.8, 1.0);
	Normal = u_NormalMatrix * normal;

	FragPos = vec3(u_Transform * vec4(position, 1.0));
	ViewPos = u_ViewPosition;
}

#type fragment
#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
  
uniform Material material;

in vec4 Color;
in vec3 Normal;

in vec3 FragPos;
in vec3 ViewPos;

out vec4 outColor;

void main(){
	
	vec3 lightColor = vec3(0.5, 1, 1);

	vec3 ambient = lightColor * material.ambient;

	float specularStrength = 0.5;

	vec3 lightPos = vec3(1.0, 10.0, 0.0);
	vec3 lightDirection = normalize(lightPos - FragPos);

	vec3 viewDirection = normalize(ViewPos - FragPos);
	vec3 reflectDirection = reflect(-lightDirection, Normal);



	float diff = max(dot(Normal, lightDirection), 0.0);
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 32);

	vec3 diffuse = diff * lightColor;
	vec3 specular = specularStrength * spec * lightColor;

	outColor = Color * vec4(ambient + diffuse + specular, 1.0);
}