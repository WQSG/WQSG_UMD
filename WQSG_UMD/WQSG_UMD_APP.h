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
// WQSG_UMD.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CWQSG_UMDApp:
// 有关此类的实现，请参阅 WQSG_UMD.cpp
//

class CWQSG_UMDApp : public CWinApp
{
	BOOL m_bCmdMode;
	int m_CmdR;
public:
	CWQSG_UMDApp();

// 重写
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CWQSG_UMDApp theApp;