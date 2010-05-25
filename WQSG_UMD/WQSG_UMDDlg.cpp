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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
struct
{
	WCHAR const*const	name;
	const int			w;
}WQSG_FileList[] = {
	{	L"fileName"	,	300	},
	{	L"LBA"		,	80	},
	{	L"size"		,	100	},
};

typedef void (AFX_MSG_CALL CWQSG_UMDDlg::*TMenuFun)();
struct SMenuData
{
	size_t m_LangId;
	TMenuFun m_pFn;
	UINT m_uFlags;
};

const SMenuData g_MenuData0[] = {
	{ 1 , &CWQSG_UMDDlg::OnExportFiles , MF_STRING },
	{ 0 , NULL , MF_SEPARATOR },
	{ 2 , NULL , MF_STRING },
	{ 3 , NULL , MF_STRING },//L"添加文件" 
	{ 0 , NULL , MF_SEPARATOR },
	{ 4 , &CWQSG_UMDDlg::On32774_替换文件 , MF_STRING },
	{ 5 , &CWQSG_UMDDlg::On32776_写文件偏移 , MF_STRING },
};
//MF_GRAYED
const SMenuData g_MenuData1[] = {
	{ 1 , &CWQSG_UMDDlg::OnExportFiles , MF_STRING },
	{ 0 , NULL , MF_SEPARATOR },
	{ 2 , NULL , MF_STRING },
	{ 3 , NULL , MF_STRING },//L"添加文件" 
	{ 0 , NULL , MF_SEPARATOR },
	{ 4 , NULL , MF_STRING | MF_DISABLED | MF_GRAYED },
	{ 5 , NULL , MF_STRING | MF_DISABLED | MF_GRAYED },
};

const SMenuData g_MenuData2[] = {
	{ 1 , &CWQSG_UMDDlg::OnExportFiles , MF_STRING },
	{ 0 , NULL , MF_SEPARATOR },
	{ 2 , NULL , MF_STRING | MF_DISABLED | MF_GRAYED },
	{ 3 , NULL , MF_STRING | MF_DISABLED | MF_GRAYED },
	{ 0 , NULL , MF_SEPARATOR },
	{ 4 , NULL , MF_STRING | MF_DISABLED | MF_GRAYED },
	{ 5 , NULL , MF_STRING | MF_DISABLED | MF_GRAYED },
};

#define DEF_MAKE_ITEM( __defx ) { __defx , sizeof(__defx)/sizeof(*__defx) },
const CWQSG_UMDDlg::SMenuDataList CWQSG_UMDDlg::ms_MenuDataList[CWQSG_UMDDlg::ms_Max_Pop] = {
	DEF_MAKE_ITEM( g_MenuData0 )
	DEF_MAKE_ITEM( g_MenuData1 )
	DEF_MAKE_ITEM( g_MenuData2 )
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
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWQSG_UMDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FILE, m_cFileList);
	DDX_Text(pDX, IDC_EDIT1, m_pathW);
	DDX_Text(pDX, IDC_EDIT2, m_strInfo);
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
	MessageBox( L"Init menu error" );
	return FALSE;
}

BEGIN_MESSAGE_MAP(CWQSG_UMDDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CWQSG_UMDDlg::OnBnClickedButton1)
	ON_NOTIFY(LVN_ITEMACTIVATE, IDC_LIST_FILE, &CWQSG_UMDDlg::OnLvnItemActivateListFile)
	ON_BN_CLICKED(IDC_BUTTON_UP, &CWQSG_UMDDlg::OnBnClickedButtonUp)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_FILE, &CWQSG_UMDDlg::OnNMRClickListFile)
	ON_BN_CLICKED(IDC_BUTTON_About, &CWQSG_UMDDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON_Expand_ISO, &CWQSG_UMDDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON_Apply_WIFP, &CWQSG_UMDDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON_Create_WIFP, &CWQSG_UMDDlg::OnBnClickedButton5)
	ON_COMMAND_RANGE( 1000 , 2000 , &CWQSG_UMDDlg::OnPopMenu)
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
	m_cFileList.SetExtendedStyle( m_cFileList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	const int count = sizeof(WQSG_FileList)/sizeof(*WQSG_FileList);
	for( int i = 0 ; (i >= 0) && (i < count) ; ++i )
	{
		if( i != m_cFileList.InsertColumn( i , WQSG_FileList[i].name , 0 , WQSG_FileList[i].w ) )
		{
			MessageBox( L"Filelist init error" );
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
			!m_imageList.Create( info.rcImage.right , info.rcImage.bottom , ILC_COLORDDB , 1 , 1 ) )
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
		MessageBox( L"Init menu error" );
		EndDialog( IDCANCEL );
		return FALSE;
	}

	for( int i = 0 ; i < ms_Max_Pop ; ++i )
	{
		if( !InitPopMenu( m_PopMenu[i] , ms_MenuDataList[i].m_pData , ms_MenuDataList[i].m_Count , 1000 + (WORD)m_MenuFun.size() ) )
		{
			EndDialog( IDCANCEL );
			return FALSE;
		}
	}

	SetDlgItemTextW( IDC_BUTTON_OPEN , m_StringMgr.GetString(6) );
	SetDlgItemTextW( IDC_BUTTON_Apply_WIFP , m_StringMgr.GetString(7) );
	SetDlgItemTextW( IDC_BUTTON_Create_WIFP , m_StringMgr.GetString(8) );
	SetDlgItemTextW( IDC_BUTTON_Expand_ISO , m_StringMgr.GetString(9) );
	SetDlgItemTextW( IDC_BUTTON_About , m_StringMgr.GetString(10) );

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

	CDialog::OnClose();
	EndDialog( IDCANCEL );
}

void CWQSG_UMDDlg::OnCancel(){}
void CWQSG_UMDDlg::OnOK(){}
bool CWQSG_UMDDlg::OpenISO( CStringW ISO_PathName , const BOOL bCanWrite )
{
	CloseISO();
	if( !m_umd.OpenISO( ISO_PathName , bCanWrite , E_WIT_UMD ) )
	{
		MessageBox( m_umd.GetErrStr() );
		m_umd.CleanErrStr();
		return false;
	}
	m_path = L"";
	UpDataLbaData();
	m_umd.IsCanWrite()?UiOpenRW():UiOpenR();
	return true;
}

void CWQSG_UMDDlg::CloseISO()
{
	UiClose();
	m_umd.CloseISO( );
	m_path = "";
	UpDataGUI();
}

void CWQSG_UMDDlg::UpDataLbaData()
{
	m_umd.GetFreeInfo( &m_uMaxFreeBlock , &m_uFreeLbaCount , &m_uFreeBlockCount );
	UpDataGUI();
}

void CWQSG_UMDDlg::UpDataGUI()
{
	m_cFileList.DeleteAllItems();
	m_strInfo = L"";
#if 1
	SIsoFileFind* handle;
	if( m_umd.IsOpen() && (handle = m_umd.FindIsoFile( m_path )) )
	{
		u32 uFile = 0;
		u32 uDir = 0;
		SIsoFileData data;
		while( m_umd.FindNextIsoFile( handle , data ) )
		{
			CString str;
			str = data.name;
			const int index = m_cFileList.InsertItem( m_cFileList.GetItemCount() , str , ((data.isDir)?0:1) );
			if( index >= 0 )
			{
				if( data.isDir )
				{
					uDir++;

					m_cFileList.SetItemData( index , 0 );
				}
				else
				{
					uFile++;

					CString tmp;

					tmp.Format( L"%d" , data.lba );
					m_cFileList.SetItemText( index , 1 , tmp );

					tmp.Format( L"%d" , data.size );
					m_cFileList.SetItemText( index , 2 , tmp );

					m_cFileList.SetItemData( index , 1 );
				}
			}
		}
		m_umd.CloseFindIsoFile( handle );

		m_strInfo.Format( m_StringMgr.GetString(13) ,
			uDir , uFile , m_umd.GetMaxLbaCount() , m_uFreeLbaCount , m_uFreeBlockCount , m_uMaxFreeBlock );

		m_pathW = m_path;
	}
	UpdateData( FALSE );
#endif
}

void CWQSG_UMDDlg::OnBnClickedButton1()
{
	static CWQSGFileDialog_Open dlg( L"*.ISO|*.iso||" );

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

BOOL CWQSG_UMDDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if( WM_DROPFILES == pMsg->message )
	{
		const HDROP hDrop = (HDROP)pMsg->wParam;
		if( pMsg->hwnd == m_cFileList.m_hWnd )
		{
			if( m_umd.IsCanWrite() )
			{
				WCHAR strPathName[MAX_PATH*2];

				const int fileCount = DragQueryFile( hDrop , (UINT)-1 , NULL , 0 );

				for( int i = 0 ; (i>=0) && (i<fileCount) && ( DragQueryFile( hDrop , i , strPathName , MAX_PATH*2 ) != (UINT)-1 ) ; ++i )
				{
					BOOL bFileBreak;
					if( !m_umd.EasyImport( bFileBreak , strPathName , m_path ) )
					{
						if( bFileBreak )
							CloseISO();
						MessageBox( m_umd.GetErrStr() );
						m_umd.CleanErrStr();
						break;
					}
				}
				UpDataLbaData();
			}
		}
		DragFinish( hDrop );
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CWQSG_UMDDlg::OnBnClickedButtonUp()
{
	// TODO: 在此添加控件通知处理程序代码
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
	}
}


void CWQSG_UMDDlg::OnExportFiles()
{
	// TODO: 在此添加命令处理程序代码
	CWQSG_DirDlg dlg( m_hWnd , m_StringMgr.GetString(20) , m_LastSelDir.GetString() );
	WCHAR outPath[MAX_PATH*2];
	if( !dlg.GetPath( outPath ) )
		return ;

	m_LastSelDir = outPath ;
	if( m_LastSelDir.Right( 1 ) != L'\\' )
		m_LastSelDir += L'\\';

	POSITION pos = m_cFileList.GetFirstSelectedItemPosition();
	int index = 0;
	while( (index = m_cFileList.GetNextSelectedItem( pos )) >= 0 )
	{
		CString str( m_cFileList.GetItemText( index , 0 ) );
		CStringA strA;	strA = str;

		SIsoFileData data;
		if( !m_umd.GetFileData( data , m_path , strA ) )
		{
			MessageBox( m_umd.GetErrStr() );
			m_umd.CleanErrStr();
			break;
		}

		if( data.isDir )
		{
			if( !m_umd.ExportDir( m_LastSelDir + str , m_path + strA ) )
			{
				MessageBox( m_umd.GetErrStr() );
				m_umd.CleanErrStr();
				break;
			}
		}
		else
		{
			if( !m_umd.ExportFile( m_LastSelDir + str , m_path , strA ) )
			{
				MessageBox( m_umd.GetErrStr() );
				m_umd.CleanErrStr();
				break;
			}
		}
	}
	UpDataLbaData();
}

void CWQSG_UMDDlg::On32774_替换文件()
{
	// TODO: 在此添加命令处理程序代码
	if( !m_umd.IsCanWrite() )
	{
		MessageBox( m_StringMgr.GetString(14) );
		return;
	}

	POSITION pos = m_cFileList.GetFirstSelectedItemPosition();
	const int index = m_cFileList.GetNextSelectedItem( pos );
	if( index >= 0 )
	{
		CString str ( m_cFileList.GetItemText( index , 0 ) );
		CStringA strA; strA = str;

		static ::CWQSGFileDialog_Open dlg( L"*.*|*.*||" );
		dlg.SetWindowTitle( m_StringMgr.GetString(21) );

		if( IDOK != dlg.DoModal() )
			return;

		BOOL bFileBreak;
		if( !m_umd.ImportFile( bFileBreak , m_path , strA , dlg.GetPathName() ) )
		{
			if( bFileBreak )
				CloseISO();
			MessageBox( m_umd.GetErrStr() );
			m_umd.CleanErrStr();
		}

		UpDataLbaData();
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
void CWQSG_UMDDlg::On32776_写文件偏移()
{
	// TODO: 在此添加命令处理程序代码
	if( !m_umd.IsCanWrite() )
	{
		MessageBox( m_StringMgr.GetString(14) );
		return;
	}

	POSITION pos = m_cFileList.GetFirstSelectedItemPosition();
	const int index = m_cFileList.GetNextSelectedItem( pos );
	if( index >= 0 )
	{
		CString name( m_cFileList.GetItemText( index , 0 ) );
		CStringA nameA;	nameA = name;

		SIsoFileData data;
		if( !m_umd.GetFileData( data , m_path , nameA ) )
		{
			MessageBox( m_umd.GetErrStr() );
			m_umd.CleanErrStr();
			return ;
		}

		static CWQSGFileDialog_Open dlg( L"*.*|*.*||" );
		dlg.SetWindowTitle( L"选择要导入的文件..." );

		if( IDOK != dlg.DoModal() )
			return;

		CInputBox ibox( L"相对于文件的偏移" , L"输入相对于文件的偏移" , m_oldOffset , data.size );
		if( IDOK != ibox.DoModal() )
			return ;

		m_oldOffset = ibox.GetVal();

		BOOL bFileBreak;
		if( !m_umd.WriteFile( bFileBreak , m_path , nameA , m_oldOffset , dlg.GetPathName() ) )
		{
			if( bFileBreak )
				CloseISO();

			MessageBox( m_umd.GetErrStr() );
			m_umd.CleanErrStr();
		}

		UpDataLbaData();
	}
}

void CWQSG_UMDDlg::OnBnClickedButton2()
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

	WQSG_About( m_hIcon , m_hWnd , m_StringMgr.GetString(10) , strAppName + L"\r\nv" + strAppVer ,
		CStringW(m_StringMgr.GetString(15)) + L" : <A HREF=\"http://code.google.com/p/wqsg-umd\">http://code.google.com/p/wqsg-umd</A>\r\n"
		+ CStringW(m_StringMgr.GetString(16)) + L" : <A HREF=\"http://code.google.com/p/wqsglib\">http://code.google.com/p/wqsglib</A>\r\n                 <A HREF=\"http://wqsg.ys168.com\">http://wqsg.ys168.com</A>\r\n" ,
		strAuthor2 + L"(" + strAuthor1 + L")" + L"\r\nKid" );
}

void CWQSG_UMDDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
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
		MessageBox( m_umd.AddLbaCount( lba )?m_StringMgr.GetString(18):m_StringMgr.GetString(19) );

		UpDataLbaData();
	}
}

void CWQSG_UMDDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();

	if( !m_umd.IsOpen() )
		return;

	if( !m_umd.IsCanWrite() )
		return;

	static CWQSGFileDialog_Open dlg( L"*.wifp|*.wifp||" );
	if( dlg.DoModal() != IDOK )
		return;

	CWQSG_File fp;
	if( !fp.OpenFile( dlg.GetPathName().GetString() , 1 , 3 ) )
	{
		return;
	}

	BOOL bFileBreak;
	if( m_umd.ImportFilePackage( bFileBreak , fp , TRUE ) )
	{
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
	}
}

void CWQSG_UMDDlg::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();

	if( !m_umd.IsOpen() )
		return;

	static CWQSGFileDialog_Open dlg_iso( L"*.iso|*.iso||" );
	dlg_iso.SetWindowTitle( L"选择原版ISO..." );

	if( dlg_iso.DoModal() != IDOK )
		return;

	static CWQSGFileDialog_Save dlg_out( L"*.wifp|*.wifp||" , L"wifp" );
	dlg_out.SetWindowTitle( L"选择原版ISO..." );

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

	CISO_App oldIso;
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

	MessageBox( L"成功创建补丁" , dlg_out.GetPathName() );
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

	CString strTitle( str1 + L" v" + str2 ); 
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

	if( tp.GetCP() != en_CP_UTF8 )
		return false;

	while( WCHAR* __line = tp.GetLine() )
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
		//if( !IsValidLocaleName( strLocaleName ) )
		//	continue;

		const LCID lcid = LocaleNameToLCID( strLocaleName , 0 );
		if( lcid == 0 )
			continue;

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
	}
	while(bNext);

	return TRUE;
}
