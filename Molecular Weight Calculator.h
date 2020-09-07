// Molecular Weight Calculator.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CMolecularWeightCalculatorApp:
// See Molecular Weight Calculator.cpp for the implementation of this class
//

class CMolecularWeightCalculatorApp : public CWinApp
{
public:
	CMolecularWeightCalculatorApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CMolecularWeightCalculatorApp theApp;