// ovr1.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include <boost/thread/thread.hpp>

void ovrDisplay1st();
void ovrDisplaying(mat4);
//#define ONLYGL
void rsthread_main(ovrrealsense * rs);
quat RotationBetweenVectors(vec3 start, vec3 dest);
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

	//ovr init part
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

	//create gl
	GL *ovrGL = new GL(desc.Resolution.w*0.5, desc.Resolution.h*0.5);
	GLuint depthbuffer[2];
	GLuint ovrFramBuffer[2];

	//set framebuffer for each eye
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

	//set framebuffer for pc window
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

	//skybox texture
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

	//gen skybox's vao
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

	//get glfw window
	auto window = ovrGL->getWindow();
	static GLuint shaderprogram, myVao, indexNum;
	static mat4 view, projection, model = mat4(1);
	static	GLfloat lasttime;

	ovrrealsense *ovrrs = new ovrrealsense(window);
	boost::thread rsthread(rsthread_main,ovrrs);

	//model shader
	shaderprogram = ovrGL->load_shader("vs0.vshader", "fs0.fshader");
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

	//load model
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

		//render part
		if (isVisible){
			for (int eye = 0; eye < 2; ++eye){
				// Increment to use next texture, just before writing
				tSet[eye]->CurrentIndex = (tSet[eye]->CurrentIndex + 1) % tSet[eye]->TextureCount;

				// Switch to eye render target
				//eyeRenderTexture[eye]->SetAndClearRenderSurface(eyeDepthBuffer[eye]);
				auto texl = reinterpret_cast<ovrGLTexture*>(&tSet[eye]->Textures[tSet[eye]->CurrentIndex]);

				//render into each eye's framebuffer
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
				
				char buffer[512];
				sprintf_s(buffer, 512, "pos:%4.2f %4.2f %4.2f", shiftedEyePos.x, shiftedEyePos.y, shiftedEyePos.z);
				glfwSetWindowTitle(window, buffer);

				auto handmoved = ovrrs->getHandPos();
				//cout << handmoved.x << "/" << handmoved.y << "/" << handmoved.z << endl;
				quat rot1 = RotationBetweenVectors(vec3(0.0f, 0.0f, 1.0f), handmoved);
				mat4 rotmat = toMat4(rot1);
				mat4 rotmodel = rotmat*model;
				//render model
				glUseProgram(shaderprogram);
				glUniformMatrix4fv(glGetUniformLocation(shaderprogram, "modelMatrix"), 1, GL_FALSE, (float*)&rotmodel);
				glUniformMatrix4fv(glGetUniformLocation(shaderprogram, "viewMatrix"), 1, GL_TRUE, (float*)&viewOvr.M[0]);
				glUniformMatrix4fv(glGetUniformLocation(shaderprogram, "projectionMatrix"), 1, GL_TRUE, (float*)&proj.M[0]);

				//ovrGL->render(indexNum, deltatime, myVao);
				glBindVertexArray(myVao);
				glDrawElements(GL_TRIANGLES, indexNum, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
				glUseProgram(0);

				//render skybox
				glDepthFunc(GL_LEQUAL);
				glBindVertexArray(sb_vao);
				glUseProgram(skyboxprogram);
				glUniformMatrix4fv(glGetUniformLocation(skyboxprogram, "modelMatrix"), 1, GL_FALSE, (float*)&model);
				glUniformMatrix4fv(glGetUniformLocation(skyboxprogram, "viewMatrix"), 1, GL_TRUE, (float*)&viewOvr.M[0]);
				glUniformMatrix4fv(glGetUniformLocation(skyboxprogram, "projectionMatrix"), 1, GL_TRUE, (float*)&proj.M[0]);
				glActiveTexture(GL_TEXTURE0);
				glUniform1i(glGetUniformLocation(skyboxprogram, "texSkyBox"), 0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
				glDrawArrays(GL_TRIANGLES, 0, 36);
				glBindVertexArray(0);
				glUseProgram(0);
				glDepthFunc(GL_LESS);

				//clear the tex attach to the fbo for next render process
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
		//render mirrorfbo into fbo0
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

	//destroy glfw before ovr
	glfwTerminate();
	ovr_Destroy(session);

	//system("pause");
	return 0;

}

void rsthread_main(ovrrealsense * rs){

	rs->Start();
}

quat RotationBetweenVectors(vec3 start, vec3 dest){
    start = normalize(start);
    dest = normalize(dest);

    float cosTheta = dot(start, dest);
    vec3 rotationAxis;

    if (cosTheta < -1 + 0.001f){
        // special case when vectors in opposite directions:
        // there is no "ideal" rotation axis
        // So guess one; any will do as long as it's perpendicular to start
        rotationAxis = cross(vec3(0.0f, 0.0f, 1.0f), start);
        if (length2(rotationAxis) < 0.01 ) // bad luck, they were parallel, try again!
            rotationAxis = cross(vec3(1.0f, 0.0f, 0.0f), start);
		
        rotationAxis = normalize(rotationAxis);
        return angleAxis(180.0f, rotationAxis);
		
    }

    rotationAxis = cross(start, dest);

    float s = sqrt( (1+cosTheta)*2 );
    float invs = 1 / s;

    return quat(
        s * 0.5f,
        rotationAxis.x * invs,
        rotationAxis.y * invs,
        rotationAxis.z * invs
    );

}