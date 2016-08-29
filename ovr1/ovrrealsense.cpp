#include "stdafx.h"
#include "ovrrealsense.h"

ovrrealsense::ovrrealsense(GLFWwindow *_window){
	ps = nullptr;
	psm = nullptr;
	ptc = nullptr;
	handlerAlert = new ovrAlertHandler();
	handlerUXEvent = new ovrUXEventHandler();
	handlerUXEvent->rs = this;
	isStop = false;
	window = _window;
	roll = glm::mat4(1.f);
	oldpos = glm::vec3(0.5f);
	moved = glm::vec3(0.f);
}


ovrrealsense::~ovrrealsense(){
	ptc->UnsubscribeAlert(handlerAlert);
	ptc->UnsubscribeEvent(handlerUXEvent);
	delete handlerAlert;
	delete handlerUXEvent;
	//ptc->Release();//managed by psm
	psm->Close();
	psm->Release();
}

void ovrrealsense::Start(){
	ps = PXCSession::CreateInstance();
	if (!ps){
		cerr << "PXCSession::CreateInstance::FAILED" << endl;
		return;
	}
	psm = PXCSenseManager::CreateInstance();
	if (!psm){
		cerr << "PXCSenseManager::CreateInstance::FAILED" << endl;
		return;
	}
	if (psm->EnableTouchlessController() <= pxcStatus::PXC_STATUS_NO_ERROR){
		cerr << "PXCSenseManager::EnableTouchlessController::FAILED" << endl;
		return;
	}
	psm->Init();
	ptc = psm->QueryTouchlessController();
	IFCERR(ptc == NULL, "PXCSenseManager::QueryTouchlessController::FAILED");
	ptc->SubscribeEvent(handlerUXEvent);
	ptc->SubscribeAlert(handlerAlert);

	while (psm->AcquireFrame(0) >= pxcStatus::PXC_STATUS_NO_ERROR){
		if (isStop) break;
		psm->ReleaseFrame();
	}
}

void ovrrealsense::SetConfiguration(){
	PXCTouchlessController::ProfileInfo _ptc_info;
	IFCERR(ptc->QueryProfile(&_ptc_info) < pxcStatus::PXC_STATUS_NO_ERROR, "PXCTouchlessController::QueryProfile::FAILED");
	_ptc_info.config |=	PXCTouchlessController::ProfileInfo::Configuration_Allow_Zoom | 
						PXCTouchlessController::ProfileInfo::Configuration_Scroll_Horizontally |
						PXCTouchlessController::ProfileInfo::Configuration_Scroll_Vertically;
	IFCERR(ptc->SetProfile(&_ptc_info) < pxcStatus::PXC_STATUS_NO_ERROR, "PXCTouchlessController::SetProfile::FAILED");
	ptc->SetScrollSensitivity(1.f);
	ptc->SetPointerSensitivity(PXCTouchlessController::PointerSensitivity_Smoothed);
}

glm::vec3 ovrrealsense::getHandMove(){
	//return glm::vec3(0.2, 0, 0);//for test
	return moved;
}

void ovrUXEventHandler::OnFiredUXEvent(const PXCTouchlessController::UXEventData * _data){
	switch (_data->type){
	case PXCTouchlessController::UXEventData::UXEvent_ScrollDown:

	case PXCTouchlessController::UXEventData::UXEvent_ScrollLeft:

	case PXCTouchlessController::UXEventData::UXEvent_ScrollRight:

	case PXCTouchlessController::UXEventData::UXEvent_ScrollUp:

		break;
	default:
		break;
	}

	if (rs->oldpos == glm::vec3(0.5f)){
		rs->oldpos = glm::vec3(_data->position.x, _data->position.y, _data->position.z);
	}
	rs->moved = getPosition_from_UXEventData(const_cast<PXCTouchlessController::UXEventData*>(_data)) - rs->oldpos;
}

glm::vec3 ovrUXEventHandler::PXCPoint3_to_vec3(PXCPoint3DF32 &_position){
	return glm::vec3(_position.x, _position.y, _position.z);
}

glm::vec3 ovrUXEventHandler::getPosition_from_UXEventData(PXCTouchlessController::UXEventData * _data){
	return PXCPoint3_to_vec3(_data->position);
}

void ovrAlertHandler::OnFiredAlert(const PXCTouchlessController::AlertData *_data){
	switch (_data->type){
	case PXCTouchlessController::AlertData::Alert_TooClose:
		
	case PXCTouchlessController::AlertData::Alert_TooFar:

	case PXCTouchlessController::AlertData::Alert_NoAlerts:
		break;
	}
}