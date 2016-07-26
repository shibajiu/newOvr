// ovr1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define VALIDATE(x,msg) if(!(x)){fprintf_s(stderr,msg);}

static bool isOnlyGL;
static vec2 glwindowSize(800, 600);

int _tmain(int argc, _TCHAR* argv[]){
	isOnlyGL = false;
	// Initializes LibOVR, and the Rift
	ovrResult result = ovr_Initialize(nullptr);
	GL ovrGl;
	if (!OVR_SUCCESS(result)){
		printf_s("Failed to initialize libOVR.\n");
		isOnlyGL = true;
		goto Done;
	}

	TextureBuffer * eyeRenderTexture[2] = { nullptr, nullptr };
	DepthBuffer   * eyeDepthBuffer[2] = { nullptr, nullptr };
	ovrMirrorTexture mirrorTexture = nullptr;
	GLuint          mirrorFBO = 0;
	bool isVisible = true;
	long long frameIndex = 0;

	ovrSession session;
	ovrGraphicsLuid luid;
	result = ovr_Create(&session, &luid);
	if (!OVR_SUCCESS(result))
		return -1;

Done:
	if (isOnlyGL){
		mat4 view, projection, model=mat4(1);
		ovrGl.init(glwindowSize.x, glwindowSize.y);

		GLuint shaderprogram = ovrGl.load_shader("vs0.vshader", "fs0.fshader");
		
		ovrGl.setCameraPos(vec3(200, 200, 200));
		projection = perspective(radians(60.f), glwindowSize.x / glwindowSize.y, 0.1f, 1000.f);
		GLFWwindow* window = ovrGl.getWindow();
		GLfloat lasttime = glfwGetTime();
		GLuint myVao = ovrGl.creatVao("e:/vessel.obj", FILE_VESSEL);
		GLuint indexNum = ovrGl.getElementNum();
		glEnable(GL_DEPTH_TEST | GL_STENCIL_TEST);
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);
		glStencilMask(0x00);
		glEnable(GL_CULL_FACE);
		
		while (!glfwWindowShouldClose(window)){
			view = ovrGl.getViewMatrix();
			glClearColor(0, 0, 0, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glViewport(0, 0, 800,600);

			glUseProgram(shaderprogram);
			glUniformMatrix4fv(glGetUniformLocation(shaderprogram, "modelMatrix"), 1, GL_FALSE, (float*)&model);
			glUniformMatrix4fv(glGetUniformLocation(shaderprogram, "viewMatrix"), 1, GL_FALSE, (float*)&view);
			glUniformMatrix4fv(glGetUniformLocation(shaderprogram, "projectionMatrix"), 1, GL_FALSE, (float*)&projection);

			GLfloat now = glfwGetTime(), deltatime = now - lasttime;
			lasttime = now;
			ovrGl.render(indexNum, deltatime, myVao);

			glfwPollEvents();
			glfwSwapBuffers(window);
		}

		//system("pause");
	}
	glfwTerminate();

	return 0;
}
