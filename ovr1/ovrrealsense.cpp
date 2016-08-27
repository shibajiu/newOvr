#include "stdafx.h"
#include "ovrrealsense.h"


ovrrealsense::ovrrealsense()
{
}


ovrrealsense::~ovrrealsense(){
	//ptc->Release();//managed by psm
	psm->Close();
	psm->Release();
}

void ovrrealsense::init(){
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
	ptc->SubscribeEvent(handler);
}

void ovrrealsense::SetConfiguration(){
	PXCTouchlessController::ProfileInfo *_ptc_info;
	IFCERR(ptc->QueryProfile(_ptc_info) < pxcStatus::PXC_STATUS_NO_ERROR, "PXCTouchlessController::QueryProfile::FAILED");
	_ptc_info->config |=	PXCTouchlessController::ProfileInfo::Configuration_Allow_Zoom | 
						PXCTouchlessController::ProfileInfo::Configuration_Scroll_Horizontally |
						PXCTouchlessController::ProfileInfo::Configuration_Scroll_Vertically;
	IFCERR(ptc->SetProfile(_ptc_info) < pxcStatus::PXC_STATUS_NO_ERROR, "PXCTouchlessController::SetProfile::FAILED");
}