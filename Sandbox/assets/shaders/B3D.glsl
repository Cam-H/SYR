#type vertex
#version 330 core
			
layout(location=0) in vec3 position;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec4 Color;

void main(){
	gl_Position = u_ViewProjection * u_Transform * vec4(position, 1.0);

	Color = vec4(position, 1.0);
}

#type fragment
#version 330 core

in vec4 Color;

out vec4 outColor;

void main(){
	outColor = Color;
}