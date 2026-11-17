# pragma once
# include "MyListCtrl.h"
# include "VListCtrl.h"

class CSortClass
{
public:
	CSortClass(CListCtrlXP * _pWnd, const int _iCol);
	virtual ~CSortClass();
	void Sort(bool bAsc, int tipo);
	static int CALLBACK Compare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

protected:
	CListCtrlXP *pWnd;
	CVListCtrl  *vpWnd;
	struct CSortItem
	{
		CSortItem(const DWORD _dw, const CString &_txt);
		DWORD dw; 
		CString txt;
	};
};
