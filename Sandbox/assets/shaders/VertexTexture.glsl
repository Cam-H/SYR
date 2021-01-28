#type vertex
#version 330 core
			
layout(location=0) in vec3 position;
layout(location=1) in vec2 texcoord;
			
uniform mat4 viewProjection;
uniform mat4 transform;

out vec2 Texcoord;

void main(){
	gl_Position = viewProjection * transform * vec4(position, 1.0);
	Texcoord = texcoord;
}