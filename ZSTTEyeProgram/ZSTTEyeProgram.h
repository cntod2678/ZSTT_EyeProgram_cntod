
// ZSTTEyeProgram.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once
#include "stdafx.h"

#ifndef __AFXWIN_H__
	//#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CZSTTEyeProgramApp:
// �� Ŭ������ ������ ���ؼ��� ZSTTEyeProgram.cpp�� �����Ͻʽÿ�.
//

class CZSTTEyeProgramApp : public CWinApp
{
public:
	CZSTTEyeProgramApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CZSTTEyeProgramApp theApp;