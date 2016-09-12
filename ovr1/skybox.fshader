#version 440 core

uniform samplerCube texSkyBox;

in vec3 texCoords_o;

out vec4 color;

void main(){
	color=texture(texSkyBox,texCoords_o);
}
