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
// stdafx.cpp : 只包括标准包含文件的源文件
// WQSG_UMD.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"


#include<ISO/ISO_cpp.h>

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
		return m_strFolderPath;
	}

	virtual BOOL OnFileNameOK()
	{
		POSITION pos = GetStartPosition( );
		m_strFolderPath = GetNextPathName( pos );

		::PathRemoveFileSpecW(m_strFolderPath.GetBuffer());
		m_strFolderPath.ReleaseBuffer();

		return CFileDialog::OnFileNameOK();
	}
};


CWQSGFileDialog_Save::~CWQSGFileDialog_Save()
{
	delete m_pDlg;
	m_pDlg = NULL;
}

INT_PTR CWQSGFileDialog_Save::DoModal()
{
	if( m_pDlg )
	{
		delete m_pDlg;
		m_pDlg = NULL;
	}

	m_pDlg = new CWQSGFileDialog( FALSE , m_strDefExt , m_strFileName , OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT , m_strFilter );

	if( !m_strWindowTitle.IsEmpty() )
		m_pDlg->m_ofn.lpstrTitle = m_strWindowTitle;

	const INT_PTR rt = m_pDlg->DoModal();

	if( rt == IDOK )
	{
		m_strFileName = m_pDlg->GetFileName();
		m_strFolderPath = m_pDlg->GetFolderPath();
	}

	return rt;
}


CString CWQSGFileDialog_Save::GetPathName()const
{
	return m_pDlg?m_pDlg->GetPathName():CString();
}

CWQSGFileDialog_Open::~CWQSGFileDialog_Open()
{
	delete m_pDlg;
	m_pDlg = NULL;
}

INT_PTR CWQSGFileDialog_Open::DoModal()
{
	if( m_pDlg )
	{
		delete m_pDlg;
		m_pDlg = NULL;
	}

	m_pDlg = new CWQSGFileDialog( TRUE , NULL , m_strFileName , OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT , m_strFilter );

	if( !m_strWindowTitle.IsEmpty() )
		m_pDlg->m_ofn.lpstrTitle = m_strWindowTitle;

	const INT_PTR rt = m_pDlg->DoModal();

	if( rt == IDOK )
	{
		m_strFileName = m_pDlg->GetFileName();
		m_strFolderPath = m_pDlg->GetFolderPath();
	}

	return rt;
}

CString CWQSGFileDialog_Open::GetPathName()const
{
	return m_pDlg?m_pDlg->GetPathName():CString();
}

CWQSGFileDialog_OpenS::~CWQSGFileDialog_OpenS()
{
	delete m_pDlg;
	m_pDlg = NULL;

	m_strNameBuffer.ReleaseBuffer();
}

INT_PTR CWQSGFileDialog_OpenS::DoModal()
{
	if( m_pDlg )
	{
		delete m_pDlg;
		m_pDlg = NULL;
	}

	m_pDlg = new CWQSGFileDialog( TRUE , NULL , _T("") , OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT , m_strFilter );
	m_pDlg->m_ofn.lpstrFile = m_strNameBuffer.GetBuffer();
	m_pDlg->m_ofn.nMaxFile = 65535;

	if( !m_strWindowTitle.IsEmpty() )
		m_pDlg->m_ofn.lpstrTitle = m_strWindowTitle;

	const INT_PTR rt = m_pDlg->DoModal();

	if( rt == IDOK )
	{
		m_strFolderPath = m_pDlg->GetFolderPath();
		m_strFolderPath += L'\\';
	}

	return rt;
}

POSITION CWQSGFileDialog_OpenS::GetStartPosition( ) const
{
	if( m_pDlg )
		return m_pDlg->GetStartPosition();

	return NULL;
}

BOOL CWQSGFileDialog_OpenS::GetNextPathName( CString& a_strPathName , POSITION& pos) const
{
	if( m_pDlg && pos )
	{
		a_strPathName = m_pDlg->GetNextPathName( pos );
		return TRUE;
	}
	return FALSE;
}
