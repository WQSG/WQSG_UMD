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
// WQSG_UMDDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
// CWQSG_UMDDlg 对话框
class CWQSG_UMDDlg : public CDialog
{
// 构造
	CISO_App m_umd;
	CMenu m_menu;
	s32 m_oldOffset;
public:
	CWQSG_UMDDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_WQSG_UMD_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
protected:
	virtual void OnCancel();
	virtual void OnOK();
private:
	CImageList m_imageList;
	CListCtrl m_cFileList;
	CStringA m_path;
	bool OpenISO( CStringW ISO_PathName , const BOOL bCanWrite );
	void CloseISO();
	void UpDataGUI();
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnLvnItemActivateListFile(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnNMRClickListFile(NMHDR *pNMHDR, LRESULT *pResult);
private:
	CString m_LastSelDir;
public:
	afx_msg void OnSavefile();
	afx_msg void On32774_替换文件();
	afx_msg void On32776_写文件偏移();
private:
	CString m_pathW;
};
