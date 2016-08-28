// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <thread>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

// TODO: reference additional headers your program requires here

//self headers
#include "glitems.h"
//realsense includes
#include <pxcsession.h>
#include <pxcsensemanager.h>
#include <pxccapture.h>
#include <pxctouchlesscontroller.h>
#include "ovrrealsense.h"

#define IFCERR(b,msg) if(b){cerr<<msg<<endl;return;} 