// SmartLoggerMessage.h : SmartLoggerMessage.DLL のメイン ヘッダー ファイル
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CSmartLoggerMessageApp
// このクラスの実装に関しては SmartLoggerMessage.cpp を参照してください。
//

class CSmartLoggerMessageApp : public CWinApp
{
public:
	CSmartLoggerMessageApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
