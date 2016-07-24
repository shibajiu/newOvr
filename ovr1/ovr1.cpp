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
		printf_s("Failed to initialize libOVR.");
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
		mat4 view, projection, model;
		ovrGl.init(glwindowSize.x, glwindowSize.y);
		GLuint shaderprogram = ovrGl.get_program();
		ovrGl.load_shader("vs0.vshader", "fs0.fshader");
		glProgramUniformMatrix4fv(shaderprogram, glGetUniformLocation(shaderprogram, "modelMatrix"), 1, GL_FALSE, (float*)&model);
		glProgramUniformMatrix4fv(shaderprogram, glGetUniformLocation(shaderprogram, "viewMatrix"), 1, GL_FALSE, (float*)&view);
		glProgramUniformMatrix4fv(shaderprogram, glGetUniformLocation(shaderprogram, "projectionMatrix"), 1, GL_FALSE, (float*)&projection);

		projection = perspective(radians(60.f), glwindowSize.x / glwindowSize.y, 0.1f, 500.f);
		GLFWwindow* window = ovrGl.getWindow();
		GLfloat lasttime = glfwGetTime();
		while (!glfwWindowShouldClose(window)){
			view = ovrGl.getViewMatrix();
			model = mat4(1.f);


			GLfloat now = glfwGetTime(), deltatime = now - lasttime;
			ovrGl.render(6, deltatime);

			glfwPollEvents();
			glfwSwapBuffers(window);
		}
		
		//system("pause");
	}
	return 0;
}
