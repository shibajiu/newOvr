#version 440 core

struct Material{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shiness;
};

struct Light{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec4 position;
};

uniform Material vessel;
uniform Light light;
uniform vec3 eyePos;

uniform samplerCube texSkyBox;

in vec3 normal_o;
in vec3 fragpos;

out vec4 color;

void main(){
	float diffuseStrength;
	vec3 diffuseColor,specularColor;
	vec3 c=normalize(normal_o);

	//ambient color
	vec3 ambientColor=light.ambient*vessel.ambient;

	//diffuse color
	//light is a point
	vec3 lightDir=normalize(light.position.xyz-fragpos);
	if(light.position.w==1.0){
		diffuseStrength=max(dot(lightDir,c),0);
		diffuseColor=light.diffuse*vessel.diffuse*diffuseStrength;
	}
	//light is a direction
	if(light.position.w==0.0){
		diffuseStrength=max(dot(light.position.xyz,c),0);
		diffuseColor=light.diffuse*vessel.diffuse*diffuseStrength;		
	}

	//specular color
	vec3 viewDir=normalize(eyePos-fragpos);
	vec3 halfDir=normalize(lightDir+viewDir);
	//vec3 reflectDir=reflect(-lightDir,c);
	float specularStrength=pow(max(dot(halfDir,viewDir),0),vessel.shiness);
	specularColor=vessel.specular*light.specular*specularStrength;

	vec3 result=ambientColor+diffuseColor+specularColor;

	//ambient reflect
	vec3 are=reflect(-viewDir,c);
	color=texture(texSkyBox,are);

	//Refraction
	float ratio=1.f/2.42;
	vec3 ref=refract(-viewDir,c,ratio);
	color=texture(texSkyBox,ref);

	//color=vec4(1,0,0,1);
	//color=vec4(c,1);
	//color=vec4(result,1);
}