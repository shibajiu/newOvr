#ifndef LOAD_WITH_ASSIMP_
#define LOAD_WITH_ASSIMP_
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm.hpp>
#include <string>
#include <iostream>
#include <vector>

typedef unsigned int _L_U_INT;

struct _L_Vertex{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct _L_Texture{
	unsigned short	id;
	string			type;
	aiString		path;
};

class Mesh{
private:
	vector<_L_Vertex>	vertices;
	vector<_L_Texture>	textures;
	vector<_L_U_INT>	Indices;

public:
	Mesh(){}
	Mesh::Mesh(vector<_L_Vertex>, vector<_L_Texture>, vector<_L_U_INT>);
	~Mesh(){
		this->Indices.clear();
		this->textures.clear();
		this->vertices.clear();
	}

};

class A_model{
private:
	vector<Mesh> meshes;
	vector<_L_Texture>_loaded;
	string directory;

	void load_model(string);
	void processNode(aiNode*, const aiScene*);
	Mesh processMesh(aiMesh*, const aiScene*);
	vector<_L_Texture> loadMaterialTextures(aiMaterial*, aiTextureType, string);
	_L_U_INT TextureFromFile(const char*,string);

public:
	A_model(char* _path){
		load_model(_path);
	}
};

#endif // !LOAD_WITH_ASSIMP_
