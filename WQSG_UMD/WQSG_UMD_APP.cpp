/*  WQSG_UMD
*  Copyright (C) WQSG <wqsg@qq.com>
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/
// WQSG_UMD.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "WQSG_UMD_APP.h"
#include "WQSG_UMDDlg.h"
#include "consolecommand.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWQSG_UMDApp

BEGIN_MESSAGE_MAP(CWQSG_UMDApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CWQSG_UMDApp 构造

CWQSG_UMDApp::CWQSG_UMDApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CWQSG_UMDApp 对象

CWQSG_UMDApp theApp;


// CWQSG_UMDApp 初始化
int WQSG_Main( LPWSTR* szArglist , int nArgs )
{

	if(1)
	{

	}
	wprintf( L"暂时不支持命令行" );

	_putws( L"123暂时不支持命令行456" );
	return 0;
}

BOOL CWQSG_UMDApp::InitInstance()
{
	m_bCmdMode = FALSE;
	m_CmdR = 0;

	int nArgs = 0;
	LPWSTR* szArglist = CommandLineToArgvW( GetCommandLineW() , &nArgs );
	if( szArglist && nArgs > 1)
	{
		m_bCmdMode = TRUE;
		if (!ParseCommandParam(szArglist, nArgs))
			{m_CmdR = -1;}
	}
	else
	{
		// 如果一个运行在 Windows XP 上的应用程序清单指定要
		// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
		//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
		INITCOMMONCONTROLSEX InitCtrls;
		InitCtrls.dwSize = sizeof(InitCtrls);
		// 将它设置为包括所有要在应用程序中使用的
		// 公共控件类。
		InitCtrls.dwICC = ICC_WIN95_CLASSES;
		InitCommonControlsEx(&InitCtrls);

		CWinApp::InitInstance();

		// 标准初始化
		// 如果未使用这些功能并希望减小
		// 最终可执行文件的大小，则应移除下列
		// 不需要的特定初始化例程
		// 更改用于存储设置的注册表项
		// TODO: 应适当修改该字符串，
		// 例如修改为公司或组织名
		SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

		CWQSG_UMDDlg dlg;
		m_pMainWnd = &dlg;
		INT_PTR nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
			// TODO: 在此放置处理何时用
			//  “确定”来关闭对话框的代码
		}
		else if (nResponse == IDCANCEL)
		{
			// TODO: 在此放置处理何时用
			//  “取消”来关闭对话框的代码
		}
	}
	LocalFree( szArglist );
	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}


int CWQSG_UMDApp::ExitInstance()
{
	return m_bCmdMode ? (CWinApp::ExitInstance(), m_CmdR) : CWinApp::ExitInstance();
}