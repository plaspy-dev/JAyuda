#pragma once

# include <afxhtml.h>
# include "Fixhtmlview.h"

// Vista HTML de CHtmlHelpView

class __HHELP_EXPORT__ CHtmlHelpView : public CFixHtmlView
{
	DECLARE_DYNCREATE(CHtmlHelpView)

protected:
	CHtmlHelpView();           // Constructor protegido utilizado por la creación dinámica
	virtual ~CHtmlHelpView();
	bool			m_bVisibleToolbar;
	CString			m_szInitialURL;
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Compatibilidad con DDX o DDV

	DECLARE_MESSAGE_MAP()
public:
	afx_msg virtual void OnAtras();
	afx_msg virtual void OnAdelante();
	afx_msg virtual void OnCancelar();
	afx_msg virtual void OnReload();
	afx_msg virtual void OnHome();
	virtual void OnInitialUpdate();
	CString m_szAppPath;
	LRESULT OnQueryNavigate(WPARAM wParam, LPARAM lParam);
	virtual void OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags, LPCTSTR lpszTargetFrameName, CByteArray& baPostedData, LPCTSTR lpszHeaders, BOOL* pbCancel);
protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
public:
	virtual void OnTitleChange(LPCTSTR lpszText);
	afx_msg void OnDestroy();
	void OnUpdateAdelante(CCmdUI* pCmdUI);
	void OnUpdateAtras(CCmdUI* pCmdUI);

public:
	virtual void OnDocumentComplete(LPCTSTR lpszURL);
	virtual void OnCommandStateChange(long nCommand, BOOL bEnable);
	bool m_bCanBack;
	bool m_bCanForward;
};
