#include "stdafx.h"

#include "CListCtrl_SortItems.h"

BEGIN_MESSAGE_MAP(CListCtrl_SortItems, CListCtrl)
	ON_NOTIFY_REFLECT_EX(LVN_COLUMNCLICK, OnHeaderClick)	// Column Click
	/*ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)*/
	//ON_WM_PAINT()
END_MESSAGE_MAP()


namespace {
	struct PARAMSORT
	{
		PARAMSORT(CListCtrl_SortItems *pListCtrl, int columnIndex, bool ascending)
			:m_ColumnIndex(columnIndex)
			,m_Ascending(ascending)
			,m_pListCtrl(pListCtrl)
		{}

		CListCtrl_SortItems *m_pListCtrl;
		int  m_ColumnIndex;
		bool m_Ascending;
	};

	// Comparison extracts values
	int CALLBACK SortFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
	{
		PARAMSORT *pps = (PARAMSORT*)lParamSort;

		CString szLeft, szRight;
		szLeft = pps->m_pListCtrl->GetItemText(lParam1, pps->m_ColumnIndex);
		szRight = pps->m_pListCtrl->GetItemText(lParam2, pps->m_ColumnIndex);

		//TRACE("%d %d\r\n", lParam1, lParam2);
		if (pps->m_Ascending)
			return _tcscmp( szRight, szLeft );	
		else
			return _tcscmp( szLeft, szRight );					
	}

	int CALLBACK NumSortFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
	{
		PARAMSORT *pps = (PARAMSORT*)lParamSort;

		long szLeft, szRight;
		szLeft = _tcstol(pps->m_pListCtrl->GetItemText(lParam1, pps->m_ColumnIndex), NULL, 10);
		szRight = _tcstol(pps->m_pListCtrl->GetItemText(lParam2, pps->m_ColumnIndex), NULL, 10);

		//TRACE("%d %d\r\n", lParam1, lParam2);
		if (pps->m_Ascending)
			return szRight - szLeft;	
		else
			return szLeft - szRight;					
	}
}

bool CListCtrl_SortItems::SortColumn(int columnIndex, bool ascending)
{
	PARAMSORT paramsort(this, columnIndex, ascending);

	if (IsNumColumn(columnIndex))
		SortItemsEx(NumSortFunc, (DWORD_PTR)&paramsort);
	else
		SortItemsEx(SortFunc, (DWORD_PTR)&paramsort);

	return true;
}

