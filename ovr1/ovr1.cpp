// ovr1.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
GLfloat skyboxVertices[] = {
	// Positions          
	-1.0f, 1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,

	-1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,

	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,

	-1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f
};


int _tmain(int argc, _TCHAR* argv[]){


	GL *ovrGL = new GL();
		mat4 view, projection, model = mat4(1);
		ovrGL->init(800, 600);
	A_model* model1 = new A_model("e:/it.obj");

		GLuint skybox = SOIL_load_OGL_cubemap(
			"mp_orbital/orbital-element_ft.tga",
			"mp_orbital/orbital-element_bk.tga",
			"mp_orbital/orbital-element_up.tga",
			"mp_orbital/orbital-element_dn.tga",
			"mp_orbital/orbital-element_rt.tga",
			"mp_orbital/orbital-element_lf.tga",
			SOIL_LOAD_RGB,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS
			);

		GLuint sb_vao, sb_vbo;
		glGenVertexArrays(1, &sb_vao);
		glGenBuffers(1, &sb_vbo);
		glBindVertexArray(sb_vao);
		glBindBuffer(GL_ARRAY_BUFFER, sb_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);

		GL glsb;
		GLuint skyboxprogram = glsb.load_shader(const_cast<char*>("skybox.vshader"), const_cast<char*>("skybox.fshader"));

		GLuint shaderprogram = ovrGL->load_shader("vs0.vshader", "fs0.fshader");

		GLint matAmbientLoc = glGetUniformLocation(shaderprogram, "vessel.ambient");
		GLint matDiffuseLoc = glGetUniformLocation(shaderprogram, "vessel.diffuse");
		GLint matSpecularLoc = glGetUniformLocation(shaderprogram, "vessel.specular");
		GLint matShineLoc = glGetUniformLocation(shaderprogram, "vessel.shininess");
		glProgramUniform3f(shaderprogram, matAmbientLoc, 1.0f, 0.5f, 0.31f);
		glProgramUniform3f(shaderprogram, matDiffuseLoc, 1.0f, 0.5f, 0.31f);
		glProgramUniform3f(shaderprogram, matSpecularLoc, 0.5f, 0.5f, 0.5f);
		glProgramUniform1f(shaderprogram, matShineLoc, 32.0f);
		
		GLint viewPosLoc = glGetUniformLocation(shaderprogram, "viewPos");
		GLint lightPosition = glGetUniformLocation(shaderprogram, "light.position");
		GLint lightAmbientLoc = glGetUniformLocation(shaderprogram, "light.ambient");
		GLint lightDiffuseLoc = glGetUniformLocation(shaderprogram, "light.diffuse");
		GLint lightSpecularLoc = glGetUniformLocation(shaderprogram, "light.specular");
		glProgramUniform3f(shaderprogram, lightAmbientLoc, 0.2f, 0.2f, 0.2f);
		glProgramUniform3f(shaderprogram, lightDiffuseLoc, 0.5f, 0.5f, 0.5f);
		glProgramUniform3f(shaderprogram, lightSpecularLoc, 1.0f, 1.0f, 1.0f);
		glProgramUniform4f(shaderprogram, lightPosition, 1.0f, 1.0f, 1.0f, 0.f);

		//ovrGL->setCameraPos(vec3(200, 200, 200));
		projection = perspective(radians(60.f), 4.f/3 , 0.1f, 1000.f);
		GLFWwindow* window = ovrGL->getWindow();
		GLfloat lasttime = glfwGetTime();
		GLuint myVao = ovrGL->creatVao("e:/suzanne.obj", FILE_WITH_UNSORTED_NORMALS);
		GLuint indexNum = ovrGL->getElementNum();
		glClearColor(1, 0, 0, 1);
		glEnable(GL_DEPTH_TEST);
		//glDepthMask(GL_FALSE);
		while (!glfwWindowShouldClose(window)){
			view = ovrGL->getViewMatrix();
			glViewport(0, 0, 800, 600);
			glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

			glUseProgram(shaderprogram);
			vec3 pos = ovrGL->getCameraPos();
			glUniformMatrix4fv(glGetUniformLocation(shaderprogram, "modelMatrix"), 1, GL_FALSE, (float*)&model);
			glUniformMatrix4fv(glGetUniformLocation(shaderprogram, "viewMatrix"), 1, GL_FALSE, (float*)&view);
			glUniformMatrix4fv(glGetUniformLocation(shaderprogram, "projectionMatrix"), 1, GL_FALSE, (float*)&projection);
			glUniform3fv(viewPosLoc,1, (GLfloat*)&pos);
			GLfloat now = glfwGetTime(), deltatime = now - lasttime;
			lasttime = now;
			glActiveTexture(GL_TEXTURE0);
			glUniform1i(glGetUniformLocation(skyboxprogram, "texSkyBox"), 0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
			ovrGL->render(indexNum, deltatime, myVao);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			glUseProgram(0);

			glDepthFunc(GL_LEQUAL);
			glBindVertexArray(sb_vao);
			glUseProgram(skyboxprogram);
			glUniformMatrix4fv(glGetUniformLocation(skyboxprogram, "model"), 1, GL_FALSE, (float*)&model);
			glUniformMatrix4fv(glGetUniformLocation(skyboxprogram, "view"), 1, GL_FALSE, (float*)&view);
			glUniformMatrix4fv(glGetUniformLocation(skyboxprogram, "projection"), 1, GL_FALSE, (float*)&projection);
			glActiveTexture(GL_TEXTURE0);
			glUniform1i(glGetUniformLocation(skyboxprogram, "texSkyBox"), 0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			glBindVertexArray(0);
			glUseProgram(0);
			glDepthFunc(GL_LESS);

			glfwPollEvents();
			glfwSwapBuffers(window);
		}
		delete ovrGL;

	glfwTerminate();
	//system("pause");
	return 0;
}