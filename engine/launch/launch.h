#ifndef START_H
#define START_H

#include "windows.h"
#include <stdio.h>
#include "..\R3Renderer\R3Renderer.h"

#ifndef F_RETURN
#define F_RETURN(x)    { if( FAILED(x) ) { return x; } }
#endif

HRESULT ProgramStartup(UINT);
HRESULT ProgramCleanup();
HRESULT GUIInit();
void CALLBACK GUIEvent(UINT nEvent, int nControlID);
#endif