#ifndef GLITEMS_H
#define GLITEMS_H

#if defined(_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#define OVR_OS_WIN32
#elif defined(__APPLE__)
#define GLFW_EXPOSE_NATIVE_COCOA
#define GLFW_EXPOSE_NATIVE_NSGL
#define OVR_OS_MAC
#elif defined(__linux__)
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#define OVR_OS_LINUX
#endif

#define MYOVR

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <GLFW\glfw3native.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <gtx\transform.hpp>
#include <regex>
#include <math.h>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <gtc/type_ptr.hpp>
#include <SOIL.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#ifdef MYOVR
#include<Extras/OVR_Math.h>
using namespace OVR;
#endif

#define GET_ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))
#define UFIND_INDEX(x,y) find(x.begin(),x.end(),y)==x.end()

#pragma comment(lib, "SOIL.lib")

using namespace std;
using namespace glm;

static const enum FILE_l{ FILE_NON, FILE_V, FILE_OBJ, FILE_VSHADER, FILE_FSHADER, FILE_PROGRAM, FILE_INDEX_MINUS_ONE, FILE_WITH_UNSORTED_NORMALS, FILE_VESSEL };

class objload{
private:
	struct Vertex{//vertex
		GLfloat v[3];
		Vertex(){}
		Vertex(float xi, float yi, float zi) {
			v[0] = xi, v[1] = yi, v[2] = zi;
		}
	};

	struct FaceIndex{//face index
		GLuint a, b, c;
		GLuint auv, buv, cuv;
		GLuint an, bn, cn;

		FaceIndex(){}

		FaceIndex(GLuint x, GLuint y, GLuint z) :a(x), b(y), c(z){}

		FaceIndex(GLuint x[3]) :a(x[0]-1), b(x[1]-1), c(x[2]-1){}
		
		FaceIndex(GLuint x[3], int) :a(x[0]), b(x[1]), c(x[2]){}//vessel, donnot need minus

		FaceIndex(GLuint x[3], GLuint y[3], GLuint z[3]) :
			a(x[0]), b(x[1]), c(x[2]),
			auv(y[0]), buv(y[1]), cuv(y[2]),
			an(z[0]), bn(z[1]), cn(z[2]){}
	};

	struct Normal{//normal
		float n[3];
		Normal(){}
		Normal(float a, float b, float c){
			n[0] = a, n[1] = b, n[2] = c;
		}
	};

	struct Texture{//Texture
		float u, v;
		Texture(){}
		Texture(float a, float b) :u(a), v(b){}
	};

	vector<Vertex> Vertices;
	vector<FaceIndex> Indices;
	vector<Normal> Normals;
	vector<Texture> Textures;

	GLuint* obj_indexbuffer;
	float* obj_vertexbuffer, *obj_normalbuffer;
	int obj_indexbuffer_size, obj_vertexbuffer_size, obj_normalbuffer_size;

	void load_obj(const char* path,GLuint flag=FILE_NON,GLuint method=FILE_NON);
	GLint load_obj_struct_itr(const char* path, GLuint flag = FILE_V, GLuint method = FILE_NON);

public:

	GLuint creatvao_obj(const char* path, int flag = FILE_V);
	GLuint getElementNum(){ return obj_indexbuffer_size; }

	~objload(){
		Vertices.clear();
		Indices.clear();
		Normals.clear();
		delete[] obj_vertexbuffer;
		delete[] obj_indexbuffer;
		delete[] obj_normalbuffer;
	}
};

//you must only create single one
class GL{
private:
	GLuint vbo, nbo, vshader, fshader, shaderprogram, ebo, colorbuffer, weight, hight, indexNum;
	GLint success;
	GLchar infolog[512];
	vector<vec3> vertexbuffer;
	static GLint xmov, xold, ymov, yold;
	static GLuint vao;//please store this after create
	static GLFWwindow* window;
	static objload Obj;
	static map<int, bool>keystatus;
	static bool isFullScreen, isMouseMoveAllow;
	static vec3 cameraPos, cameraFront, cameraUp;
	void do_movement(GLfloat);
	static int drop_file(string s);

public:

	GL(){}

	GL(int w, int h){ init(w, h); }

	int init(int, int);
	void render(GLuint, GLfloat, GLuint vao_load = 0);
	void creatProgram(char* vs, char* fs);
	GLuint creatVao(char* path, int flag = FILE_NON);
	static GLuint load_shader(const char* path, const char* path2);

	GLuint get_vao(){ return vao; }

	GLuint get_program(){ return shaderprogram; }//just use usProgram()

	void useProgram(){ glUseProgram(shaderprogram); }

	GLFWwindow* getWindow(){ return window; }

	mat4 getViewMatrix(){ return lookAtRH(cameraPos, cameraPos + cameraFront, cameraUp); }

	GLuint getElementNum(){ return indexNum; }

	void setCameraPos(vec3 pos){ cameraPos = pos; }

	vec3 getCameraPos(){ return cameraPos; }

#ifdef MYOVR
	static vec3 keyShift(GLfloat, Vector3f&, Vector3f&, Vector3f&,Vector3f&);
#endif
};


typedef unsigned int _L_U_INT;

struct _L_Vertex{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct _L_Texture{
	_L_U_INT id;
	string type;
	aiString path;
};

class Mesh{
private:
	vector<_L_Vertex>	vertices;
	vector<_L_Texture>	textures;
	vector<_L_U_INT>	indices;

	_L_U_INT VAO, VBO, EBO;

	void setupMesh();

public:
	Mesh(){}
	Mesh::Mesh(vector<_L_Vertex> _ve, vector<_L_Texture>_te, vector<_L_U_INT>_in){
		this->vertices = _ve;
		this->textures = _te;
		this->indices = _in;

		setupMesh();
	}
	~Mesh(){
		this->indices.clear();
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
	_L_U_INT TextureFromFile(const char*, string);

public:
	A_model(char* _path){
		load_model(_path);
	}
};

#endif//GLITEMS_H