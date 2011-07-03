#pragma once

#include "CListCtrl_Sortable.h"

class CListCtrl_SortItems : public CListCtrl_Sortable
{

	DECLARE_MESSAGE_MAP();

public:
	CListCtrl_SortItems()
	{}

	virtual bool SortColumn(int columnIndex, bool ascending);
};