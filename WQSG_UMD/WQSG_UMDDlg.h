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
#include "Control\CListCtrl_SortItems.h"
#include <map>

#define WQSG_PROCESS	1

struct SMenuData;
// CWQSG_UMDDlg 对话框
class CWQSG_UMDDlg : public CDialog
{
	const static int ms_Max_Pop = 4;
// 构造
	
	CMenu m_menu;

public:
	CISO_App m_umd;
	CWQSG_StringMgr m_StringMgr;
	s32 m_oldOffset;
	CString m_LastSelDir;
	CString m_strInfo;
	CString m_pathW;
	CStringA m_path;

protected:
	struct SLang
	{
		LCID m_lcid;
		CStringW m_strName;
		std::vector<WCHAR*> m_vIsoBaseLang;
		std::vector<WCHAR*> m_vIsoAppLang;
		std::vector<WCHAR*> m_vThisLang;
	};

	LCID m_SelLcid;
	std::map<LCID,SLang> m_Langs;
	std::vector<LCID> m_lcids;

	struct SMenuDataList
	{
		const SMenuData* m_pData;
		size_t m_Count;
	};

	CMenu m_PopMenu[ms_Max_Pop];
	static const SMenuDataList ms_MenuDataList[ms_Max_Pop];

	std::map<WORD,const SMenuData*> m_MenuFun;
public:
	CWQSG_UMDDlg(CWnd* pParent = NULL);	// 标准构造函数

	virtual ~CWQSG_UMDDlg()
	{
		std::map<LCID,SLang>::iterator it = m_Langs.begin();
		for( ; it != m_Langs.end() ; ++it )
		{
			SLang& lang = it->second;

			DeleteLang( lang.m_vIsoBaseLang );
			DeleteLang( lang.m_vIsoAppLang );
			DeleteLang( lang.m_vThisLang );
		}
	}
// 对话框数据
	enum { IDD = IDD_WQSG_UMD_DIALOG };

	void WorkingFinish()
	{
		m_bWorking = false;
		m_cProcess.ShowWindow(SW_HIDE);
		KillTimer(WQSG_PROCESS);
		m_cFileList.EnableWindow(TRUE);
	}

	void WorkingStart()
	{
		m_bWorking = true;
		m_cProcess.SetPos(0);
		m_cProcess.SetStep(10);
		m_cProcess.SetRange32(0, 100);
		m_cProcess.ModifyStyle(NULL, PBM_SETMARQUEE);
		m_cProcess.ShowWindow(SW_SHOW);
		SetTimer(WQSG_PROCESS, 50, NULL);
		m_cFileList.EnableWindow(FALSE);
	}

	int MsgBoxError(LPCWSTR lpString)
	{
		return ::MessageBox(m_hWnd, lpString, L"错误", MB_ICONERROR | MB_OK | MB_SETFOREGROUND);
	}
	int MsgBoxInfo(LPCWSTR lpString)
	{
		return ::MessageBox(m_hWnd, lpString, L"提示", MB_ICONINFORMATION | MB_OK | MB_SETFOREGROUND);
	}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	bool m_bWorking;
	bool IsWorking();
	

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	virtual void OnCancel();
	virtual void OnOK();

public:
	CImageList m_imageList;
	CListCtrl_SortItems m_cFileList;
	CProgressCtrl m_cProcess;

	
	bool OpenISO( CStringW ISO_PathName , const BOOL bCanWrite );
	void CloseISO();
	void UpDataGUI(bool bNewThread = true);
	void UpDataLbaData(bool bNewThread = true);

	u32 m_uMaxFreeBlock ;
	u32 m_uFreeLbaCount ;
	u32 m_uFreeBlockCount;
	
public:
	afx_msg void OnClose();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnLvnItemActivateListFile(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnNMRClickListFile(NMHDR *pNMHDR, LRESULT *pResult);

public:
	afx_msg void OnExportFiles();
	afx_msg void OnReplaceFile();
	afx_msg void OnWriteFile();
	afx_msg void OnCreateDir();
	afx_msg void OnAddFile();

	afx_msg void OnPopMenu(UINT a_nID);
	afx_msg void OnBnClickedButtonAbout();

	
private:
	bool LoadLang( const CString& a_strFile , SLang& a_Lang );
	void DeleteLang( std::vector<WCHAR*>& a_vList );
	bool FindLang();

public:
	afx_msg void OnBnClickedButtonExpand_ISO();
	afx_msg void OnBnClickedButtonApply_WIFP();
	afx_msg void OnBnClickedButtonCreate_WIFP();
	afx_msg void OnBnClickedButtonLang();
	void UiClose(void);
	void UiOpenR(void);
	void UiOpenRW(void);
	void SetTitle(BOOL* a_bCanWrite);

	void UseLang( LCID a_lcid );

	BOOL InitPopMenu( CMenu& a_Menu , const SMenuData* a_pMenuData , size_t a_Count , WORD a_Id );
};
