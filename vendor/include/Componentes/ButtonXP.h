#pragma once
//***************************************************************************
//
// AUTHOR:  JUHU
//
// DESCRIPTION: Este objeto deciende de CButton, cambia el estilo a XP.
//
// ORIGINAL: 
//
//***************************************************************************

#include "Tools.h"

class  __COMPONENTES_EXPORT__ CButtonXP : public CButton
{
// Construction
public:
	CButtonXP();
	virtual ~CButtonXP();

// Attributes
public:
	bool	m_bValidate;
// Operations
public:
    afx_msg void OnSetFocus(CWnd* pOldWnd);
	DECLARE_MESSAGE_MAP()
	CString	GetText();

// propiedades
	__declspec(property(get=GetText,put=SetWindowText))		CString	Text;
	__declspec(property(get=IsWindowEnabled,put=EnableWindow)) bool Enable;
};