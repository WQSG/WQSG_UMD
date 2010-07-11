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
#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CSelLang 对话框

class CSelLang : public CDialog
{
	DECLARE_DYNAMIC(CSelLang)

	std::vector<LCID>* m_pList;
	const LCID m_OldSel;
	LCID m_Sel;
public:
	CSelLang( LCID a_Sel , std::vector<LCID>* a_pList , CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSelLang();

// 对话框数据
	enum { IDD = IDD_DLG_SEL_LANG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void OnCancel();
	virtual void OnOK();
public:
	virtual BOOL OnInitDialog();
private:
	CListCtrl m_cList;
public:
	afx_msg void OnBnClickedOk();

	LCID GetLcid()const
	{
		return m_Sel;
	}
};
