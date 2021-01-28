#type vertex
#version 330 core
			
layout(location=0) in vec3 position;
layout(location=1) in vec4 color;
layout(location=2) in vec2 texcoord;
layout(location=3) in float texIndex;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec4 Color;
out vec2 Texcoord;
out float TexIndex;

void main(){
	//gl_Position = u_ViewProjection * u_Transform * vec4(position, 1.0);
	gl_Position = u_ViewProjection * vec4(position, 1.0);

	Color = color;
	Texcoord = texcoord;
	TexIndex = texIndex;
}

#type fragment
#version 330 core

in vec4 Color;
in vec2 Texcoord;
in float TexIndex;

uniform sampler2D u_Textures[32];

out vec4 outColor;

void main(){
	outColor = texture(u_Textures[int(TexIndex)], Texcoord) * Color;
}