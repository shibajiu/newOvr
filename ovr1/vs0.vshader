#version 440 core

layout(location = 0) in vec3 vertices;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main(){
gl_Position=projectionMatrix*viewMatrix*modelMatrix*vec4(vertices,1);
}