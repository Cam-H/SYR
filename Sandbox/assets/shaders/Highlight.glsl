#type vertex
#version 330 core

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

void main(){
	gl_Position = u_ViewProjection * u_Transform * vec4(position, 1.0);
}

#type fragment
#version 330 core

out vec4 outColor;

void main(){
	outColor = vec4(0.04, 0.28, 0.26, 1.0);
}