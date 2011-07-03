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
//

#include "stdafx.h"
#include "WQSG_UMD_APP.h"
#include "SelLang.h"


// CSelLang 对话框

IMPLEMENT_DYNAMIC(CSelLang, CDialog)

CSelLang::CSelLang( LCID a_Sel , std::vector<LCID>* a_pList , CWnd* pParent /*=NULL*/)
	: CDialog(CSelLang::IDD, pParent)
	, m_pList(a_pList)
	, m_OldSel(a_Sel)
	, m_Sel(a_Sel)
{
	ASSERT(a_pList);
}

CSelLang::~CSelLang()
{
}

void CSelLang::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_cList);
}


BEGIN_MESSAGE_MAP(CSelLang, CDialog)
	ON_BN_CLICKED(IDOK, &CSelLang::OnBnClickedOk)
END_MESSAGE_MAP()


// CSelLang 消息处理程序

void CSelLang::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialog::OnCancel();
}

void CSelLang::OnOK()
{
}

BOOL CSelLang::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//m_cList.SetExtendedStyle( m_cList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	m_cList.InsertColumn( 0 , L"" , 0 , 100 );
	m_cList.InsertColumn( 1 , L"" , 0 , 200 );
	m_cList.InsertColumn( 2 , L"" , 0 , 50 );

	m_cList.SetRedraw( FALSE );
	for ( std::vector<LCID>::iterator it = m_pList->begin() ;
		it != m_pList->end() ; ++it )
	{
		const LCID lcid = *it;

		WCHAR buf[1024] = {};
		if( !GetLocaleInfo( lcid , LOCALE_SNATIVELANGNAME , buf , 1024 ) )
		{
			ASSERT(0);
			continue;
		}

		const int iIndex = m_cList.InsertItem( m_cList.GetItemCount() , buf );
		if( iIndex == -1 )
		{
			ASSERT(0);
			EndDialog( IDCANCEL );
			return FALSE;
		}

		if( lcid == m_OldSel )
		{
			m_cList.SetItemState( iIndex , LVIS_SELECTED , LVIS_SELECTED );
		}

		m_cList.SetItemData( iIndex , lcid );

		if( WQSG_LCIDToLocaleName( lcid , buf , 1024 ) )
			m_cList.SetItemText( iIndex , 2 , buf );

		if( GetLocaleInfo( lcid , LOCALE_SENGLANGUAGE , buf , 1024 ) )
		{
			m_cList.SetItemText( iIndex , 1 , buf );
		}
	}
	m_cList.SetRedraw( TRUE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSelLang::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos = m_cList.GetFirstSelectedItemPosition();
	const int iSel = m_cList.GetNextSelectedItem( pos );
	if( iSel == -1 )
	{
		MessageBox( L"Select a language." );
		return;
	}

	m_Sel = m_cList.GetItemData( iSel );
	EndDialog(IDOK);
}
