
// Kinect2Tools.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// Kinect2Tools:
// See Kinect2Tools.cpp for the implementation of this class
//

class Kinect2Tools : public CWinApp
{
public:
	Kinect2Tools();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern Kinect2Tools theApp;