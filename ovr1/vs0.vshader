#version 440 core

layout(location = 0) in vec3 vertices;
layout(location = 1) in vec3 normals;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 normal_o;
out vec3 fragpos;
out vec3 view_normal;

void main(){
	view_normal=vec3(viewMatrix*vec4(normals,0));
	normal_o=mat3(modelMatrix)*normals;
	fragpos=mat3(modelMatrix)*vertices;
	gl_Position=projectionMatrix*viewMatrix*modelMatrix*vec4(vertices,1);
}