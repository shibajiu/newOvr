// ovr1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
void ovrDisplay1st();
void ovrDisplaying(mat4);
#define ONLYGL
#ifdef USE
#pragma region

void rendering_loop();
void quat_to_matrix(const float *quat, float *mat);
void draw_scene(void);
void draw_box(float xsz, float ysz, float zsz, float norm_sign);
void genFBO();
unsigned int gen_chess_tex(float r0, float g0, float b0, float r1, float g1, float b1);
GLuint chess_tex;

void quat_to_matrix(const float *quat, float *mat){
	mat[0] = 1.0 - 2.0 * quat[1] * quat[1] - 2.0 * quat[2] * quat[2];
	mat[4] = 2.0 * quat[0] * quat[1] + 2.0 * quat[3] * quat[2];
	mat[8] = 2.0 * quat[2] * quat[0] - 2.0 * quat[3] * quat[1];
	mat[12] = 0.0f;

	mat[1] = 2.0 * quat[0] * quat[1] - 2.0 * quat[3] * quat[2];
	mat[5] = 1.0 - 2.0 * quat[0] * quat[0] - 2.0 * quat[2] * quat[2];
	mat[9] = 2.0 * quat[1] * quat[2] + 2.0 * quat[3] * quat[0];
	mat[13] = 0.0f;

	mat[2] = 2.0 * quat[2] * quat[0] + 2.0 * quat[3] * quat[1];
	mat[6] = 2.0 * quat[1] * quat[2] - 2.0 * quat[3] * quat[0];
	mat[10] = 1.0 - 2.0 * quat[0] * quat[0] - 2.0 * quat[1] * quat[1];
	mat[14] = 0.0f;

	mat[3] = mat[7] = mat[11] = 0.0f;
	mat[15] = 1.0f;
}

void draw_scene(void){
	int i;
	float grey[] = { 0.8, 0.8, 0.8, 1 };
	float col[] = { 0, 0, 0, 1 };
	float lpos[][4] = {
		{ -8, 2, 10, 1 },
		{ 0, 15, 0, 1 }
	};
	float lcol[][4] = {
		{ 0.8, 0.8, 0.8, 1 },
		{ 0.4, 0.3, 0.3, 1 }
	};

	for (i = 0; i<2; i++) {
		glLightfv(GL_LIGHT0 + i, GL_POSITION, lpos[i]);
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, lcol[i]);
	}

	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();
	glTranslatef(0, 10, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, grey);
	glBindTexture(GL_TEXTURE_2D, chess_tex);
	glEnable(GL_TEXTURE_2D);
	draw_box(30, 20, 30, -1.0);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	for (i = 0; i<4; i++) {
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, grey);
		glPushMatrix();
		glTranslatef(i & 1 ? 5 : -5, 1, i & 2 ? -5 : 5);
		draw_box(0.5, 2, 0.5, 1.0);
		glPopMatrix();

		col[0] = i & 1 ? 1.0 : 0.3;
		col[1] = i == 0 ? 1.0 : 0.3;
		col[2] = i & 2 ? 1.0 : 0.3;
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, col);

		glPushMatrix();
		if (i & 1) {
			glTranslatef(0, 0.25, i & 2 ? 2 : -2);
		}
		else {
			glTranslatef(i & 2 ? 2 : -2, 0.25, 0);
		}
		draw_box(0.5, 0.5, 0.5, 1.0);
		glPopMatrix();
	}

	col[0] = 1;
	col[1] = 1;
	col[2] = 0.4;
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, col);
	draw_box(0.05, 1.2, 6, 1.0);
	draw_box(6, 1.2, 0.05, 1.0);
}

void draw_box(float xsz, float ysz, float zsz, float norm_sign){
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glScalef(xsz * 0.5, ysz * 0.5, zsz * 0.5);

	if (norm_sign < 0.0) {
		glFrontFace(GL_CW);
	}

	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1 * norm_sign);
	glTexCoord2f(0, 0); glVertex3f(-1, -1, 1);
	glTexCoord2f(1, 0); glVertex3f(1, -1, 1);
	glTexCoord2f(1, 1); glVertex3f(1, 1, 1);
	glTexCoord2f(0, 1); glVertex3f(-1, 1, 1);
	glNormal3f(1 * norm_sign, 0, 0);
	glTexCoord2f(0, 0); glVertex3f(1, -1, 1);
	glTexCoord2f(1, 0); glVertex3f(1, -1, -1);
	glTexCoord2f(1, 1); glVertex3f(1, 1, -1);
	glTexCoord2f(0, 1); glVertex3f(1, 1, 1);
	glNormal3f(0, 0, -1 * norm_sign);
	glTexCoord2f(0, 0); glVertex3f(1, -1, -1);
	glTexCoord2f(1, 0); glVertex3f(-1, -1, -1);
	glTexCoord2f(1, 1); glVertex3f(-1, 1, -1);
	glTexCoord2f(0, 1); glVertex3f(1, 1, -1);
	glNormal3f(-1 * norm_sign, 0, 0);
	glTexCoord2f(0, 0); glVertex3f(-1, -1, -1);
	glTexCoord2f(1, 0); glVertex3f(-1, -1, 1);
	glTexCoord2f(1, 1); glVertex3f(-1, 1, 1);
	glTexCoord2f(0, 1); glVertex3f(-1, 1, -1);
	glEnd();
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0, 1 * norm_sign, 0);
	glTexCoord2f(0.5, 0.5); glVertex3f(0, 1, 0);
	glTexCoord2f(0, 0); glVertex3f(-1, 1, 1);
	glTexCoord2f(1, 0); glVertex3f(1, 1, 1);
	glTexCoord2f(1, 1); glVertex3f(1, 1, -1);
	glTexCoord2f(0, 1); glVertex3f(-1, 1, -1);
	glTexCoord2f(0, 0); glVertex3f(-1, 1, 1);
	glEnd();
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0, -1 * norm_sign, 0);
	glTexCoord2f(0.5, 0.5); glVertex3f(0, -1, 0);
	glTexCoord2f(0, 0); glVertex3f(-1, -1, -1);
	glTexCoord2f(1, 0); glVertex3f(1, -1, -1);
	glTexCoord2f(1, 1); glVertex3f(1, -1, 1);
	glTexCoord2f(0, 1); glVertex3f(-1, -1, 1);
	glTexCoord2f(0, 0); glVertex3f(-1, -1, -1);
	glEnd();

	glFrontFace(GL_CCW);
	glPopMatrix();
}

unsigned int gen_chess_tex(float r0, float g0, float b0, float r1, float g1, float b1){
	int i, j;
	unsigned int tex;
	unsigned char img[8 * 8 * 3];
	unsigned char *pix = img;

	for (i = 0; i<8; i++) {
		for (j = 0; j<8; j++) {
			int black = (i & 1) == (j & 1);
			pix[0] = (black ? r0 : r1) * 255;
			pix[1] = (black ? g0 : g1) * 255;
			pix[2] = (black ? b0 : b1) * 255;
			pix += 3;
		}
	}

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 8, 8, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
	printf("generated texture\n");
	return tex;
}

#pragma endregion
#endif


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
#ifndef ONLYGL
	if (!OVR_SUCCESS(ovr_Initialize(nullptr))){
		fprintf(stderr, "Failed to initialize libOVR.\n");
		return 0;
	}

	static ovrSession session;
	static ovrGraphicsLuid luid;

	if (OVR_FAILURE(ovr_Create(&session, &luid))){
		fprintf(stderr, "Failed to create the HMD.\n");
		ovr_Shutdown();
		return 0;
	}

	ovrHmdDesc desc = ovr_GetHmdDesc(session);
	ovrLayerEyeFov layer;
	bool isVisible = 1;
	ovrVector3f hmdToEyeViewOffset[2];
	ovrSizei fovtextureSize[2];

	ovrSwapTextureSet *tSet[2];
	//ovrGLTexture *tex;
	GL *ovrGL = new GL(desc.Resolution.w*0.5, desc.Resolution.h*0.5);
	GLuint depthbuffer[2];
	GLuint ovrFramBuffer[2];
	for (int i = 0; i < 2; ++i){
		fovtextureSize[i] = ovr_GetFovTextureSize(session, (ovrEyeType)i, desc.DefaultEyeFov[i], 1.f);
		layer.Viewport[i] = Recti(fovtextureSize[i]);
		if (ovr_CreateSwapTextureSetGL(session, GL_SRGB8_ALPHA8, fovtextureSize[i].w, fovtextureSize[i].h, tSet + i) == ovrSuccess){
			for (int it = 0; it < tSet[i]->TextureCount; ++it){
				auto tex = (ovrGLTexture*)&tSet[i]->Textures[it];
				glBindTexture(GL_TEXTURE_2D, tex->OGL.TexId);
				glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
		}
		//set frambuffer for each eye
	
		glGenFramebuffers(1, &ovrFramBuffer[i]);
		//eyeDepthBuffer[eye] = new DepthBuffer(eyeRenderTexture[eye]->GetSize(), 0);
		glGenTextures(1, depthbuffer+i);
		glBindTexture(GL_TEXTURE_2D, depthbuffer[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, fovtextureSize[i].w, fovtextureSize[i].h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
	}

	
	ovrGLTexture* mirrorTexture;
	if (OVR_FAILURE(ovr_CreateMirrorTextureGL(session, GL_SRGB8_ALPHA8, desc.Resolution.w*0.5, desc.Resolution.h*0.5,reinterpret_cast<ovrTexture**>(&mirrorTexture)))){
		fprintf(stderr, "Failed to create mirror texture.");
	}
	GLuint mirrorFBO;
	glGenFramebuffers(1, &mirrorFBO);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFBO);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mirrorTexture->OGL.TexId, 0);
	glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	layer.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;
	layer.Header.Type = ovrLayerType_EyeFov;
	for (int eye = 0; eye < 2; ++eye){
		layer.ColorTexture[eye] = tSet[eye];
		auto renderDesc = ovr_GetRenderDesc(session, (ovrEyeType)eye, desc.DefaultEyeFov[eye]);
		layer.Fov[eye] = renderDesc.Fov;
		hmdToEyeViewOffset[eye] = renderDesc.HmdToEyeViewOffset;
	}

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

#ifdef USE
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_NORMALIZE);
	float V[] = { 1, 1, 1, 1 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, V);
	glEnable(GL_COLOR_MATERIAL);

	glClearColor(1, 1, 1, 1);
	chess_tex = gen_chess_tex(1.0, 0.7, 0.4, 0.4, 0.7, 1.0);
#endif

	
	auto window = ovrGL->getWindow();
	static GLuint shaderprogram, myVao, indexNum;
	static mat4 view, projection, model = mat4(1);
	static	GLfloat lasttime;

	shaderprogram = ovrGL->load_shader("vs0.vshader", "fs0.fshader");
	//ovrGL->setCameraPos(vec3(200, 200, 200));
	myVao = ovrGL->creatVao("e:/vessel.obj", FILE_VESSEL);
	indexNum = ovrGL->getElementNum();
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_DEPTH_TEST | GL_STENCIL_TEST);
	glDepthFunc(GL_LESS);
	//glDepthMask(GL_TRUE);
	//glStencilMask(0x00);
	//glEnable(GL_CULL_FACE);
	lasttime = glfwGetTime();
	static Vector3f Pos2(0.0f, 1.6f, -5.0f);
	Pos2 += Matrix4f::RotationY(0).Transform(Vector3f(200.f, 200, 200));
	while (!glfwWindowShouldClose(window)){

		double displayMidpointSeconds = ovr_GetPredictedDisplayTime(session, 0);
		double sensorSampleTime = ovr_GetTimeInSeconds();
		ovrTrackingState hmdState = ovr_GetTrackingState(session, displayMidpointSeconds, ovrTrue);
		ovr_CalcEyePoses(hmdState.HeadPose.ThePose, hmdToEyeViewOffset, layer.RenderPose);
		layer.SensorSampleTime = sensorSampleTime;
		glClearColor(0, 1, 0, 0);
		//view = ovrGL->getViewMatrix();

		//Pos2.y = ovr_GetFloat(session, OVR_KEY_EYE_HEIGHT, Pos2.y);

		if (isVisible){
			for (int eye = 0; eye < 2; ++eye){
				// Increment to use next texture, just before writing
				tSet[eye]->CurrentIndex = (tSet[eye]->CurrentIndex + 1) % tSet[eye]->TextureCount;

				// Switch to eye render target
				//eyeRenderTexture[eye]->SetAndClearRenderSurface(eyeDepthBuffer[eye]);
				auto texl = reinterpret_cast<ovrGLTexture*>(&tSet[eye]->Textures[tSet[eye]->CurrentIndex]);

				glBindFramebuffer(GL_FRAMEBUFFER, ovrFramBuffer[eye]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texl->OGL.TexId, 0);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthbuffer[eye], 0);

				glViewport(0, 0, fovtextureSize[eye].w, fovtextureSize[eye].h);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glEnable(GL_FRAMEBUFFER_SRGB);
				auto proj = ovrMatrix4f_Projection(desc.DefaultEyeFov[eye], 0.1, 1000.f, ovrProjection_RightHanded);

				mat4 projection;
				Matrix4f rollPitchYaw = Matrix4f::RotationY(0);
				Matrix4f finalRollPitchYaw = rollPitchYaw * Matrix4f(layer.RenderPose[eye].Orientation);
				Vector3f finalUp = finalRollPitchYaw.Transform(Vector3f(0, 1, 0));
				Vector3f finalForward = finalRollPitchYaw.Transform(Vector3f(0, 0, -1));
				Vector3f shiftedEyePos = Pos2 + rollPitchYaw.Transform(layer.RenderPose[eye].Position);
				
				GLfloat now = glfwGetTime(), deltatime = now - lasttime;
				lasttime = now;
				ovrGL->keyShift(deltatime, shiftedEyePos, finalForward, finalUp,Pos2);
				Matrix4f viewOvr = Matrix4f::LookAtRH(shiftedEyePos, shiftedEyePos + finalForward, finalUp);
			/*for (int i = 0; i < 4; ++i){

					for (int j = 0; i < 4; ++j){
						projection[i][j] = proj.M[j][i];
						view[i][j] = viewOvr.M[j][i];
					}
				}*/
				
				glDepthMask(GL_FALSE);
				glBindVertexArray(sb_vao);
				glUseProgram(skyboxprogram);
				glUniformMatrix4fv(glGetUniformLocation(skyboxprogram, "modelMatrix"), 1, GL_FALSE, (float*)&model);
				glUniformMatrix4fv(glGetUniformLocation(skyboxprogram, "viewMatrix"), 1, GL_TRUE, (float*)&viewOvr.M[0]);
				glUniformMatrix4fv(glGetUniformLocation(skyboxprogram, "projectionMatrix"), 1, GL_TRUE, (float*)&proj.M[0]);
				glActiveTexture(GL_TEXTURE0);
				glUniform1i(glGetUniformLocation(skyboxprogram,"texSkyBox"),0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
				glDrawArrays(GL_TRIANGLES, 0, 36);
				glBindVertexArray(0);
				glUseProgram(0);
				glDepthMask(GL_TRUE);
				
				char buffer[512];
				sprintf_s(buffer, 512, "pos:%4.2f %4.2f %4.2f", shiftedEyePos.x, shiftedEyePos.y, shiftedEyePos.z);
				glfwSetWindowTitle(window, buffer);

				glUseProgram(shaderprogram);
				glUniformMatrix4fv(glGetUniformLocation(shaderprogram, "modelMatrix"), 1, GL_FALSE, (float*)&model);
				glUniformMatrix4fv(glGetUniformLocation(shaderprogram, "viewMatrix"), 1, GL_TRUE, (float*)&viewOvr.M[0]);
				glUniformMatrix4fv(glGetUniformLocation(shaderprogram, "projectionMatrix"), 1, GL_TRUE, (float*)&proj.M[0]);

				//ovrGL->render(indexNum, deltatime, myVao);
				glBindVertexArray(myVao);
				glDrawElements(GL_TRIANGLES, indexNum, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
				glUseProgram(0);

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
		}
		// Set up positional data.
		ovrViewScaleDesc viewScaleDesc;
		viewScaleDesc.HmdSpaceToWorldScaleInMeters = 1.0f;
		viewScaleDesc.HmdToEyeViewOffset[0] = hmdToEyeViewOffset[0];
		viewScaleDesc.HmdToEyeViewOffset[1] = hmdToEyeViewOffset[1];



		ovrLayerHeader* layers = &layer.Header;
		isVisible = (ovr_SubmitFrame(session, 0, &viewScaleDesc, &layers, 1) == ovrSuccess);
		//printf("isVisible:%d\n", isVisible);

		// Blit mirror texture to back buffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		GLint w = mirrorTexture->OGL.Header.TextureSize.w;
		GLint h = mirrorTexture->OGL.Header.TextureSize.h;
		glBlitFramebuffer(0, h, w, 0,
			0, 0, w, h,
			GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}
#else
#ifdef ONLYGL
	GL *ovrGL = new GL();
		mat4 view, projection, model = mat4(1);
		ovrGL->init(800, 600);

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
		glEnable(GL_DEPTH_TEST | GL_STENCIL_TEST);
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);
		glStencilMask(0x00);
		glEnable(GL_CULL_FACE);
		while (!glfwWindowShouldClose(window)){
			view = ovrGL->getViewMatrix();
			glClearColor(0, 0, 0, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glViewport(0, 0, 800, 600);

			glDepthMask(GL_FALSE);
			glBindVertexArray(sb_vao);
			glUseProgram(skyboxprogram);
			glUniformMatrix4fv(glGetUniformLocation(skyboxprogram, "modelMatrix"), 1, GL_FALSE, (float*)&model);
			glUniformMatrix4fv(glGetUniformLocation(skyboxprogram, "viewMatrix"), 1, GL_FALSE, (float*)&view);
			glUniformMatrix4fv(glGetUniformLocation(skyboxprogram, "projectionMatrix"), 1, GL_FALSE, (float*)&projection);
			glActiveTexture(GL_TEXTURE0);
			glUniform1i(glGetUniformLocation(skyboxprogram, "texSkyBox"), 0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			glBindVertexArray(0);
			glUseProgram(0);
			glDepthMask(GL_TRUE);

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

			glfwPollEvents();
			glfwSwapBuffers(window);
		}
		delete ovrGL;
#endif
#endif
	glfwTerminate();
#ifndef ONLYGL
	ovr_Destroy(session);
#endif
	//system("pause");
	return 0;
}