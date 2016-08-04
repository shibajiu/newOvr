#version 440 core

layout(location = 0) in vec3 vertices;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 texCoords_o;

void main(){

	gl_Position=(projection*mat4(mat3(view))*model*vec4(vertices,1)).xyww;
	texCoords_o=vertices;
}