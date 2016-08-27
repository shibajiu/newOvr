#pragma once
class ovrrealsense
{
private:
	PXCSession *ps;
	PXCSenseManager *psm;
	PXCTouchlessController *ptc;
	ovrUXEventHandler *handler;

	void SetConfiguration();
public:
	ovrrealsense();
	~ovrrealsense();
	void init();
};

class ovrUXEventHandler :public PXCTouchlessController::UXEventHandler{
public:
	virtual  void PXCAPI OnFiredUXEvent(const PXCTouchlessController::UXEventData *uxed);
};