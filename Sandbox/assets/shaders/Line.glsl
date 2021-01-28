#type vertex
#version 330 core
			
layout(location=0) in vec3 position;
layout(location=1) in vec4 color;

uniform mat4 u_ViewProjection;

out vec4 Color;

void main(){
	gl_Position = u_ViewProjection * vec4(position, 1.0);

	Color = color;
}

#type fragment
#version 330 core

in vec4 Color;

out vec4 outColor;

void main(){
	outColor = Color;
}