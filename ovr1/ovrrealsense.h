#pragma once
class ovrUXEventHandler;
class ovrAlertHandler;
class ovrrealsense
{
private:
	friend class ovrUXEventHandler;
	PXCSession *ps;
	PXCSenseManager *psm;
	PXCTouchlessController *ptc;
	ovrUXEventHandler *handlerUXEvent;
	ovrAlertHandler * handlerAlert;
	GLFWwindow *window;
	glm::mat4 roll;
	glm::vec3 oldpos;
	glm::vec3 moved;

	bool isStop;

	void SetConfiguration();

public:
	//using GL::setWindowTitle(GLFWwindow*, bool);
	ovrrealsense(GLFWwindow*);
	~ovrrealsense();
	void Start();

};

class ovrUXEventHandler :public PXCTouchlessController::UXEventHandler{
private:
	friend class ovrrealsense;
	ovrrealsense* rs;
	glm::vec3 PXCPoint3_to_vec3(PXCPoint3DF32 &);
	glm::vec3 getPosition_from_UXEventData(PXCTouchlessController::UXEventData *);

public:	
	virtual void PXCAPI OnFiredUXEvent(const PXCTouchlessController::UXEventData *);
};

class ovrAlertHandler :public PXCTouchlessController::AlertHandler{
public:
	virtual void PXCAPI OnFiredAlert(const PXCTouchlessController::AlertData *);
};

class ovrActionHandler :public PXCTouchlessController::ActionHandler{
public:	
	virtual void PXCAPI OnFiredAction(const PXCTouchlessController::Action);
};