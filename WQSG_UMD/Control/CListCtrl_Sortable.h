#pragma once

//#include "CListCtrl_DataModel.h"
#include "..\Stdafx.h"
#include "..\Resource.h"
#include <afxwin.h>
#include <vector>

class CListCtrl_Sortable : public CListCtrl
{
	bool m_Ascending;
	int  m_SortCol;

	DECLARE_MESSAGE_MAP();

	afx_msg BOOL OnHeaderClick(NMHDR* pNMHDR, LRESULT* pResult);
	void PreSubclassWindow();

	/*static Color focuscolors[];
	static Color focuscolors2[];
	static Color overcolors[];
	static Color downcolors[];
	static float positions[];

	HDC m_hDC;*/

	std::vector<int> vecNumCol;

public:
	CListCtrl_Sortable()
		:m_Ascending(false)
		,m_SortCol(-1)
	{
		//m_hDC = NULL;
	}

	int GetColumnData(int col) const;
	void SetSortArrow(int col, bool ascending);
	void AddNumColumn(int col);
	bool IsNumColumn(int col);

	bool IsAscending() const { return m_Ascending; }
	void ResetSortOrder();
	virtual bool SortColumn(int columnIndex, bool ascending) = 0;
	virtual BOOL DeleteAllItems();

	/*afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);*/
};