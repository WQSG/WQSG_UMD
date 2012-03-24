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
// WQSG_UMDDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WQSG_UMD_APP.h"
#include "WQSG_UMDDlg.h"

#include "WQSG_UMDDlg_Lang.h"
#include "SelLang.h"
#include <vector>

#define WM_WQSG_UPDATE (WM_USER + 0x10)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
struct
{
	const WCHAR*		name;
	const int			w;
}WQSG_FileList[] = {
	{	g_CWQSG_UMDDlg_String[23]	,	300	},
	{	g_CWQSG_UMDDlg_String[24]	,	80	},
	{	g_CWQSG_UMDDlg_String[25]	,	100	},
};

typedef void (AFX_MSG_CALL CWQSG_UMDDlg::*TMenuFun)();
struct SMenuData
{
	size_t m_LangId;
	TMenuFun m_pFn;
	UINT m_uFlags;
};

struct SFileData
{
	//int iIndex;
	bool bDir;
	CStringW szName;
	unsigned long lba;
	unsigned long size;
};

struct SThreadData
{
	CWQSG_UMDDlg *pDlg;
	//std::string 我们使用标准库
	std::string szData1;
	std::string szData2;
	std::wstring szWideData1;
	std::vector<std::wstring> vecData;
};

struct SThreadWIFPData
{
	CWQSG_UMDDlg *pDlg;
	std::wstring szInPath;
	std::wstring szOutPath;
	BOOL bCheckCrc32;
};

const SMenuData g_MenuData0[] = {
	{ 1 , &CWQSG_UMDDlg::OnExportFiles , MF_STRING },
	{ 0 , NULL , MF_SEPARATOR },
	{ 2 , &CWQSG_UMDDlg::OnCreateDir , MF_STRING },
	{ 3 , &CWQSG_UMDDlg::OnAddFile , MF_STRING },//L"添加文件" 
	{ 0 , NULL , MF_SEPARATOR },
	{ 4 , &CWQSG_UMDDlg::OnReplaceFile , MF_STRING },
	{ 5 , &CWQSG_UMDDlg::OnWriteFile , MF_STRING },
};
//MF_GRAYED
const SMenuData g_MenuData1[] = {
	{ 1 , &CWQSG_UMDDlg::OnExportFiles , MF_STRING },
	{ 0 , NULL , MF_SEPARATOR },
	{ 2 , &CWQSG_UMDDlg::OnCreateDir , MF_STRING },
	{ 3 , &CWQSG_UMDDlg::OnAddFile , MF_STRING },//L"添加文件" 
	{ 0 , NULL , MF_SEPARATOR | MF_DISABLED},
	{ 4 , NULL , MF_STRING | MF_DISABLED | MF_GRAYED },
	{ 5 , NULL , MF_STRING | MF_DISABLED | MF_GRAYED },
};

const SMenuData g_MenuData2[] = {
	{ 1 , &CWQSG_UMDDlg::OnExportFiles , MF_STRING },
	{ 0 , NULL , MF_SEPARATOR },
	{ 2 , &CWQSG_UMDDlg::OnCreateDir , MF_STRING },
	{ 3 , &CWQSG_UMDDlg::OnAddFile , MF_STRING },
	{ 0 , NULL , MF_SEPARATOR | MF_DISABLED },
	{ 4 , NULL , MF_STRING | MF_DISABLED | MF_GRAYED },
	{ 5 , NULL , MF_STRING | MF_DISABLED | MF_GRAYED },
};

const SMenuData g_MenuData3[] = {
	{ 1 , &CWQSG_UMDDlg::OnExportFiles , MF_STRING | MF_GRAYED },
	{ 0 , NULL , MF_SEPARATOR },
	{ 2 , &CWQSG_UMDDlg::OnCreateDir , MF_STRING },
	{ 3 , &CWQSG_UMDDlg::OnAddFile , MF_STRING },//L"添加文件" 
	{ 0 , NULL , MF_SEPARATOR },
	{ 4 , &CWQSG_UMDDlg::OnReplaceFile , MF_STRING | MF_GRAYED },
	{ 5 , &CWQSG_UMDDlg::OnWriteFile , MF_STRING | MF_GRAYED },
};

#define DEF_MAKE_ITEM( __defx ) { __defx , sizeof(__defx)/sizeof(*__defx) },
const CWQSG_UMDDlg::SMenuDataList CWQSG_UMDDlg::ms_MenuDataList[CWQSG_UMDDlg::ms_Max_Pop] = {
	DEF_MAKE_ITEM( g_MenuData0 )
	DEF_MAKE_ITEM( g_MenuData1 )
	DEF_MAKE_ITEM( g_MenuData2 )
	DEF_MAKE_ITEM( g_MenuData3 )
};
#undef DEF_MAKE_ITEM

// CWQSG_UMDDlg 对话框
CWQSG_UMDDlg::CWQSG_UMDDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWQSG_UMDDlg::IDD, pParent)
	, m_oldOffset(0)
	, m_pathW(_T(""))
	, m_strInfo(_T(""))
	, m_StringMgr( g_CWQSG_UMDDlg_String , sizeof(g_CWQSG_UMDDlg_String)/sizeof(*g_CWQSG_UMDDlg_String) )
	, m_SelLcid(0)
	, m_bWorking(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWQSG_UMDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FILE, m_cFileList);
	DDX_Text(pDX, IDC_EDIT1, m_pathW);
	DDX_Text(pDX, IDC_EDIT2, m_strInfo);
	DDX_Control(pDX, IDC_PROGRESS_INFO, m_cProcess);
}

BOOL CWQSG_UMDDlg::InitPopMenu( CMenu& a_Menu , const SMenuData* a_pMenuData , size_t a_Count , WORD a_Id )
{
	if( !a_Menu.CreatePopupMenu() )
		goto __gtErr;

	for( ; a_Count > 0 ; ++a_pMenuData , ++a_Id , --a_Count )
	{
		if( m_MenuFun.find(a_Id) != m_MenuFun.end() )
		{
			ASSERT( m_MenuFun.find(a_Id) == m_MenuFun.end() );
			goto __gtErr;
		}

		if( !a_Menu.AppendMenuW( a_pMenuData->m_uFlags , a_Id , m_StringMgr.GetString(a_pMenuData->m_LangId) ) )
			goto __gtErr;

		m_MenuFun[a_Id] = a_pMenuData;
	}

	if( !m_menu.AppendMenu( MF_STRING | MF_BYPOSITION | MF_POPUP , (UINT_PTR)a_Menu.GetSafeHmenu() , L"?" ) )
		goto __gtErr;

	return TRUE;
__gtErr:
	MsgBoxError(L"Init menu error!");
	return FALSE;
}

BEGIN_MESSAGE_MAP(CWQSG_UMDDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CWQSG_UMDDlg::OnBnClickedButtonOpen)
	ON_NOTIFY(LVN_ITEMACTIVATE, IDC_LIST_FILE, &CWQSG_UMDDlg::OnLvnItemActivateListFile)
	ON_BN_CLICKED(IDC_BUTTON_UP, &CWQSG_UMDDlg::OnBnClickedButtonUp)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_FILE, &CWQSG_UMDDlg::OnNMRClickListFile)
	ON_BN_CLICKED(IDC_BUTTON_About, &CWQSG_UMDDlg::OnBnClickedButtonAbout)
	ON_BN_CLICKED(IDC_BUTTON_Expand_ISO, &CWQSG_UMDDlg::OnBnClickedButtonExpand_ISO)
	ON_BN_CLICKED(IDC_BUTTON_Apply_WIFP, &CWQSG_UMDDlg::OnBnClickedButtonApply_WIFP)
	ON_BN_CLICKED(IDC_BUTTON_Create_WIFP, &CWQSG_UMDDlg::OnBnClickedButtonCreate_WIFP)
	ON_COMMAND_RANGE( 2000 , 3000 , &CWQSG_UMDDlg::OnPopMenu)
	ON_BN_CLICKED(IDC_BUTTON_LANG, &CWQSG_UMDDlg::OnBnClickedButtonLang)
END_MESSAGE_MAP()// CWQSG_UMDDlg 消息处理程序

BOOL CWQSG_UMDDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	FindLang();
	UseLang( GetThreadLocale() );
	// TODO: 在此添加额外的初始化代码
	UiClose();
	//m_cFileList.SetExtendedStyle( m_cFileList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	m_cFileList.AddNumColumn(1);
	m_cFileList.AddNumColumn(2);

	m_cProcess.ShowWindow(SW_HIDE);

	const int count = sizeof(WQSG_FileList)/sizeof(*WQSG_FileList);
	for( int i = 0 ; (i >= 0) && (i < count) ; ++i )
	{
		if( i != m_cFileList.InsertColumn( i , WQSG_FileList[i].name , 0 , WQSG_FileList[i].w ) )
		{
			MsgBoxError(L"Filelist init error!");
			EndDialog( IDCANCEL );
			return FALSE;
		}
	}
	{
		HIMAGELIST hi = NULL;
		SHFILEINFO shfi;

		memset(&shfi, 0 ,sizeof(shfi));
		hi = (HIMAGELIST)SHGetFileInfoW( L"TESTICON" , FILE_ATTRIBUTE_DIRECTORY , &shfi , sizeof(shfi) ,
			SHGFI_SYSICONINDEX | SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES );

		if( !hi )
			ASSERT( FALSE );

		IMAGEINFO info = {0};
		if( !CImageList::FromHandle(hi)->GetImageInfo( 0 , &info ) ||
			!m_imageList.Create( info.rcImage.right - info.rcImage.left , info.rcImage.bottom - info.rcImage.top , ILC_COLOR32 | ILC_MASK, 16 , 16 ) )
			ASSERT( FALSE );

		m_imageList.SetBkColor( m_cFileList.GetBkColor() );

		m_imageList.Add( shfi.hIcon );

		memset( &shfi , 0 , sizeof(shfi) );
		if( !SHGetFileInfoW( L"TESTICON" , 0 , &shfi , sizeof(shfi) ,
			SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES ) )
			ASSERT( FALSE );

		m_imageList.Add( shfi.hIcon );
	}

	m_cFileList.SetImageList( &m_imageList , LVSIL_SMALL );
	//--------------------------------------------------------------
	if( !m_menu.CreateMenu() )
	{
		MsgBoxError(L"Init menu error!");
		EndDialog( IDCANCEL );
		return FALSE;
	}

	for( int i = 0 ; i < ms_Max_Pop ; ++i )
	{
		if( !InitPopMenu( m_PopMenu[i] , ms_MenuDataList[i].m_pData , ms_MenuDataList[i].m_Count , 2000 + (WORD)m_MenuFun.size() ) )
		{
			EndDialog( IDCANCEL );
			return FALSE;
		}
	}


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}
// 如果向对话框添加最小化按钮，则需要下面的代码来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，这将由框架自动完成。
void CWQSG_UMDDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CWQSG_UMDDlg::OnQueryDragIcon(){	return static_cast<HCURSOR>(m_hIcon);}
void CWQSG_UMDDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (!IsWorking())
	{
		CDialog::OnClose();
		EndDialog( IDCANCEL );
	}
}

void CWQSG_UMDDlg::OnCancel(){}
void CWQSG_UMDDlg::OnOK(){}
bool CWQSG_UMDDlg::OpenISO( CStringW ISO_PathName , const BOOL bCanWrite )
{
	if (IsWorking())
	{
		return false;
	}

	CloseISO();
	if( !m_umd.OpenISO( ISO_PathName , bCanWrite , E_WIT_UMD ) )
	{
		MsgBoxError(m_umd.GetErrStr());
		m_umd.CleanErrStr();
		return false;
	}
	m_path = L"";
	m_umd.IsCanWrite()?UiOpenRW():UiOpenR();

	UpDataLbaData();

	return true;
}

void CWQSG_UMDDlg::CloseISO()
{
	UiClose();
	m_umd.CloseISO();
	m_path = "";
	m_pathW = L"";
	UpDataGUI(false);
}

void CWQSG_UMDDlg::UpDataLbaData(bool bNewThread)
{
	if (bNewThread)
	{
		if (IsWorking())
		{
			return;
		}
	}

	m_umd.GetFreeInfo( &m_uMaxFreeBlock , &m_uFreeLbaCount , &m_uFreeBlockCount );
	UpDataGUI(bNewThread);
}

DWORD WINAPI UpDataGUIProc(LPVOID lParam)
{
	SThreadData *pData = (SThreadData *)lParam;
	if (!pData) return 0;

	CWQSG_UMDDlg *pDlg = pData->pDlg;

	pDlg->m_cFileList.DeleteAllItems();
	pDlg->m_strInfo = L"";
#if 1
	SIsoFileFind* handle;
	std::vector<SFileData> vecFiles;

	if( pDlg->m_umd.IsOpen() && (handle = pDlg->m_umd.FindIsoFile( pDlg->m_path )) )
	{
		u32 uFile = 0;
		u32 uDir = 0;
		SIsoFileData data;
		while( pDlg->m_umd.FindNextIsoFile( handle , data ) )
		{
			SFileData sfd;
			sfd.bDir = data.isDir;
			sfd.szName = data.name;
			if (data.isDir)
			{
				sfd.lba = 0;
				sfd.size = 0;
			} else {
				sfd.lba = data.lba;
				sfd.size = data.size;
			}
			vecFiles.push_back(sfd);
		}

		pDlg->m_umd.CloseFindIsoFile( handle );

		pDlg->m_cFileList.SetRedraw(FALSE);

		for (unsigned long i = 0; i < vecFiles.size(); i++)
		{
			if( vecFiles[i].bDir )
			{
				const int index = pDlg->m_cFileList.InsertItem(pDlg->m_cFileList.GetItemCount(), vecFiles[i].szName , ((vecFiles[i].bDir)?0:1) );
				if( index >= 0 )
				{
					uDir++;
					pDlg->m_cFileList.SetItemData( index , 0 );
				}
			}
		}

		for (unsigned long i = 0; i < vecFiles.size(); i++)
		{
			if( !vecFiles[i].bDir )
			{
				const int index = pDlg->m_cFileList.InsertItem(pDlg->m_cFileList.GetItemCount(), vecFiles[i].szName , ((vecFiles[i].bDir)?0:1) );
				if( index >= 0 )
				{
					uFile++;

					CString tmp;

					tmp.Format( L"%d" , vecFiles[i].lba );
					pDlg->m_cFileList.SetItemText( index , 1 , tmp );

					tmp.Format( L"%d" , vecFiles[i].size );
					pDlg->m_cFileList.SetItemText( index , 2 , tmp );

					pDlg->m_cFileList.SetItemData( index , 1 );
				}
			}
		}

		pDlg->m_cFileList.SetRedraw(TRUE);

		pDlg->m_strInfo.Format( pDlg->m_StringMgr.GetString(13) ,
			uDir , uFile , pDlg->m_umd.GetMaxLbaCount() , pDlg->m_uFreeLbaCount , pDlg->m_uFreeBlockCount , pDlg->m_uMaxFreeBlock );

		pDlg->m_pathW = pDlg->m_path;
	}

	pDlg->PostMessage(WM_WQSG_UPDATE, FALSE);
	//pDlg->m_cFileList.SetFocus();
#endif

	//pDlg->UpDataLbaData(false);
	pDlg->WorkingFinish();
	delete pData;

	return 0;
}

void CWQSG_UMDDlg::UpDataGUI(bool bNewThread)
{
	if (bNewThread)
	{
		if (IsWorking())
		{
			return;
		}

		SThreadData *pData = new SThreadData;
		pData->pDlg = this;

		WorkingStart();
		::CloseHandle(
			::CreateThread(NULL, 0, UpDataGUIProc, (LPVOID)pData, NULL, NULL));
	} else {
		SThreadData *pData = new SThreadData;
		pData->pDlg = this;

		UpDataGUIProc((LPVOID)pData);
	}
}

void CWQSG_UMDDlg::OnBnClickedButtonOpen()
{
	if (IsWorking())
	{
		return;
	}

	static CWQSGFileDialog_Open dlg( L"镜像文件(*.iso)|*.iso||" );

	CString name;
#if _DEBUG && 0
	name = "s:\\Luxor.iso";
#else
	if( IDOK != dlg.DoModal() )
		return;

	name = dlg.GetPathName();
#endif
	OpenISO( name , TRUE );
	// TODO: 在此添加控件通知处理程序代码
}

void CWQSG_UMDDlg::OnLvnItemActivateListFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	if (IsWorking())
	{
		return;
	}

	if( 0 != m_cFileList.GetItemData( pNMIA->iItem ) )
		return;

	CString strNameW( m_cFileList.GetItemText( pNMIA->iItem , 0 ) );

	CStringA strNameA;
	strNameA = strNameW;

	if( m_path.Right( 1 ) != '/' )
		m_path += '/';

	m_path += strNameA;

	UpDataGUI();
}


DWORD WINAPI ImportFileProc(LPVOID lParam)
{
	SThreadData *pData = (SThreadData *)lParam;
	if (!pData) return 0;
	CWQSG_UMDDlg *pDlg = pData->pDlg;

	BOOL bFileBreak;
	for(unsigned long i = 0; i < pData->vecData.size() ; i++)
	{
		if( !pDlg->m_umd.EasyImport( bFileBreak , pData->vecData[i].c_str() , pData->szData1.c_str() ) )
		{
			if( bFileBreak )
				pDlg->CloseISO();

			pDlg->MsgBoxError( pDlg->m_umd.GetErrStr() );
			pDlg->m_umd.CleanErrStr();
			break;
		}
	}

	pDlg->m_umd.Flush();
	pDlg->UpDataLbaData(false);
	pDlg->WorkingFinish();
	delete pData;

	return 0;
}



BOOL CWQSG_UMDDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if( WM_DROPFILES == pMsg->message )
	{
		const HDROP hDrop = (HDROP)pMsg->wParam;
		if( pMsg->hwnd == m_cFileList.m_hWnd )
		{
			if(m_umd.IsCanWrite() && !IsWorking())
			{
				WCHAR strPathName[MAX_PATH*2];

				const int fileCount = DragQueryFile( hDrop , (UINT)-1 , NULL , 0 );
				if (fileCount > 0)
				{
					SThreadData *pData = new SThreadData;
					pData->pDlg = this;

					for( int i = 0 ; (i>=0) && (i<fileCount) && ( DragQueryFile( hDrop , i , strPathName , MAX_PATH*2 ) != (UINT)-1 ) ; ++i )
					{
						pData->vecData.push_back(strPathName);
					}

					pData->szData1 = m_path;
					WorkingStart();
					::CloseHandle(
						::CreateThread(NULL, 0, ImportFileProc, (LPVOID)pData, NULL, NULL));
				}
			}
		}
		DragFinish( hDrop );
	} else if( WM_TIMER == pMsg->message && pMsg->wParam == WQSG_PROCESS){
		m_cProcess.StepIt();
	} else if (WM_WQSG_UPDATE == pMsg->message){
		UpdateData(pMsg->wParam);
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CWQSG_UMDDlg::OnBnClickedButtonUp()
{
	// TODO: 在此添加控件通知处理程序代码
	if (IsWorking())
	{
		return;
	}

	CStringA path( m_path );
	int pos = path.ReverseFind( L'/' );
	if( pos >= 0 )
		path = path.Left( pos );

	m_path = path;
	UpDataGUI();
}

void CWQSG_UMDDlg::OnNMRClickListFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	//------------------------------------
	CPoint pos( pNMItemActivate->ptAction );
	m_cFileList.ClientToScreen( &pos );

	const int count = m_cFileList.GetSelectedCount();

	if( count > 0 )
	{
		CMenu*const sub = m_menu.GetSubMenu( m_umd.IsCanWrite()?((count==1)?0:1):2 );
		if( sub )
			sub->TrackPopupMenu( TPM_LEFTALIGN /*|TPM_RIGHTBUTTON*/ , pos.x , pos.y , this );
	} else {
		if (m_umd.IsCanWrite())
		{
			CMenu*const sub = m_menu.GetSubMenu(3);
			if( sub )
				sub->TrackPopupMenu( TPM_LEFTALIGN /*|TPM_RIGHTBUTTON*/ , pos.x , pos.y , this );
		}
	}
}

DWORD WINAPI ExportFileProc(LPVOID lParam)
{
	SThreadData *pData = (SThreadData *)lParam;
	if (!pData) return 0;
	CWQSG_UMDDlg *pDlg = pData->pDlg;

	// TODO: 在此添加命令处理程序代码

	POSITION pos = pDlg->m_cFileList.GetFirstSelectedItemPosition();
	int index = 0;
	CStringA path = pDlg->m_path;
	if (path.Right(1) != '/')
		path.Append("/");
	while( (index = pDlg->m_cFileList.GetNextSelectedItem( pos )) >= 0 )
	{
		CString str( pDlg->m_cFileList.GetItemText( index , 0 ) );
		CStringA strA;	strA = str;

		SIsoFileData data;
		if( !pDlg->m_umd.GetFileData( data , pDlg->m_path , strA ) )
		{
			pDlg->MsgBoxError( pDlg->m_umd.GetErrStr() );
			pDlg->m_umd.CleanErrStr();
			break;
		}

		if( data.isDir )
		{
			if( !pDlg->m_umd.ExportDir( pDlg->m_LastSelDir + str , path + strA ) )
			{
				pDlg->MsgBoxError( pDlg->m_umd.GetErrStr() );
				pDlg->m_umd.CleanErrStr();
				break;
			}
		}
		else
		{
			if( !pDlg->m_umd.ExportFile( pDlg->m_LastSelDir + str , pDlg->m_path , strA ) )
			{
				pDlg->MsgBoxError( pDlg->m_umd.GetErrStr() );
				pDlg->m_umd.CleanErrStr();
				break;
			}
		}
	}

	pDlg->UpDataLbaData(false);
	pDlg->WorkingFinish();
	delete pData;

	return 0;
}


void CWQSG_UMDDlg::OnExportFiles()
{
	if (IsWorking())
	{
		return;
	}

	if (m_cFileList.GetSelectedCount() > 0)
	{
		CWQSG_DirDlg dlg( m_hWnd , m_StringMgr.GetString(20) , m_LastSelDir.GetString() );
		WCHAR outPath[MAX_PATH*2];
		if( !dlg.GetPath( outPath ) )
			return;

		m_LastSelDir = outPath ;
		if( m_LastSelDir.Right( 1 ) != L'\\' )
			m_LastSelDir += L'\\';

		SThreadData *pData = new SThreadData;
		pData->pDlg = this;
		pData->szWideData1 = m_LastSelDir;

		WorkingStart();
		::CloseHandle(
			::CreateThread(NULL, 0, ExportFileProc, (LPVOID)pData, NULL, NULL));
	}
}

DWORD WINAPI ReplaceFileProc(LPVOID lParam)
{
	SThreadData *pData = (SThreadData *)lParam;
	if (!pData) return 0;
	CWQSG_UMDDlg *pDlg = pData->pDlg;

	BOOL bFileBreak;
	if( !pDlg->m_umd.ImportFile( bFileBreak , pDlg->m_path , pData->szData1.c_str() , pData->szWideData1.c_str() ) )
	{
		if( bFileBreak )
			pDlg->CloseISO();
		pDlg->MessageBox( pData->pDlg->m_umd.GetErrStr() );
		pDlg->m_umd.CleanErrStr();
	}
	pDlg->m_umd.Flush();
	
	pDlg->UpDataLbaData(false);
	pDlg->WorkingFinish();
	delete pData;

	return 0;
}

DWORD WINAPI CreateDirProc(LPVOID lParam)
{
	SThreadData *pData = (SThreadData *)lParam;
	if (!pData) return 0;
	CWQSG_UMDDlg *pDlg = pData->pDlg;

	BOOL bFileBreak;
	if( !pDlg->m_umd.CreateDir( bFileBreak , pData->szData1.c_str(), pDlg->m_path) )
	{
		if( bFileBreak )
			pDlg->CloseISO();
		pDlg->MessageBox( pData->pDlg->m_umd.GetErrStr() );
		pDlg->m_umd.CleanErrStr();
	} else {
		CStringA strPath = pDlg->m_path;
		if (strPath.Right(1) != '/')
			strPath.Append("/");

		if( !pDlg->m_umd.ImportDir( bFileBreak , strPath + pData->szData1.c_str() , pData->szWideData1.c_str() ) )
		{
			if( bFileBreak )
				pDlg->CloseISO();
			pDlg->MessageBox( pData->pDlg->m_umd.GetErrStr() );
			pDlg->m_umd.CleanErrStr();
		}
	}

	pDlg->m_umd.Flush();
	
	pDlg->UpDataLbaData(false);
	pDlg->WorkingFinish();
	delete pData;

	return 0;
}

DWORD WINAPI CreateWIFPProc(LPVOID lParam)
{
	SThreadWIFPData *pData = (SThreadWIFPData *)lParam;
	if (!pData) return 0;
	CWQSG_UMDDlg *pDlg = pData->pDlg;

	CISO_App oldIso;
	CWQSG_File fp;

	if( !oldIso.OpenISO( pData->szInPath.c_str() , FALSE , E_WIT_UMD ) )
	{
		pDlg->MsgBoxError(L"打开原始ISO失败！");
		goto exit_l;
	}

	if( !fp.OpenFile( pData->szOutPath.c_str() , 4 , 3 ) )
	{
		pDlg->MsgBoxError(L"打开输出文件失败！");
		goto exit_l;
	}

	if( !pDlg->m_umd.MakeFilePackage( oldIso , fp , pData->bCheckCrc32 ) )
	{
		CString str;
		str.Format( L"创建补丁失败！\r\n%s" , pDlg->m_umd.GetErrStr() );
		pDlg->m_umd.CleanErrStr();
		pDlg->MsgBoxError(str);
		goto exit_l;
	}

	pDlg->MsgBoxInfo(L"成功创建补丁！");

exit_l:
	pDlg->WorkingFinish();
	delete pData;

	return 0;
}

DWORD WINAPI ApplyWIFPProc(LPVOID lParam)
{
	SThreadWIFPData *pData = (SThreadWIFPData *)lParam;
	if (!pData) return 0;
	CWQSG_UMDDlg *pDlg = pData->pDlg;

	CWQSG_File fp;
	BOOL bFileBreak = FALSE;

	if(!fp.OpenFile(pData->szInPath.c_str(), 1, 3))
	{
		pDlg->MsgBoxError(L"打开补丁文件失败！");
		goto exit_l;
	}

	if(pDlg->m_umd.ImportFilePackage(bFileBreak, fp, TRUE))
	{
		pDlg->m_umd.Flush();
		pDlg->MsgBoxInfo(L"成功应用补丁！");
	}
	else if(bFileBreak)
	{
		pDlg->CloseISO();
		CString str;
		str.Format( L"打补丁失败，ISO可能已损坏！\r\n\r\n失败原因:\r\n%s" , pDlg->m_umd.GetErrStr());
		pDlg->m_umd.CleanErrStr();
		pDlg->MsgBoxError(str);
	}
	else
	{
		CString str;
		str.Format(L"打补丁失败！\r\n\r\n失败原因:\r\n%s" , pDlg->m_umd.GetErrStr());
		pDlg->m_umd.CleanErrStr();
		pDlg->MsgBoxError(str);
	}

exit_l:
	pDlg->WorkingFinish();
	delete pData;

	return 0;
}

void CWQSG_UMDDlg::OnCreateDir()
{
	if (IsWorking())
	{
		return;
	}
	if( !m_umd.IsCanWrite() )
	{
		MessageBox( m_StringMgr.GetString(14) );
		return;
	}

	CWQSG_DirDlg dlg( m_hWnd , _T("选择导入文件夹") , m_LastSelDir.GetString() );
	WCHAR outPath[MAX_PATH*2];
	if(!dlg.GetPath( outPath ))
		return;
	CStringA strName;
	for (int i = lstrlenW(outPath) - 1; i > 0; i--)
	{
		if (outPath[i] == '\\' || outPath[i + 1] == '/')
		{
			strName = outPath + i + 1;
			break;
		}
	}
	SThreadData *pData = new SThreadData;
	pData->pDlg = this;
	pData->szData1 = (LPCSTR)strName.GetString();
	pData->szWideData1 = outPath;

	WorkingStart();
	::CloseHandle(
		::CreateThread(NULL, 0, CreateDirProc, (LPVOID)pData, NULL, NULL));
}

void CWQSG_UMDDlg::OnAddFile()
{
	if (IsWorking())
	{
		return;
	}

	if( !m_umd.IsCanWrite() )
	{
		MessageBox( m_StringMgr.GetString(14) );
		return;
	}

	static ::CWQSGFileDialog_Open dlg( L"所有文件(*.*)|*.*||" );
	dlg.SetWindowTitle( m_StringMgr.GetString(21) );
	if( IDOK != dlg.DoModal() )
		return;
	TCHAR strName[512];
	lstrcpy(strName, dlg.GetPathName());
	CStringA strA;
	for (int i = lstrlen(strName) - 1; i > 0; i--)
	{
		if (strName[i] == '\\' || strName[i] == '/')
		{
			strA = strName + i + 1;
			break;
		}
	}

	if (strA.GetLength() > 0)
	{
		SThreadData *pData = new SThreadData;
		pData->pDlg = this;
		pData->szData1 = (LPCSTR)strA.GetString();
		pData->szWideData1 = dlg.GetPathName();

		WorkingStart();
		::CloseHandle(
			::CreateThread(NULL, 0, ReplaceFileProc, (LPVOID)pData, NULL, NULL));
	}
}

void CWQSG_UMDDlg::OnReplaceFile()
{
	// TODO: 在此添加命令处理程序代码
	if (IsWorking())
	{
		return;
	}

	if( !m_umd.IsCanWrite() )
	{
		MessageBox( m_StringMgr.GetString(14) );
		return;
	}


	POSITION pos = m_cFileList.GetFirstSelectedItemPosition();
	const int index = m_cFileList.GetNextSelectedItem( pos );
	if( index >= 0 )
	{
		CString str(m_cFileList.GetItemText( index , 0 ) );

		static ::CWQSGFileDialog_Open dlg( L"所有文件(*.*)|*.*||" );
		dlg.SetWindowTitle( m_StringMgr.GetString(21) );
		CStringA strA;
		strA = str;
		if( IDOK != dlg.DoModal() )
			return;

		SThreadData *pData = new SThreadData;
		pData->pDlg = this;
		pData->szData1 = (LPCSTR)strA.GetString();
		pData->szWideData1 = dlg.GetPathName();

		WorkingStart();
		::CloseHandle(
			::CreateThread(NULL, 0, ReplaceFileProc, (LPVOID)pData, NULL, NULL));
		//ReplaceFileProc((LPVOID)pData);
	}

}

void CWQSG_UMDDlg::OnPopMenu(UINT a_nID )
{
	std::map<WORD,const SMenuData*>::iterator it = m_MenuFun.find(a_nID);
	if( it != m_MenuFun.end() )
	{
		const SMenuData* pData = it->second;
		if( pData->m_pFn )
		{
			(this->*pData->m_pFn)();
		}
	}
}

#include "InputBox.h"
DWORD WINAPI WriteFileProc(LPVOID lParam)
{
	SThreadData *pData = (SThreadData *)lParam;
	if (!pData) return 0;
	CWQSG_UMDDlg *pDlg = pData->pDlg;

	POSITION pos = pDlg->m_cFileList.GetFirstSelectedItemPosition();
	const int index = pDlg->m_cFileList.GetNextSelectedItem( pos );
	bool bWorked = false;
	if( index >= 0 )
	{
		
		CString name( pDlg->m_cFileList.GetItemText( index , 0 ) );
		CStringA nameA;	nameA = name;

		SIsoFileData data;
		if( !pDlg->m_umd.GetFileData( data , pDlg->m_path , nameA ) )
		{
			pDlg->MsgBoxError( pDlg->m_umd.GetErrStr() );
			pDlg->m_umd.CleanErrStr();
			goto exit_l;
		}

		static CWQSGFileDialog_Open dlg( L"所有文件(*.*)|*.*||" );
		dlg.SetWindowTitle( L"选择要导入的文件…" );

		if( IDOK != dlg.DoModal() )
			goto exit_l;

		CInputBox ibox( L"相对于文件的偏移" , L"输入相对于文件的偏移" , pDlg->m_oldOffset , data.size );
		if( IDOK != ibox.DoModal() )
			goto exit_l;

		pDlg->m_oldOffset = ibox.GetVal();

		BOOL bFileBreak;
		if( !pDlg->m_umd.WriteFile( bFileBreak , pDlg->m_path , nameA , pDlg->m_oldOffset , dlg.GetPathName() ) )
		{
			if( bFileBreak )
				pDlg->CloseISO();

			pDlg->MsgBoxError( pDlg->m_umd.GetErrStr() );
			pDlg->m_umd.CleanErrStr();
		}

		pDlg->m_umd.Flush();
		bWorked = true;
	}

exit_l:
	if (bWorked)
	{
		pDlg->UpDataLbaData(false);
	}
	pDlg->WorkingFinish();
	delete pData;


	return 0;
}

void CWQSG_UMDDlg::OnWriteFile()
{
	// TODO: 在此添加命令处理程序代码
	if (IsWorking())
	{
		return;
	}
	if( !m_umd.IsCanWrite() )
	{
		MessageBox( m_StringMgr.GetString(14) );
		return;
	}

	if (m_cFileList.GetSelectedCount() > 0)
	{
		SThreadData *pData = new SThreadData;
		pData->pDlg = this;

		WorkingStart();
		::CloseHandle(
			::CreateThread(NULL, 0, WriteFileProc, (LPVOID)pData, NULL, NULL));
	}
}

void CWQSG_UMDDlg::OnBnClickedButtonAbout()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strAppName;
	strAppName.LoadString( IDS_APP_NAME );

	CString strAppVer;
	strAppVer.LoadString( IDS_APP_VER );

	CString strAuthor1;
	strAuthor1.LoadString( IDS_APP_AUTHOR );

	CString strAuthor2;
	strAuthor2.LoadString( IDS_APP_AUTHOR2 );

	CString strSVN;
	strSVN.LoadString( IDS_APP_SVN );
	
	WQSG_About( m_hIcon , m_hWnd , m_StringMgr.GetString(10) , strAppName + L"\r\n " + strSVN + L" v" + strAppVer ,
		CStringW(m_StringMgr.GetString(15)) + L" : <a href=\"http://code.google.com/p/wqsg-umd\">http://code.google.com/p/wqsg-umd</a>\r\n"
		+ CStringW(m_StringMgr.GetString(16)) + L" : <a href=\"http://code.google.com/p/wqsglib\">http://code.google.com/p/wqsglib</a>\r\n"
								L"                 <a href=\"http://wqsg.ys168.com\">http://wqsg.ys168.com</a>\r\n" ,
		strAuthor2 + L"(" + strAuthor1 + L")" + L"\r\nKid\r\n腾袭");
}

void CWQSG_UMDDlg::OnBnClickedButtonExpand_ISO()
{
	// TODO: 在此添加控件通知处理程序代码
	if (IsWorking())
	{
		return;
	}

	if( !m_umd.IsCanWrite() )
		return;

	CString str;
	str.Format( m_StringMgr.GetString(17) , m_umd.GetPerLbaSize() );

	CInputBox ibox( m_StringMgr.GetString(9) , str , 0 , (n32)(((u32)-1)>>1) - m_umd.GetMaxLbaCount() );
	if( IDOK != ibox.DoModal() )
		return ;

	n32 lba = ibox.GetVal();

	if( lba > 0 )
	{
		const WCHAR* pMsg = m_umd.AddLbaCount( lba )?m_StringMgr.GetString(18):m_StringMgr.GetString(19);
		m_umd.Flush();

		MessageBox( pMsg );
		UpDataLbaData();
	}
}

void CWQSG_UMDDlg::OnBnClickedButtonApply_WIFP()
{
	// TODO: 在此添加控件通知处理程序代码
	if (IsWorking())
	{
		return;
	}

	UpdateData();

	if( !m_umd.IsOpen() )
		return;

	if( !m_umd.IsCanWrite() )
		return;

	static CWQSGFileDialog_Open dlg( L"补丁文件(*.wifp)|*.wifp||" );
	dlg.SetWindowTitle( L"选择补丁文件" );
	if( dlg.DoModal() != IDOK )
		return;

	SThreadWIFPData *pData = new SThreadWIFPData;
	pData->pDlg = this;
	pData->szInPath = dlg.GetPathName();

	WorkingStart();
	::CloseHandle(
		::CreateThread(NULL, 0, ApplyWIFPProc, (LPVOID)pData, NULL, NULL));

	/*CWQSG_File fp;
	if( !fp.OpenFile( dlg.GetPathName().GetString() , 1 , 3 ) )
	{
		return;
	}

	BOOL bFileBreak;
	if( m_umd.ImportFilePackage( bFileBreak , fp , TRUE ) )
	{
		m_umd.Flush();
		MessageBox( L"打补丁成功" );
	}
	else if( bFileBreak )
	{
		CloseISO();
		CString str;
		str.Format( L"打补丁失败,ISO可能已损坏\r\n\r\n失败原因:\r\n%s" , m_umd.GetErrStr() );
		m_umd.CleanErrStr();
		MessageBox( str );
	}
	else
	{
		CString str;
		str.Format( L"打补丁失败\r\n\r\n失败原因:\r\n%s" , m_umd.GetErrStr() );
		m_umd.CleanErrStr();
		MessageBox( str );
	}*/
}

void CWQSG_UMDDlg::OnBnClickedButtonCreate_WIFP()
{
	// TODO: 在此添加控件通知处理程序代码
	if (IsWorking())
	{
		return;
	}

	UpdateData();

	if( !m_umd.IsOpen() )
		return;

	static CWQSGFileDialog_Open dlg_iso( L"镜像文件(*.iso)|*.iso||" );
	dlg_iso.SetWindowTitle( L"选择原版ISO" );

	if( dlg_iso.DoModal() != IDOK )
		return;

	static CWQSGFileDialog_Save dlg_out( L"补丁文件(*.wifp)|*.wifp||" , L"wifp" );
	dlg_out.SetWindowTitle( L"补丁文件保存为" );

	if( dlg_out.DoModal() != IDOK )
		return;

	BOOL bCheckCrc32;
	switch( MessageBox( L"制作补丁时，是否要检验原版文件的CRC32" , NULL ,MB_YESNOCANCEL ) )
	{
	case IDYES:
		bCheckCrc32 = TRUE;
		break;
	case IDNO:
		bCheckCrc32 = FALSE;
		break;
	default:
		return;
	}

	SThreadWIFPData *pData = new SThreadWIFPData;
	pData->pDlg = this;
	pData->szInPath = dlg_iso.GetPathName();
	pData->szOutPath = dlg_out.GetPathName();
	pData->bCheckCrc32 = bCheckCrc32;

	WorkingStart();
	::CloseHandle(
		::CreateThread(NULL, 0, CreateWIFPProc, (LPVOID)pData, NULL, NULL));

	/*CISO_App oldIso;
	if( !oldIso.OpenISO( dlg_iso.GetPathName() , FALSE , E_WIT_UMD ) )
	{
		MessageBox( L"打开ISO失败" , dlg_iso.GetPathName() );
		return;
	}

	CWQSG_File fp;
	if( !fp.OpenFile( dlg_out.GetPathName() , 4 , 3 ) )
	{
		MessageBox( L"打开文件失败" , dlg_out.GetPathName() );
		return;
	}

	if( !m_umd.MakeFilePackage( oldIso , fp , bCheckCrc32 ) )
	{
		CString str;
		str.Format( L"创建补丁失败\r\n%s" , m_umd.GetErrStr() );
		m_umd.CleanErrStr();
		MessageBox( str , dlg_out.GetPathName() );
		return;
	}

	MessageBox( L"成功创建补丁" , dlg_out.GetPathName() );*/
}

#define DEF_EnableWindow( __def_ID , _def_Enable ) do{CWnd*pWnd = GetDlgItem(__def_ID);pWnd->EnableWindow(_def_Enable);}while(0)
void CWQSG_UMDDlg::UiClose(void)
{
	m_cFileList.EnableWindow( FALSE );
	DEF_EnableWindow(IDC_BUTTON_UP,FALSE);
	DEF_EnableWindow(IDC_BUTTON3,FALSE);
	DEF_EnableWindow(IDC_BUTTON4,FALSE);
	DEF_EnableWindow(IDC_BUTTON5,FALSE);

	SetTitle( NULL );
}

void CWQSG_UMDDlg::UiOpenR(void)
{
	m_cFileList.EnableWindow( TRUE );
	DEF_EnableWindow(IDC_BUTTON_UP,TRUE);
	DEF_EnableWindow(IDC_BUTTON3,FALSE);
	DEF_EnableWindow(IDC_BUTTON4,FALSE);
	DEF_EnableWindow(IDC_BUTTON5,TRUE);
	BOOL b = FALSE;
	SetTitle( &b );
}

void CWQSG_UMDDlg::UiOpenRW(void)
{
	m_cFileList.EnableWindow( TRUE );
	DEF_EnableWindow(IDC_BUTTON_UP,TRUE);
	DEF_EnableWindow(IDC_BUTTON3,TRUE);
	DEF_EnableWindow(IDC_BUTTON4,TRUE);
	DEF_EnableWindow(IDC_BUTTON5,TRUE);
	BOOL b = TRUE;
	SetTitle( &b );
}

void CWQSG_UMDDlg::SetTitle(BOOL* a_bCanWrite)
{
	CString str1,str2,str3;
	str1.LoadString( IDS_APP_NAME );
	str2.LoadString( IDS_APP_VER );
	str3.LoadString( IDS_APP_SVN );

	CString strTitle( str1 + L" " + str3 + L" v" + str2 ); 
	if( a_bCanWrite )
	{
		strTitle += L" ";

		strTitle += (*a_bCanWrite)?m_StringMgr.GetString(11):m_StringMgr.GetString(12);
	}

	SetWindowText( strTitle );
}

template<size_t TMin,size_t TMax>
bool AddStr( std::vector<WCHAR*>& a_vList , int a_iIndex , const CStringW& a_str )
{
	if( a_iIndex > TMin && a_iIndex < TMax )
	{
		const size_t index = a_iIndex - TMin;
		if( index < a_vList.size() )
		{
			if( a_vList[index] )
			{
				delete[]a_vList[index];
				a_vList[index] = NULL;
			}

			a_vList[index] = new WCHAR[a_str.GetLength()+1];

			WQSG_strcpy( a_str.GetString() , a_vList[index] );
			return true;
		}
	}
	return false;
}

bool CWQSG_UMDDlg::LoadLang( const CString& a_strFile , SLang& a_Lang )
{
	if( a_Lang.m_vIsoBaseLang.size() || a_Lang.m_vIsoAppLang.size() || a_Lang.m_vThisLang.size() )
		return false;

	a_Lang.m_vIsoBaseLang.resize( m_umd.Base_GetDefaultLangStringCount() , NULL );
	a_Lang.m_vIsoAppLang.resize( m_umd.GetDefaultLangStringCount() , NULL );
	a_Lang.m_vThisLang.resize( m_StringMgr.GetStringCount() , NULL );

	CMemTextW tp;
	if( !tp.Load( a_strFile.GetString() , 1024*1024 ) )
		return false;

	if( tp.GetCP() != E_CP_UTF8 )
		return false;

	while( const WCHAR* __line = tp.GetLine() )
	{
		CStringW line(__line);
		delete[]__line;

		line.TrimLeft();

		if( line.GetLength() == 0 )
			continue;

		if( line[0] == L'#' )
			continue;

		const int pos = line.Find( L"=" );
		if( pos < 1 )
			continue;

		CString strIndex = line.Left( pos );
		CString strLang = line.Mid( pos + 1 );

		strLang.Replace(L"\\r\\n", L"\r\n");
		strLang.Replace(L"\\\"", L"\"");

		const int iIndex = _wtoi( strIndex.GetString() );
		if( iIndex <= 0 )
			continue;

		if( !AddStr<0,10000>( a_Lang.m_vIsoBaseLang , iIndex , strLang ) )
			if( !AddStr<10000,20000>( a_Lang.m_vIsoAppLang , iIndex , strLang ) )
				AddStr<20000,30000>( a_Lang.m_vThisLang , iIndex , strLang );
	}

	return true;
}

void CWQSG_UMDDlg::DeleteLang( std::vector<WCHAR*>& a_vList )
{
	for( std::vector<WCHAR*>::iterator it = a_vList.begin() ;
		it != a_vList.end() ; ++it )
	{
		if( (*it) )
		{
			delete[](*it);
			(*it) = NULL;
		}
	}
}

bool CWQSG_UMDDlg::FindLang()
{
	CFileFind find;

	WCHAR exe[MAX_PATH*2];
	if( !WQSG_GetExePath( exe , MAX_PATH*2 ) ||
		!find.FindFile( CString( exe ) + L"\\lang\\*.lang" ) )
		return false;

	BOOL bNext;
	do
	{
		bNext = find.FindNextFile();

		const CStringW strLocaleName( find.GetFileTitle() );
#if 1
		const LCID lcid = WQSG_LocaleNameToLCID( strLocaleName );
		if( lcid == 0 )
			continue;
#else
// 		if( !IsValidLocaleName( strLocaleName ) )
// 			continue;

		const LCID lcid = LocaleNameToLCID( strLocaleName , 0 );
		if( lcid == 0 )
			continue;

		
#endif
		WCHAR buf[1024] = {};
		if( !GetLocaleInfo( lcid , LOCALE_SNATIVELANGNAME , buf , 1024 ) )
		{
			ASSERT(0);
			continue;
		}

		if( m_Langs.find( lcid ) != m_Langs.end() )
		{
			ASSERT( m_Langs.find( lcid ) == m_Langs.end() );
		}

		SLang& lang = m_Langs[lcid];
		lang.m_lcid = lcid;
		lang.m_strName = buf;

		const CString strFile( find.GetFilePath() );
		if( !LoadLang( strFile , lang ) )
			m_Langs.erase( m_Langs.find( lcid ) );

		m_lcids.push_back( lcid );
	}
	while(bNext);

	return TRUE;
}

void CWQSG_UMDDlg::OnBnClickedButtonLang()
{
	// TODO: 在此添加控件通知处理程序代码

	CSelLang dlg( m_SelLcid , &m_lcids );
	if( dlg.DoModal() == IDOK )
		UseLang( dlg.GetLcid() );
}

void CWQSG_UMDDlg::UseLang( LCID a_lcid )
{
	std::map<LCID,SLang>::iterator it = m_Langs.find(a_lcid);
	if( it != m_Langs.end() )
	{
		const SLang& lang = it->second;

		m_SelLcid = a_lcid;

		if( lang.m_vIsoBaseLang.size() )
			m_umd.Base_SetLangString( (&lang.m_vIsoBaseLang[0]) , lang.m_vIsoBaseLang.size() );

		if( lang.m_vIsoAppLang.size() )
			m_umd.SetLangString( (&lang.m_vIsoAppLang[0]) , lang.m_vIsoAppLang.size() );

		if( lang.m_vThisLang.size() )
			m_StringMgr.SetString( (&lang.m_vThisLang[0]) , lang.m_vThisLang.size() );
	}

	SetDlgItemTextW( IDC_BUTTON_OPEN , m_StringMgr.GetString(6) );
	SetDlgItemTextW( IDC_BUTTON_Apply_WIFP , m_StringMgr.GetString(7) );
	SetDlgItemTextW( IDC_BUTTON_Create_WIFP , m_StringMgr.GetString(8) );
	SetDlgItemTextW( IDC_BUTTON_Expand_ISO , m_StringMgr.GetString(9) );
	SetDlgItemTextW( IDC_BUTTON_About , m_StringMgr.GetString(10) );
	SetDlgItemTextW( IDC_BUTTON_UP , m_StringMgr.GetString(22) );
	
	if (m_StringMgr.GetString(23))
		WQSG_FileList[0].name = m_StringMgr.GetString(23);
	if (m_StringMgr.GetString(24))
		WQSG_FileList[1].name = m_StringMgr.GetString(24);
	if (m_StringMgr.GetString(25))
		WQSG_FileList[2].name = m_StringMgr.GetString(25);
}


bool CWQSG_UMDDlg::IsWorking()
{
	bool b_ret = m_bWorking;
	if (m_bWorking)
	{
		MessageBox(L"当前有任务进行中，请稍后。", L"提示", MB_OK | MB_ICONINFORMATION);
	}

	//防止在消息框的时间里任务结束又启动一个任务
	return b_ret;
}