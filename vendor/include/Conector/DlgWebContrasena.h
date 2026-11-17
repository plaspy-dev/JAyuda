#pragma once
#include "afxwin.h"
# include <Componentes/Componentes.h>
# include "resource.h"

// CDlgWebContrasena dialog

class CDlgWebContrasena : public CDialog
{
	DECLARE_DYNAMIC(CDlgWebContrasena)
	CString m_szURL;
	int m_nAncho;
	int m_nAlto;
public:
	CDlgWebContrasena(const CString &url, CWnd* pParent, int ancho=-1, int alto=-1);   // standard constructor
	virtual ~CDlgWebContrasena();

// Dialog Data
	enum { IDD = IDD_DWEBPASSWORD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnBnClickedCancel();
public:
	CHTMLCtrl m_EWebBrowser;
	CWnd		*m_pAfxMainWnd;
public:
	virtual BOOL OnInitDialog();
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
};
