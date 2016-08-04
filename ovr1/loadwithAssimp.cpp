#include "stdafx.h"
#include "loadwithAssimp.h"

Mesh::Mesh(vector<_L_Vertex>_ve, vector<_L_Texture>_te, vector<_L_U_INT>_in){
	this->vertices = _ve;
	this->textures = _te;
	this->Indices = _in;
}

void A_model::load_model(string _path){
	Assimp::Importer importer;
	auto scene = importer.ReadFile(_path, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
		cerr << "ERR::ASSIMP::" << importer.GetErrorString() << endl;
		return;
	}
	this->directory = _path.substr(0, _path.find_last_of('/'));
	this->processNode(scene->mRootNode, scene);
}

void A_model::processNode(aiNode* _node, const aiScene* _scene){
	for (int i = 0; i < _node->mNumMeshes; ++i){
		auto _aimesh = _scene->mMeshes[_node->mMeshes[i]];
		this->meshes.push_back(processMesh(_aimesh,_scene));
	}
	for (int i = 0; i < _node->mNumChildren; ++i){
		this->processNode(_node->mChildren[i], _scene);
	}
}

Mesh A_model::processMesh(aiMesh* _aimesh, const aiScene* _scene){
	vector<_L_Vertex>	_vertices;
	vector<_L_Texture>	_textures;
	vector<_L_U_INT>	_indices;

	for (int i = 0; i < _aimesh->mNumVertices; ++i){
		glm::vec3 _v;
		_v.x = _aimesh->mVertices[i].x;
		_v.y = _aimesh->mVertices[i].y;
		_v.z = _aimesh->mVertices[i].z;
		_vertices[i].Position = _v;
		_v.x = _aimesh->mNormals[i].x;
		_v.y = _aimesh->mNormals[i].y;
		_v.z = _aimesh->mNormals[i].z;
		_vertices[i].Normal = _v;
		if (_aimesh->mTextureCoords[0]){
			glm::vec2 _v;
			_v.x = _aimesh->mTextureCoords[0][i].x;
			_v.y = _aimesh->mTextureCoords[0][i].y;
			_vertices[i].TexCoords = _v;
		}
		else
			_vertices[i].TexCoords = vec2(0, 0);
	}
	for (int i = 0; i < _aimesh->mNumFaces; ++i){
		auto face = _aimesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; ++j)
			_indices.push_back(face.mIndices[j]);
	}

	if (_aimesh->mMaterialIndex >= 0){
		auto _material = _scene->mMaterials[_aimesh->mMaterialIndex];
		vector<_L_Texture> _t_d = this->loadMaterialTextures(_material, aiTextureType_DIFFUSE, "TextureType_DIFFUSE");
		_textures.insert(_textures.end(), _t_d.begin(), _t_d.end());
		vector<_L_Texture> _t_s = this->loadMaterialTextures(_material, aiTextureType_SPECULAR, "TextureType_SPECULAR");
		_textures.insert(_textures.end(), _t_s.begin(), _t_s.end());
	}

	return Mesh(_vertices, _textures, _indices);
}

vector<_L_Texture> A_model::loadMaterialTextures(aiMaterial* _mat, aiTextureType _type, string _typeName){
	vector<_L_Texture> _textures;
	bool skip = false;
	for (int i = 0; i < _mat->GetTextureCount(_type); ++i){
		aiString _s;
		_mat->GetTexture(_type, i, &_s);
		for (int j = 0; j < _loaded.size(); ++j){
			if (_loaded[j].path == _s){
				_textures.push_back(_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip){
			_L_Texture _t;
			_t.type = _typeName;
			_t.path = _s;
			_t.id = this->TextureFromFile(_s.C_Str(), this->directory);
			_textures.push_back(_t);
			_loaded.push_back(_t);
		}
	}
	
	return _textures;
}

_L_U_INT A_model::TextureFromFile(const char* _s, string _p){
	return 0;
}