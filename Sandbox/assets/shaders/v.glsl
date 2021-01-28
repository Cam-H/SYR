#shader vertex

#version 330 core
layout(location=0) in vec3 position;

out vec4 Color;

void main() {
    Color = vec4(1.0, 1.0, 1.0, 1.0);
    gl_Position = vec4(position, 1.0);
};
