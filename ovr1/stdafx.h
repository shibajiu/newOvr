// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here

//self headers
#include "glitems.h"
//realsense includes
#include <pxcsession.h>
#include <pxcsensemanager.h>
#include <pxccapture.h>
#include <pxctouchlesscontroller.h>

#define IFCERR(b,msg) if(b){cerr<<msg<<endl;return;} 