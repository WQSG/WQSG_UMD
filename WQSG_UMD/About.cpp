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
// About.cpp : 实现文件
//

#include "stdafx.h"
#include "About.h"


// CAbout 对话框

IMPLEMENT_DYNAMIC(CAbout, CDialog)

CAbout::CAbout(CWnd* pParent /*=NULL*/)
	: CDialog(CAbout::IDD, pParent)
{

}

CAbout::~CAbout()
{
}

void CAbout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAbout, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK1, &CAbout::OnNMClickSyslink1)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK2, &CAbout::OnNMClickSyslink2)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK3, &CAbout::OnNMClickSyslink3)
END_MESSAGE_MAP()


// CAbout 消息处理程序

void CAbout::OnNMClickSyslink1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	ShellExecute( NULL , L"open" , L"http://code.google.com/p/wqsg-umd" , NULL , NULL , SW_SHOW );

	*pResult = 0;
}

void CAbout::OnNMClickSyslink2(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	ShellExecute( NULL , L"open" , L"http://code.google.com/p/wqsglib" , NULL , NULL , SW_SHOW );

	*pResult = 0;
}

BOOL CAbout::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CString str;
	str.LoadString( IDS_APP_NAME );
	SetDlgItemText( IDC_APP_NAME , str );

	str.LoadString( IDS_APP_VER );
	SetDlgItemText( IDC_APP_VER , L"v" + str );

	CString str2;
	str.LoadString( IDS_APP_AUTHOR );
	str2.LoadString( IDS_APP_AUTHOR2 );

	SetDlgItemText( IDC_EDIT1 , str2 + L"(" + str + L")" + L"\r\nKid" );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CAbout::OnNMClickSyslink3(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	ShellExecute( NULL , L"open" , L"http://wqsg.ys168.com" , NULL , NULL , SW_SHOW );

	*pResult = 0;
}
