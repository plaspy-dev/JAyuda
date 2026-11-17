// MainFrm.h: interfaz de la clase CMainFrame
//


#pragma once

#include "conector/conector.h"


class CMainFrame : public CMDIFrameWndXP
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();
private:

protected:
	CHelpBar		m_wndHelpBar;
// Atributos
public:

// Operaciones
public:

// Reemplazos
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementación
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	void			ResetToolbars();

	virtual void	SaveState();
	virtual void	LoadState();


// Funciones de asignación de mensajes generadas
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnUpdateFileNew(CCmdUI *pCmdUI);
	CDocument *OnAbrir(CString archivo);

	afx_msg void OnAyudaAyuda();
	afx_msg void OnAyudaIndice();
	afx_msg void OnAyudaBuscar();
	afx_msg void OnUpdateAyudaIndice(CCmdUI *pCmdUI);
	afx_msg void OnUpdateAyudaBuscar(CCmdUI *pCmdUI);

	LRESULT OnQueryNavigate(WPARAM wParam, LPARAM lParam);
	virtual void InitializeRibbon();
};


