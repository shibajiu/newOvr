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

#define GET_ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))
#define UFIND_INDEX(x,y) find(x.begin(),x.end(),y)==x.end()

#pragma comment(lib, "SOIL.lib")

using namespace std;
using namespace glm;

static const enum FILE_l{ FILE_NON, FILE_V, FILE_OBJ, FILE_VSHADER, FILE_FSHADER, FILE_PROGRAM };

class objload{
private:
	struct Vertex{//vertex
		float x, y, z;
		Vertex(){}
		Vertex(float xi, float yi, float zi) :x(xi), y(yi), z(zi){}
	};

	struct FaceIndex{//face index
		GLuint a, b, c;
		GLuint auv, buv, cuv;
		GLuint an, bn, cn;

		FaceIndex(){}
		FaceIndex(GLuint x, GLuint y, GLuint z) :a(x - 1), b(y - 1), c(z - 1){}
		FaceIndex(GLuint x[3]) :a(x[0] - 1), b(x[1] - 1), c(x[2] - 1){}
		FaceIndex(GLuint x[3], GLuint y[3], GLuint z[3]) :
			a(x[0]), b(x[1]), c(x[2]),
			auv(y[0]), buv(y[1]), cuv(y[2]),
			an(z[0]), bn(z[1]), cn(z[2]){}
	};

	struct Normal{//normal
		float x, y, z;
		Normal(){}
		Normal(float a, float b, float c) :x(a), y(b), z(c){}
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

	void load_obj(const char* path);
	GLint load_obj_struct_itr(const char* path, int flag = FILE_V);

public:

	GLuint creatvao_obj(const char* path, int flag = FILE_V);

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
	GLuint vbo, nbo, vshader, fshader, shaderprogram, ebo, colorbuffer, weight, hight;
	GLint success;
	GLchar infolog[512];
	vector<vec3> vertexbuffer;
	static GLint xmov, xold;
	static GLuint vao;//please store this after create
	static GLFWwindow* window;
	static objload Obj;
	static map<int, bool>keystatus;
	static bool isFullScreen;
	static vec3 cameraPos, cameraFront, cameraUp;

	static int drop_file(string s);
	void do_movement(GLfloat);

public:

	//only use with specific shader
	/*GL(
		GLfloat* g_vertex_buffer_data_cube,
		size_t vertex_size,
		GLuint* indices,
		size_t index_size,
		GLfloat* g_color_buffer_data,
		size_t color_size,
		char* vs,
		char* fs){

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glEnableVertexAttribArray(0);
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertex_size, g_vertex_buffer_data_cube, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, indices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);
		glGenBuffers(1, &colorbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glBufferData(GL_ARRAY_BUFFER, color_size, g_color_buffer_data, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

		//glEnableVertexAttribArray(2);
		//glGenBuffers(GL_ARRAY_BUFFER, &nbo);
		//glBindBuffer(GL_ARRAY_BUFFER, nbo);
		//glBufferData(GL_ARRAY_BUFFER,sizof)

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		creatProgram(vs, fs);
		}

		GL(
		GLfloat v_n[],
		size_t vertex_size,
		char* vs,
		char* fs){

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glEnableVertexAttribArray(0);
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertex_size, v_n, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		creatProgram(vs, fs);
		}

		void display(mat4 VPmatrix, mat4 modelrotate, vec3 eye){
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, 800, 600);
		glBindVertexArray(vao);
		glUseProgram(shaderprogram);
		glProgramUniformMatrix4fv(shaderprogram, glGetUniformLocation(shaderprogram, "mvp"), 1, GL_FALSE, (float*)&VPmatrix);//load mvp
		glProgramUniformMatrix4fv(shaderprogram, glGetUniformLocation(shaderprogram, "model"), 1, GL_FALSE, (float*)&modelrotate);
		glProgramUniform3f(shaderprogram, glGetUniformLocation(shaderprogram, "lightDir"), 1, 0, 0);
		glProgramUniform3f(shaderprogram, glGetUniformLocation(shaderprogram, "viewPos"), eye.x, eye.y, eye.z);
		glProgramUniform3f(shaderprogram, glGetUniformLocation(shaderprogram, "lightcolor"), 1, 1, 1);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glUseProgram(0);
		}

		void display(mat4 VPmatrix){
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, 800, 600);
		glBindVertexArray(vao);
		glUseProgram(shaderprogram);
		glProgramUniformMatrix4fv(shaderprogram, glGetUniformLocation(shaderprogram, "mvp"), 1, GL_FALSE, (float*)&VPmatrix);//load mvp

		glDrawArrays(GL_TRIANGLES, 0, 36);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glUseProgram(0);
		}

		void display(mat4 model, mat4 view, mat4 projection){
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, 800, 600);
		glUseProgram(shaderprogram);
		glProgramUniformMatrix4fv(shaderprogram, glGetUniformLocation(shaderprogram, "model"), 1, GL_FALSE, (float*)&model);//load mvp
		glProgramUniformMatrix4fv(shaderprogram, glGetUniformLocation(shaderprogram, "view"), 1, GL_FALSE, (float*)&view);//load mvp
		glProgramUniformMatrix4fv(shaderprogram, glGetUniformLocation(shaderprogram, "projection"), 1, GL_FALSE, (float*)&projection);//load mvp

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glUseProgram(0);
		}*/

	GL(){}

	GL(int w, int h){ init(w, h); }

	int init(int, int);
	void render(GLuint, GLfloat);
	void creatProgram(char* vs, char* fs);
	static GLuint load_shader(const char* path, const char* path2);

	GLuint get_vao(){ return vao; }

	GLuint get_program(){ return shaderprogram; }

	GLFWwindow* getWindow(){ return window; }

	mat4 getViewMatrix(){ return lookAtRH(cameraPos, cameraPos + cameraFront, cameraUp); }

};

#endif//GLITEMS_H