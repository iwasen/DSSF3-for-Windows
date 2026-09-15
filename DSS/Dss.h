// Dss.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// メイン シンボル
#include "comres.h"
#include "DssData.h"

// CDssApp:
// このクラスの実装については、Dss.cpp を参照してください。
//

class CDssApp : public CWinApp
{
public:
	virtual BOOL InitInstance();

	void OnOnlineUpdate();

	DECLARE_MESSAGE_MAP()
};

extern CDssApp theApp;
