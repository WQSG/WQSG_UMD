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
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展





#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT









#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


#include<WQSG_lib.h>
#include<ISO/ISO_App.h>

class CWQSGFileDialog : public CFileDialog
{
	CString m_strFolderPath;
public:
	explicit CWQSGFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL,
		DWORD dwSize = 0,
		BOOL bVistaStyle = TRUE)
		: CFileDialog( bOpenFileDialog,lpszDefExt,lpszFileName,dwFlags,lpszFilter,pParentWnd,dwSize,bVistaStyle)
	{

	}

	virtual ~CWQSGFileDialog(){}

	CString GetFolderPath() const
	{
		CString strResult;
		if (m_bVistaStyle == TRUE)
			strResult = CFileDialog::GetFolderPath();
		else
		{
			strResult = m_strFolderPath;
		}
		return strResult;
	}

	virtual BOOL OnFileNameOK()
	{
		if (!m_bVistaStyle)
		{
			CString strResult;
			ASSERT(::IsWindow(m_hWnd));
			ASSERT(m_ofn.Flags & OFN_EXPLORER);

			if (GetParent()->SendMessage(CDM_GETFOLDERPATH, (WPARAM)MAX_PATH, (LPARAM)strResult.GetBuffer(MAX_PATH)) < 0)
				strResult.Empty();
			else
				strResult.ReleaseBuffer();

			m_strFolderPath = strResult;
		}

		return CFileDialog::OnFileNameOK();
	}
};

#define DEF_WQSG_IsoPatch_Head_Magic "WQSG-IsoFilePack"

#pragma pack(push,1)
struct SWQSG_IsoPatch_Head
{
	u8 m_Magic[16];//DEF_WQSG_IsoPatch_Head_Magic "WQSG-IsoFilePack"
	u8 m_Ver[4];
	n32 m_nFileCount;
	n64 m_nSize;
};

struct SWQSG_IsoPatch_Block
{
	u16 m_uSize;
	u32 m_uCrc32;
	char m_PathName[256];
	char m_FileName[256];
	u32 m_uFileSize;
	u32 m_uOldFileCrc32;
	SIsoTime m_time;
};

#pragma pack(pop)
