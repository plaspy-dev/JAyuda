#pragma once
# include <afxmdiframewndex.h>
#include <afxcontrolbars.h>     // MFC support for ribbons and control bars
# include "../extras/TreeNavBar.h"
# include "../FadeWnd.h"
# include "shockwaveflash1.h"

# define VENTANAS_PREFIJO	23215

#define ID_STATUSBAR_TEXTO 121
#define ID_STATUSBAR_BARRADEPROGRESO 122
#define ID_STATUSBAR_ANIMACIONPROGRESO 123
#define ID_STATUSBAR_USUARIO 124
#define ID_STATUSBAR_SERVIDOR 125
#define ID_STATUSBAR_TOTAL 126

#define ID_WINDOW_MANAGER               131

/////////////////////////////////////////////////////////////////////////////
// CMDIFrameWndXP for menu to documents

class   __COMPONENTES_EXPORT__ CMDIFrameWndXP : public CMDIFrameWndEx
{

  DECLARE_DYNCREATE(CMDIFrameWndXP);
protected:
	//flash
	CString				m_szArchivo;
	DWORD	m_dwBkColor;
	int					m_nAncho;
	int					m_nAlto;
	CShockwaveflash1	*m_pFlash;
	CSize	m_sizeClient;
protected:
	// funciones del flash
	void BuildFlash(bool load=false);
	// fondo
	HBITMAP	m_hBitmap;
	BITMAP		m_bmInfo;
	CFont	m_LogoFont;

public: // nuevos elementos office 2k7
	CMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;

  ULONGLONG		m_ulTotal;
  ULONGLONG		m_ulRealizados;

  int		m_nCurrentHilos;
  int		m_nAnimation;

protected:
	CMiscelaneosEx  m_pAceleradores;

	CTreeNavBar		m_pNavBar;
	// XStatusBar		m_wndStatusBar;

	CTime			m_pTLocal, m_pTServidor;
	CTimeSpan		m_psDiferencia;
	CFadeWnd		*m_pFadeWnd;

public:
	bool			m_bInactivo;
	int				m_nInactivo;
	bool			m_bFocus;
	bool			m_bConectando;
public:
	~CMDIFrameWndXP();
	CMDIFrameWndXP();
  
  DECLARE_MESSAGE_MAP()

	// nuevo solo para el portapapeles
	afx_msg void OnEditPaste();
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditUndo();
	afx_msg void OnEditClear();
	afx_msg void OnEditSelectAll();
	afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditUndo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditCut(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditClear(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditSelectAll(CCmdUI *pCmdUI);
	
protected:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void LoadState();
	bool AddAcelerator(ACCEL nueva);
	CString ParseAcelerator(CString label, int id);
	bool LoadDocument( CMenu *menu, HTREEITEM hRoot, CString nombre, UINT nIDResource,CRuntimeClass* pViewClass, CRuntimeClass *pDocClass=NULL);
	bool LoadDocument( CMenu *menu, HTREEITEM hRoot, CString nombre, UINT nIDResource,CRuntimeClass* pViewClass, const CMiscelaneosEx &permisos, DWORD grupo, CRuntimeClass *pDocClass=NULL);
	virtual void SaveState();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	void OnUpdateVentanas(CCmdUI *pCmdUI);
	void OnCommandVentanas(UINT id);
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	bool LoadDocumentos(DWORD parametro);
	bool LoadDocumentos(const CMiscelaneosEx &permisos, DWORD grupo);
	afx_msg LRESULT OnSetStatus(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnThreadResult(WPARAM wParam, LPARAM lParam);
	void OnUpdateMenus(CCmdUI *pCmdUI);
	void OnCommandMenu(UINT id);
public:
	afx_msg LRESULT OnTreeNavegador(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStatusBar(WPARAM wParam, LPARAM lParam);
	void DockControlBarLeftOf(CToolBar* Bar, CToolBar* LeftOf);
	void SendMessageBroadCast(UINT message,WPARAM wParam=0, LPARAM lParam=0);
	afx_msg void OnClose();
	afx_msg void OnVerNavegador();
	afx_msg void OnUpdateVerNavegador(CCmdUI *pCmdUI);
	afx_msg LRESULT OnQueryTime(WPARAM wParam, LPARAM lParam);
	afx_msg void OnNextPane();
	afx_msg void OnUpdateNextPane(CCmdUI *pCmdUI);
	afx_msg void OnPrevPane();
	afx_msg void OnUpdatePrevPane(CCmdUI *pCmdUI);
	afx_msg void OnCloseAll();
	int LoadDocumentosEx(CString nombreDLL, HTREEITEM hRoot, const CMiscelaneosEx &permisos, DWORD grupo);
	int LoadDocumentosEx(CString nombreDLL, HTREEITEM hRoot, DWORD parametro);
	afx_msg void OnHelpIndex();
	CTreeNavBar	*GetNavigateBar();
	void OnApplicationLook(UINT id);
	void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	void OnWindowManager();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void InitializeRibbon();
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	virtual BOOL OnShowMDITabContextMenu(CPoint point, DWORD dwAllowedItems, BOOL bDrop);
	virtual BOOL OnEraseMDIClientBackground(CDC* pDC);
	void SetFlash(CString archivo, int ancho=0, int alto=0, DWORD bkColor=-1);
};

