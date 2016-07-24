#include "glitems.h"
#include "stdafx.h"

void objload::load_obj(const char* path){
	ifstream obj(path);
	if (!obj.is_open()){
		fprintf_s(stderr, "cannot open file\n");
		return;
	}
	char buffer[128];
	float f1, f2, f3;
	GLuint i0[3], i1[3], i2[3];
	while (!obj.eof()){
		obj.getline(buffer, 128, '\n');
		if (buffer[0] == 'v' && (buffer[1] == ' ' || buffer[1] == 32)){
			if (sscanf_s(buffer, "v %f %f %f", &f1, &f2, &f3) == 3){
				Vertices.push_back(Vertex(f1, f2, f3));
			}
			else{
				fprintf(stderr, "ERROR: vertex not in wanted format in OBJLoader\n");
			}
		}
		else if (buffer[0] == 'f' && (buffer[1] == ' ' || buffer[1] == 32)){
			if (sscanf_s(buffer, "f %d %d %d", &i0[0], &i0[1], &i0[2]) == 3){
				Indices.push_back(FaceIndex(i0));
			}
			else if (sscanf_s(buffer, "f %d/%d/%d %d/%d/%d %d/%d/%d",
				&i0[0], &i1[0], &i2[0],
				&i0[1], &i1[1], &i2[1],
				&i0[2], &i1[2], &i2[2]) == 9){
				Indices.push_back(FaceIndex(i0, i1, i2));
			}
			else{
				fprintf(stderr, "ERROR: FaceIndex not in wanted format in OBJLoader\n");
			}
		}
		else if (buffer[0] == 'n' && (buffer[1] == ' ' || buffer[1] == 32)){
			if (sscanf_s(buffer, "n %f %f %f", &f1, &f2, &f3) == 3){
				Normals.push_back(Normal(f1, f2, f3));
			}
			else{
				fprintf(stderr, "ERROR: Normal not in wanted format in OBJLoader\n");
			}
		}
		else if ((buffer[0] == 'v' && buffer[1] == 'n') && (buffer[2] == ' ' || buffer[2] == 32)){
			if (sscanf_s(buffer, "vn %f %f %f", &f1, &f2, &f3) == 3){
				Normals.push_back(Normal(f1, f2, f3));
			}
			else{
				fprintf(stderr, "ERROR: Normal not in wanted format in OBJLoader\n");
			}
		}
		else if ((buffer[0] == 'v' && buffer[1] == 't') && (buffer[2] == ' ' || buffer[2] == 32)){
			if (sscanf_s(buffer, "vt %f %f", &f1, &f2) == 2){
				Textures.push_back(Texture(f1, f2));
			}
			else{
				fprintf(stderr, "ERROR: Texture not in wanted format in OBJLoader\n");
			}
		}
	}
	obj.close();
}

GLint objload::load_obj_struct_itr(const char* path, int flag){

	load_obj(path);
	int iv = 0;
	printf_s("Loading obj file...\n");
	if (!Vertices.size() || !Indices.size())
		goto jump1;

	if (flag == FILE_NON){
		obj_vertexbuffer_size = Vertices.size() * 3;
		obj_vertexbuffer = new float[obj_vertexbuffer_size];
		for (vector<Vertex>::const_iterator ite = Vertices.begin(); ite != Vertices.end(); ++ite){
			obj_vertexbuffer[iv * 3] = ite->x;
			obj_vertexbuffer[iv * 3 + 1] = ite->y;
			obj_vertexbuffer[iv * 3 + 2] = ite->z;
			iv++;
		}
		printf_s("vertexbuffer loaded\n");

		obj_normalbuffer_size = Normals.size() * 3;
		obj_normalbuffer = new float[obj_normalbuffer_size];
		iv = 0;
		for (vector<Normal>::const_iterator ite = Normals.begin(); ite != Normals.end(); ++ite){
			obj_normalbuffer[iv * 3] = ite->x;
			obj_normalbuffer[iv * 3 + 1] = ite->y;
			obj_normalbuffer[iv * 3 + 2] = ite->z;
			iv++;
		}
		printf_s("normalbuffer loaded\n");
		printf_s("obj file loaded\n");
	}

	iv = 0;
	obj_indexbuffer_size = Indices.size() * 3;
	obj_indexbuffer = new GLuint[obj_indexbuffer_size];
	for (vector<FaceIndex>::const_iterator ite = Indices.begin(); ite != Indices.end(); ++ite){
		obj_indexbuffer[iv * 3] = ite->a;
		obj_indexbuffer[iv * 3 + 1] = ite->b;
		obj_indexbuffer[iv * 3 + 2] = ite->c;
		iv++;
	}
jump1:
	printf_s("indexbuffer loaded: vertices:%d,normals:%d,faces:%d\n", Vertices.size(), Normals.size(), Indices.size());
	return (!Vertices.size() || !Indices.size());

}

GLuint objload::creatvao_obj(const char* path, int flag){
	GLuint vao_l, vbo_l, ebo_l, nbo_l;
	if (load_obj_struct_itr(path, flag)){
		fprintf_s(stderr, "obj err\n");
		return 0;
	}

	if (flag == FILE_NON){

		glGenVertexArrays(1, &vao_l);
		glBindVertexArray(vao_l);

		glEnableVertexAttribArray(0);
		glGenBuffers(1, &vbo_l);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_l);
		glBufferData(GL_ARRAY_BUFFER, sizeof(obj_vertexbuffer), obj_vertexbuffer, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), NULL);

		glGenBuffers(1, &ebo_l);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_l);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(obj_indexbuffer), obj_indexbuffer, GL_STATIC_DRAW);

		glBindVertexArray(0);
	}
	else if (flag == FILE_V){

		glGenVertexArrays(1, &vao_l);
		glBindVertexArray(vao_l);

		glEnableVertexAttribArray(0);
		glGenBuffers(1, &vbo_l);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_l);
		glBufferData(GL_ARRAY_BUFFER, Vertices.size()* sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, x));

		glGenBuffers(1, &ebo_l);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_l);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(obj_indexbuffer), obj_indexbuffer, GL_STATIC_DRAW);

		printf_s("sorting normals....");
		vector<Normal>sequentialNormals(Vertices.size());
		vector<GLushort> temp_index;
		for (auto ite = Indices.begin(); ite != Indices.end(); ++ite){
			if (UFIND_INDEX(temp_index, ite->a)){
				sequentialNormals[ite->a - 1] = Normals[ite->an - 1];
				temp_index.push_back(ite->a);
			}
			if (UFIND_INDEX(temp_index, ite->b)){
				sequentialNormals[ite->b - 1] = Normals[ite->bn - 1];
				temp_index.push_back(ite->b);
			}
			if (UFIND_INDEX(temp_index, ite->c)){
				sequentialNormals[ite->c - 1] = Normals[ite->cn - 1];
				temp_index.push_back(ite->c);
			}

			//cout << temp_index.size() << "	";
			if (temp_index.size() == Vertices.size()){
				printf_s("complete\n");
				break;
			}
		}

		glEnableVertexAttribArray(1);
		glGenBuffers(1, &nbo_l);
		glBindBuffer(GL_ARRAY_BUFFER, nbo_l);
		glBufferData(GL_ARRAY_BUFFER, sequentialNormals.size()* sizeof(Normal), &sequentialNormals[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Normal), (void *)offsetof(Normal, x));

		glBindVertexArray(0);

		vector<Normal>().swap(sequentialNormals);
		vector<GLushort>().swap(temp_index);
	}
	return vao_l;
}

//initialize static variable
bool GL::isFullScreen = false;
GLint GL::xmov = 0, GL::xold = 0;
objload GL::Obj = objload();
vec3 GL::cameraFront = vec3(0, 0, -1), GL::cameraPos = vec3(0, 0, -7), GL::cameraUp = vec3(0, 1, 0);
GLFWwindow* GL::window = nullptr;
map<int, bool> GL::keystatus = map<int, bool>();
GLuint GL::vao = NULL;

void GL::creatProgram(char* vs, char* fs){
	vshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vshader, 1, &vs, NULL);
	glCompileShader(vshader);
	glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
	if (!success){
		glGetShaderInfoLog(vshader, 512, NULL, infolog);
		fprintf_s(stderr, "vshader:%s", infolog);
	}

	fshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fshader, 1, &fs, NULL);
	glCompileShader(fshader);
	glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
	if (!success){
		glGetShaderInfoLog(fshader, 512, NULL, infolog);
		fprintf_s(stderr, "fshader:%s", infolog);
	}

	shaderprogram = glCreateProgram();
	glAttachShader(shaderprogram, vshader);
	glAttachShader(shaderprogram, fshader);
	glLinkProgram(shaderprogram);
	glGetProgramiv(shaderprogram, GL_LINK_STATUS, &success);
	if (!success){
		glGetProgramInfoLog(shaderprogram, 512, NULL, infolog);
		fprintf_s(stderr, "shaderprogram:%s", infolog);
	}
	glGetProgramiv(shaderprogram, GL_ACTIVE_UNIFORMS, &success);
	printf_s("number:%d\n", success);
	glDetachShader(shaderprogram, vshader);
	glDetachShader(shaderprogram, fshader);
}

int GL::init(int weight, int hight){
	this->weight = weight;
	this->hight = hight;
	keystatus[GLFW_KEY_W] = false;
	keystatus[GLFW_KEY_A] = false;
	keystatus[GLFW_KEY_S] = false;
	keystatus[GLFW_KEY_D] = false;
	keystatus[GLFW_KEY_U] = false;
	keystatus[GLFW_KEY_J] = false;

	if (glfwInit() == GLFW_FALSE){
		fprintf_s(stderr, "failed to init glfw\n");
		glfwTerminate();
	}
	window = glfwCreateWindow(weight, hight, "shader", NULL, NULL);
	if (!window){
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwSetErrorCallback(
		[](int error, const char * desc){
		fprintf_s(stderr, "ERROR:%s\n", desc);
	}
	);

	glfwSetDropCallback(
		window,
		[](GLFWwindow* win, int count, const char * path[]){
		int len = GET_ARRAY_LEN(path), f, path_v = -1, path_f = -1;

		for (int p = 0; p < len; ++p){
			switch (drop_file(path[p])){
			case FILE_NON:
				printf_s("File not found\n");
				break;

			case FILE_OBJ:
				printf_s("Found obj file\n");
				vao = Obj.creatvao_obj(path[p]);
				printf_s("please store this before you create another vao\n");
				break;

			case FILE_VSHADER:
				printf_s("Found vshader file\n");
				if (path_f != -1)
					load_shader(path[p], path[path_f]);
				else
					path_v = p;
				break;

			case FILE_FSHADER:
				printf_s("Found fshader file\n");
				if (path_v != -1)
					load_shader(path[path_v], path[p]);
				else
					path_f = p;
				break;

			default:
				break;
			}
		}
	}
	);
	glfwSetCursorPosCallback(
		window,
		[](GLFWwindow* win, double x, double y){
		xmov += x - xold;
		xold = x;
	}
	);
	glfwSetKeyCallback(
		window,
		[](GLFWwindow* win, int key, int scancode, int action, int mods){
		switch (key){
		case GLFW_KEY_F:
			if (action == GLFW_PRESS){
				glfwSetWindowMonitor(window, isFullScreen ? NULL : glfwGetPrimaryMonitor(), 0, 0, 800, 600, GLFW_DONT_CARE);
				isFullScreen = !isFullScreen;
			}
			break;

		case GLFW_KEY_W:
		case GLFW_KEY_A:
		case GLFW_KEY_S:
		case GLFW_KEY_D:
		case GLFW_KEY_U:
		case GLFW_KEY_J:
			if (action == GLFW_PRESS)
				keystatus[key] = true;
			if (action == GLFW_RELEASE)
				keystatus[key] = false;
			break;

		case GLFW_KEY_R:
			cameraPos = vec3(0, 0, -7);
			break;

		default:
			break;

		}
	}
	);
	//init glew after setting glfw
	if (glewInit() == GLEW_OK)
		printf_s("glew initialized\n");
	else
		fprintf_s(stderr, "failed to initialize glew\n");
	return 0;
}

GLuint GL::load_shader(const char* path, const char* path2){
	string vshader_f_source, fshader_f_source, temp;
	auto shaderfstream = ifstream(path, ios::in);
	if (shaderfstream.is_open()){
		while (getline(shaderfstream, temp)){
			vshader_f_source += temp + "\n";
		}
	}
	shaderfstream.close();

	shaderfstream = ifstream(path2);
	if (shaderfstream.is_open()){
		while (getline(shaderfstream, temp)){
			fshader_f_source += temp + "\n";
		}
	}
	shaderfstream.close();

	GLint success;
	GLchar infolog[512];
	GLuint l_vs, l_fs, l_program;
	char const *l_vshader = vshader_f_source.c_str();
	char const *l_fshader = fshader_f_source.c_str();
	l_vs = glCreateShader(GL_VERTEX_SHADER);
	l_fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(l_vs, 1, &l_vshader, NULL);
	glCompileShader(l_vs);
	glGetShaderiv(l_vs, GL_COMPILE_STATUS, &success);
	if (!success){
		glGetShaderInfoLog(l_vs, 512, NULL, infolog);
		fprintf_s(stderr, "loaded_vshader:%s", infolog);
	}

	glShaderSource(l_fs, 1, &l_fshader, NULL);
	glCompileShader(l_fs);
	glGetShaderiv(l_fs, GL_COMPILE_STATUS, &success);
	if (!success){
		glGetShaderInfoLog(l_fs, 512, NULL, infolog);
		fprintf_s(stderr, "loaded_fshader:%s", infolog);
	}

	l_program = glCreateProgram();
	glAttachShader(l_program, l_vs);
	glAttachShader(l_program, l_fs);
	glLinkProgram(l_program);
	glGetProgramiv(l_program, GL_LINK_STATUS, &success);
	if (!success){
		glGetProgramInfoLog(l_program, 512, NULL, infolog);
		fprintf_s(stderr, "shaderprogram:%s", infolog);
	}
	glDetachShader(l_program, l_vs);
	glDetachShader(l_program, l_fs);

	return l_program;
}

int GL::drop_file(string s){

	auto reg = regex("([0-9a-zA-Z\_\-]+?)\.(obj|vshader|fshader)$");
	smatch sm;
	//use regex to check the dropped file
	if (regex_search(s, sm, reg)){
		if (sm[2] == "obj"){
			glfwSetWindowTitle(window, "obj");
			for (auto x = sm.begin(); x != sm.end(); x++)
				cout << x->str() << endl;
			return FILE_OBJ;
		}
		else if (sm[2] == "vshader"){
			return FILE_VSHADER;
		}
		else if (sm[2] == "fshader"){
			return FILE_FSHADER;
		}
		else
			printf_s("non\n");
		return FILE_NON;
	}
	else
		printf_s("non1\n");
	return FILE_NON;
}

void GL::do_movement(GLfloat deltaTime)
{
	// Camera controls
	GLfloat cameraSpeed = 5.0f * deltaTime;
	if (keystatus[GLFW_KEY_W])
		cameraPos += cameraSpeed * cameraFront;
	else if (keystatus[GLFW_KEY_S])
		cameraPos -= cameraSpeed * cameraFront;
	else if (keystatus[GLFW_KEY_A])
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	//cameraPos -= vec3(1, 0, 0)*cameraSpeed;
	else if (keystatus[GLFW_KEY_D])
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	//cameraPos += vec3(1, 0, 0)*cameraSpeed;
	else if (keystatus[GLFW_KEY_U])
		cameraPos += normalize(cameraUp)*cameraSpeed;
	else if (keystatus[GLFW_KEY_J])
		cameraPos -= normalize(cameraUp)*cameraSpeed;
}

void GL::render(GLuint indicesNum,GLfloat time){
	do_movement(time);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, this->weight, this->hight);
	glBindVertexArray(vao);
	glUseProgram(shaderprogram);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	glDrawElements(GL_TRIANGLES, indicesNum, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}