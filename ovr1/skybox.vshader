#version 440 core

layout(location = 0) in vec3 vertices;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 texCoords_o;

void main(){

	gl_Position=(projectionMatrix*mat4(mat3(viewMatrix))*modelMatrix*vec4(vertices,1)).xyww;
	texCoords_o=vertices;
}