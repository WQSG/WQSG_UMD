#include "stdafx.h"

#include "CListCtrl_Sortable.h"

#include <shlwapi.h>
/*
#include "..\common.h"

//要渐变的颜色 
Color CListCtrl_Sortable::focuscolors[] = {  
		Color::MakeARGB(160,160,226,255),
        Color::MakeARGB(160,143,210,255),
        Color::MakeARGB(160,130,202,255),
        Color::MakeARGB(160,185,235,255) 
    };
Color CListCtrl_Sortable::focuscolors2[] = {  
		Color::MakeARGB(50,120,151,255),
        Color::MakeARGB(50,83,135,255),
        Color::MakeARGB(50,70,147,255),
        Color::MakeARGB(50,110,160,255) 
    };
Color CListCtrl_Sortable::overcolors[] = {  
		Color::MakeARGB(130,201,246,255),
        Color::MakeARGB(130,179,230,255),
        Color::MakeARGB(130,166,222,255),
        Color::MakeARGB(130,234,255,255) 
    };
Color CListCtrl_Sortable::downcolors[] = {  
		Color::MakeARGB(150,143,210,255),
        Color::MakeARGB(150,160,226,255),
        Color::MakeARGB(150,185,235,255),
        Color::MakeARGB(150,130,202,255) 
    };

float CListCtrl_Sortable::positions[] = { 0, 0.45F, 0.5F, 1.0F }; 
*/
BEGIN_MESSAGE_MAP(CListCtrl_Sortable, CListCtrl)
	ON_NOTIFY_REFLECT_EX(LVN_COLUMNCLICK, OnHeaderClick)	// Column Click
	/*ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)*/
END_MESSAGE_MAP()

namespace {
	bool IsThemeEnabled()
	{
		HMODULE hinstDll;
		bool XPStyle = false;
		bool (__stdcall *pIsAppThemed)();
		bool (__stdcall *pIsThemeActive)();

		// Test if operating system has themes enabled
		hinstDll = ::LoadLibrary(_T("UxTheme.dll"));
		if (hinstDll)
		{
			(FARPROC&)pIsAppThemed = ::GetProcAddress(hinstDll, "IsAppThemed");
			(FARPROC&)pIsThemeActive = ::GetProcAddress(hinstDll,"IsThemeActive");
			::FreeLibrary(hinstDll);
			if (pIsAppThemed != NULL && pIsThemeActive != NULL)
			{
				if (pIsAppThemed() && pIsThemeActive())
				{
					// Test if application has themes enabled by loading the proper DLL
					hinstDll = LoadLibrary(_T("comctl32.dll"));
					if (hinstDll)
					{
						DLLGETVERSIONPROC pDllGetVersion = (DLLGETVERSIONPROC)::GetProcAddress(hinstDll, "DllGetVersion");
						::FreeLibrary(hinstDll);
						if (pDllGetVersion != NULL)
						{
							DLLVERSIONINFO dvi;
							ZeroMemory(&dvi, sizeof(dvi));
							dvi.cbSize = sizeof(dvi);
							HRESULT hRes = pDllGetVersion ((DLLVERSIONINFO *) &dvi);
							if (SUCCEEDED(hRes))
                                XPStyle = dvi.dwMajorVersion >= 6;
						}
					}
				}
			}
		}
		return XPStyle;
	}

	LRESULT EnableWindowTheme(HWND hwnd, LPCWSTR app, LPCWSTR idlist)
	{
		HMODULE hinstDll;
		HRESULT (__stdcall *pSetWindowTheme)(HWND hwnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList);
		HANDLE (__stdcall *pOpenThemeData)(HWND hwnd, LPCWSTR pszClassList);
		HRESULT (__stdcall *pCloseThemeData)(HANDLE hTheme);

		hinstDll = ::LoadLibrary(_T("UxTheme.dll"));
		if (hinstDll)
		{
			(FARPROC&)pOpenThemeData = ::GetProcAddress(hinstDll, "OpenThemeData");
			(FARPROC&)pCloseThemeData = ::GetProcAddress(hinstDll, "CloseThemeData");
			(FARPROC&)pSetWindowTheme = ::GetProcAddress(hinstDll, "SetWindowTheme");
			::FreeLibrary(hinstDll);
			if (pSetWindowTheme && pOpenThemeData && pCloseThemeData)
			{
				HANDLE theme = pOpenThemeData(hwnd, L"ListView");
				if (theme!=NULL)
				{
					VERIFY(pCloseThemeData(theme)==S_OK);
					return pSetWindowTheme(hwnd, app, idlist);
				}
			}
		}
		return S_FALSE;
	}
}

BOOL CListCtrl_Sortable::OnHeaderClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLISTVIEW* pLV = reinterpret_cast<NMLISTVIEW*>(pNMHDR);

	SetFocus();	// Ensure other controls gets kill-focus

	int colIndex = pLV->iSubItem;

	if (m_SortCol==colIndex)
	{
		m_Ascending = !m_Ascending;
	}
	else
	{
		m_SortCol = colIndex;
		m_Ascending = true;
	}

	if (SortColumn(m_SortCol, m_Ascending))
		SetSortArrow(m_SortCol, m_Ascending);

	*pResult = 0;

	return FALSE;	// Let parent-dialog get chance
}

void CListCtrl_Sortable::SetSortArrow(int colIndex, bool ascending)
{
	if (IsThemeEnabled())
	{
#if (_WIN32_WINNT >= 0x501)
		for(int i = 0; i < GetHeaderCtrl()->GetItemCount(); ++i)
		{
			HDITEM hditem = {0};
			hditem.mask = HDI_FORMAT;
			VERIFY( GetHeaderCtrl()->GetItem( i, &hditem ) );
			hditem.fmt &= ~(HDF_SORTDOWN|HDF_SORTUP);
			if (i == colIndex)
			{
				hditem.fmt |= ascending ? HDF_SORTDOWN : HDF_SORTUP;
			}
			VERIFY( CListCtrl::GetHeaderCtrl()->SetItem( i, &hditem ) );
		}
#endif
	}
	else
	{
		UINT bitmapID = m_Ascending ? IDB_DOWNARROW : IDB_UPARROW; 
		for(int i = 0; i < GetHeaderCtrl()->GetItemCount(); ++i)
		{
			HDITEM hditem = {0};
			hditem.mask = HDI_BITMAP | HDI_FORMAT;
			VERIFY( GetHeaderCtrl()->GetItem( i, &hditem ) );
			if (hditem.fmt & HDF_BITMAP && hditem.fmt & HDF_BITMAP_ON_RIGHT)
			{
				if (hditem.hbm)
				{
					DeleteObject(hditem.hbm);
					hditem.hbm = NULL;
				}
				hditem.fmt &= ~(HDF_BITMAP|HDF_BITMAP_ON_RIGHT);
				VERIFY( CListCtrl::GetHeaderCtrl()->SetItem( i, &hditem ) );
			}
			if (i == colIndex)
			{
				hditem.fmt |= HDF_BITMAP|HDF_BITMAP_ON_RIGHT;
				hditem.hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(bitmapID), IMAGE_BITMAP, 0,0, LR_LOADMAP3DCOLORS); 
				VERIFY( hditem.hbm!=NULL );
				VERIFY( CListCtrl::GetHeaderCtrl()->SetItem( i, &hditem ) );
			}
		}
	}
}

void CListCtrl_Sortable::PreSubclassWindow()
{
	CListCtrl::PreSubclassWindow();

	// Focus retangle is not painted properly without double-buffering
#if (_WIN32_WINNT >= 0x501)
	SetExtendedStyle(LVS_EX_DOUBLEBUFFER | GetExtendedStyle());
#endif
	SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);


	EnableWindowTheme(GetSafeHwnd(), L"Explorer", NULL);
	//m_hDC = ::GetDC(m_hWnd);
}

void CListCtrl_Sortable::ResetSortOrder()
{
	m_Ascending = true;
	m_SortCol = -1;
	SetSortArrow(m_SortCol, m_Ascending);
}

// The column version of GetItemData(), one can specify an unique
// identifier when using InsertColumn()
int CListCtrl_Sortable::GetColumnData(int col) const
{
	LVCOLUMN lvc = {0};
	lvc.mask = LVCF_SUBITEM;
	VERIFY( GetColumn(col, &lvc) );
	return lvc.iSubItem;
}

BOOL CListCtrl_Sortable::DeleteAllItems()
{
	SetSortArrow(-1, false);
	return CListCtrl::DeleteAllItems();
}

void CListCtrl_Sortable::AddNumColumn(int col)
{
	if (!IsNumColumn(col))
		vecNumCol.push_back(col);
}

bool CListCtrl_Sortable::IsNumColumn(int col)
{
	for (unsigned long i = 0; i < vecNumCol.size(); i++)
	{
		if (vecNumCol[i] == col)
			return true;
	}

	return false;
}
/*
BOOL CListCtrl_Sortable::OnEraseBkgnd(CDC* pDC)
{
	//pDC->SetBkMode(TRANSPARENT);
	return TRUE;
	//return CListCtrl::OnEraseBkgnd(pDC);
}

void CListCtrl_Sortable::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult) 
{ 
    // TODO: Add your control notification handler code here 
    NMLVCUSTOMDRAW* pLVCD = reinterpret_cast <NMLVCUSTOMDRAW*>(pNMHDR);

    *pResult = CDRF_DODEFAULT; 
    // Request item-specific notifications if this is the 
    // beginning of the paint cycle.

    if(CDDS_PREPAINT  == pLVCD-> nmcd.dwDrawStage) 
    { 
		::SetBkMode(pLVCD->nmcd.hdc, TRANSPARENT);
		HWND sourceImagehwnd = ::GetParent(m_hWnd);
		//得到父窗体的背景设备DC句柄
		HDC srcDC = (HDC)::SendMessage(sourceImagehwnd, WM_TA_GET_BG_DC, NULL, NULL);
		if (srcDC)
		{
			RECT lvRect, imageRect, lvWinRect, imageWinRect;

			::GetClientRect(m_hWnd, &lvRect);
			::GetClientRect(sourceImagehwnd, &imageRect);

			ClientToScreen(&lvRect);
			::GetWindowRect(m_hWnd, &lvWinRect);
			::GetWindowRect(sourceImagehwnd, &imageWinRect);

			int offsetX = lvRect.left - imageWinRect.left;
			int offsetY = lvRect.top - imageWinRect.top;

			::BitBlt(pLVCD->nmcd.hdc, 0, 0, lvWinRect.right - lvWinRect.left, lvWinRect.bottom - lvWinRect.top, srcDC, offsetX, offsetY, SRCCOPY);
			//::InvalidateRect(m_hWnd, &lvRect, 0);

			CHeaderCtrl *pHCtrl = GetHeaderCtrl();
			if (pHCtrl)
			{
				pHCtrl->Invalidate();
			}

			//RedrawItems(0, GetItemCount() - 1);

			*pResult = CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYSUBITEMDRAW;
		}
    } 
    else if(CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage) 
    { 
     // This is the beginning of an item 's paint cycle. 
		LVITEMW rItem; 
        int nItem = static_cast<int>(pLVCD->nmcd.dwItemSpec); 

        CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc); 
		CRect rcBounds, rcText;
		GetItemRect(nItem, &rcBounds, LVIR_BOUNDS);
		GetItemRect(nItem, &rcText, LVIR_LABEL);

		//pDC->FillSolidRect(rcBounds, RGB(0xff, 0xff, 0xff));

		BOOL bListHasFocus = (GetSafeHwnd() == ::GetFocus());
		// Get the image index and selected/focused state of the 
        // item being drawn. 
        ZeroMemory(&rItem, sizeof(LVITEM)); 
        rItem.mask = LVIF_IMAGE | LVIF_STATE | LVIF_TEXT; 
        rItem.iItem = nItem;
        rItem.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
		rItem.cchTextMax = 256;
		rItem.pszText = (LPWSTR)malloc(sizeof(wchar_t) * 256);
		::SendMessage(m_hWnd, LVM_GETITEMW, 0, (LPARAM)&rItem);

		Graphics imgraphics(pLVCD->nmcd.hdc);

		CPoint pt;
		::GetCursorPos(&pt);
		::ScreenToClient(m_hWnd, &pt);

		UINT flags = LVHT_ONITEM;
		int iMoveItem = HitTest(pt, &flags);

		if (iMoveItem != -1 && iMoveItem == nItem)
		{
			LinearGradientBrush lb(Gdiplus::Rect(rcBounds.left,rcBounds.top,rcBounds.Width(),rcBounds.Height()),
															Color::Blue,Color::Transparent,
															LinearGradientModeVertical);
			lb.SetInterpolationColors(overcolors, positions, 4);	
			imgraphics.FillRectangle(&lb,rcBounds.left,rcBounds.top,rcBounds.Width(),rcBounds.Height());
		}

		if (rItem.state & LVIS_SELECTED) 
        { 
			LinearGradientBrush lb(Gdiplus::Rect(rcBounds.left,rcBounds.top,rcBounds.Width(),rcBounds.Height()),
															Color::Blue,Color::Transparent,
															LinearGradientModeVertical);
            if (bListHasFocus) 
            {
				lb.SetInterpolationColors(focuscolors, positions, 4);
            } else {
				lb.SetInterpolationColors(focuscolors2, positions, 4);
            }

			imgraphics.FillRectangle(&lb,rcBounds.left,rcBounds.top,rcBounds.Width(),rcBounds.Height());
        }

		imgraphics.DrawString(rItem.pszText, -1, g_deffont, 
			RectF(rcText.left,rcText.top,rcText.Width(),rcText.Height()), 
			g_strLCfmt, g_blackbrush);

		//画图标
		if (rItem.iImage >= 0)
		{
			CImageList *pImageList = GetImageList(LVSIL_NORMAL);
			if (pImageList)
			{
				CRect rcIcon;
				if (GetItemRect(nItem, &rcIcon, LVIR_ICON))
				{
					HICON hIcon = pImageList->ExtractIcon(rItem.iImage);
					Bitmap bitIcon(hIcon);
					imgraphics.DrawImage(&bitIcon, rcIcon.left, rcIcon.top, rcIcon.Width(), rcIcon.Height());
					::DestroyIcon(hIcon);
				}
			}
		}

		//画子项
		CHeaderCtrl *pHCtrl = GetHeaderCtrl();
		if (pHCtrl)
		{
			int cSubItem = pHCtrl->GetItemCount();
			if (cSubItem > 1)
			{
				
				rItem.mask = LVIF_TEXT;
				rItem.stateMask = NULL;

				for (int i = 1; i < cSubItem; i++)
				{
					if (GetSubItemRect(nItem, i, LVIR_LABEL, rcText))
					{
						rItem.iSubItem = i;
						::SendMessage(m_hWnd, LVM_GETITEMW, 0, (LPARAM)&rItem);
						
						imgraphics.DrawString(rItem.pszText, -1, g_deffont, 
							RectF(rcText.left,rcText.top,rcText.Width(),rcText.Height()), 
							g_strLCfmt, g_blackbrush);
					}
				}
			}
		}

		free(rItem.pszText);
        //*pResult = CDRF_NOTIFYITEMDRAW;
		*pResult = CDRF_SKIPDEFAULT;
    }
}
*/