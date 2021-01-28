#type fragment
#version 330 core
			
in vec2 Texcoord;

uniform sampler2D u_Texture;

out vec4 outColor;

void main(){
	outColor = texture(u_Texture, Texcoord);
}