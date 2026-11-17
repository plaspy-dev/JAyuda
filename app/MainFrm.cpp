// MainFrm.cpp : implementación de la clase CMainFrame
//

#include "stdafx.h"
#include "Template.h"
# include "TemplateView.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

# define	ANIMATION_TIMER			300
# define	TIMER_EVENT				32000


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndXP)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndXP)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, OnUpdateFileNew)
	ON_COMMAND(ID_AYUDA_AYUDA, OnAyudaAyuda)
	ON_COMMAND(ID_AYUDA_INDICE, OnAyudaIndice)
	ON_COMMAND(ID_AYUDA_BUSCAR, OnAyudaBuscar)
	ON_UPDATE_COMMAND_UI(ID_AYUDA_INDICE, OnUpdateAyudaIndice)
	ON_UPDATE_COMMAND_UI(ID_AYUDA_BUSCAR, OnUpdateAyudaBuscar)
	ON_MESSAGE(WM_HHELP, OnQueryNavigate)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,
	ID_INDICATOR_EXT,
	ID_INDICATOR_EXT,
	ID_INDICATOR_EXT,
	ID_INDICATOR_EXT,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
};


// Construcción y destrucción de CMainFrame

CMainFrame::CMainFrame()
{
	m_nAnimation=0;
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndXP::OnCreate(lpCreateStruct) == -1)
		return -1;
	// barra de herramientas principal
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_HIDE_INPLACE | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("No se pudo crear la barra de herramientas\n");
		return -1;      // No se pudo crear
	}
	// barra de estado
	if (!m_wndStatusBar.CreateStatusBar(this, indicators, sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("No se pudo crear la barra de estado\n");
		return -1;      // Fehler beim Erzeugen
	}
	if(!m_wndHelpBar.Create("Ayuda", WS_CHILD, this, 201)) {
		TRACE0("Fallo al crear la barra de ayuda\n");
		return -1;
	}

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	m_wndHelpBar.SetBarStyle(m_wndHelpBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_wndHelpBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndHelpBar, AFX_IDW_DOCKBAR_LEFT);

	CREATE_JDOCK(this);

	// los tabs
	DWORD style = MT_TOP | MT_IMAGES | MT_HIDEWLT2VIEWS;
	m_mdiTabs.Create(this, style);

	// barra de estado
	m_wndStatusBar.SetMode(1, XSB_PROGRESS | XSB_SMOOTH);
	m_wndStatusBar.SetRange(1, 0, 100);
	m_wndStatusBar.SetWidth(1, 100);
	m_wndStatusBar.GetProgressCtrl(1)->SetShowText(false);
	m_wndStatusBar.SetMode(2, XSB_TEXT | DT_CENTER);
	m_wndStatusBar.SetText(2, " ");
	m_wndStatusBar.SetWidth( 2, 100);

	if( AfxGetApp()->GetProfileInt("control", "animation", 1)) {
		m_wndStatusBar.SetMode(4, XSB_BITMAP | DT_CENTER);
		CString temp;
		temp.Format("%d", IDB_BLOADING1);
		m_nAnimation=IDB_BLOADING1;
		m_wndStatusBar.SetBitmap(4, temp, temp);
		SetTimer(ANIMATION_TIMER, AfxGetApp()->GetProfileInt("control", "animationtimer", 200), NULL);
	} else {
		m_wndStatusBar.SetMode(4, XSB_TEXT | DT_CENTER);
		m_wndStatusBar.SetNumber(4, 0, 0);
	}

	m_wndStatusBar.SetMode(3, XSB_TEXT | DT_CENTER);
	m_wndStatusBar.SetText(3, " ", " ");
	m_wndStatusBar.SetWidth(3, 100);

	m_DefaultNewMenu.LoadToolBar(IDR_MAINFRAME, NULL);
	// este es el de la licencia
	SetTimer(4, 30*60*1000, NULL);
	
	SetWindowText("JAyuda - " + CApplicacion::GetAppName());

	LoadDocument(NULL, NULL, "Ayuda", IDR_TemplateTYPE,RUNTIME_CLASS(CHtmlHelpView));

	AfxGetApp()->WriteProfileInt("control", "loadstate", 1);
	LoadState();
	AfxGetApp()->WriteProfileInt("control", "loadstate", 0);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndXP::PreCreateWindow(cs) )
		return FALSE;
	return TRUE;
}


// Diagnósticos de CMainFrame

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndXP::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndXP::Dump(dc);
}

#endif //_DEBUG


// Controladores de mensajes de CMainFrame

void CMainFrame::LoadState() {

	CWaitCursor wait;

	LoadBarState("barras\\Barras");
	m_wndHelpBar.LoadStateBar("Barras\\Barras");
	if( !AfxGetApp()->GetProfileInt("barras\\Barras", "store",0)) {
		m_wndHelpBar.ShowWindow(SW_SHOW);
	}
	CMDIFrameWndXP::LoadState();
}

void CMainFrame::SaveState() {

	CMDIFrameWndXP::SaveState();
	m_wndHelpBar.SaveBar("Barras\\Barras");
}

void CMainFrame::OnTimer(UINT nIDEvent)
{
	if( nIDEvent==ANIMATION_TIMER) {

		if( !m_nCurrentHilos) {
			if( m_nAnimation!= IDB_BLOADING1) {
				m_nAnimation=IDB_BLOADING1;
				CString temp;
				temp.Format("%d", m_nAnimation);
				m_wndStatusBar.SetBitmap(4, temp, temp);
			}
			return;
		}
		if( m_nAnimation==IDB_BLOADING10) 
			m_nAnimation=IDB_BLOADING1;
		m_nAnimation++;
		CString temp;
		temp.Format("%d", m_nAnimation);
		m_wndStatusBar.SetBitmap(4, temp, temp);
	}
	CMDIFrameWndXP::OnTimer(nIDEvent);
}

void CMainFrame::OnUpdateFileNew(CCmdUI *pCmdUI)
{
}

CDocument *CMainFrame::OnAbrir(CString archivo) {
	
	CDocument *documento=NULL;
	bool found=false;
	for ( CWnd *child=m_LogoMdiClientWnd.GetTopWindow(); child; child =child->GetNextWindow()) {
		if(child && child->IsKindOf(RUNTIME_CLASS(CMDIChildWnd))) {
			CView *vista=((CMDIChildWnd *) child)->GetActiveView();
			if( vista && ::IsWindow(*vista) && vista->IsKindOf(RUNTIME_CLASS(CHtmlHelpView))) {
				CString cadena= AfxGetApp()->m_lpCmdLine;
				if(cadena.Find("/t")!=-1) {
					archivo.Replace("/t ", "");
					archivo.Remove('\"');
					archivo= m_wndHelpBar.FindTitulo(archivo);
				}
				if(!archivo.IsEmpty())
					OnQueryNavigate(1, (LPARAM)(const char *) archivo );
				POSITION pos= AfxGetApp()->m_pDocManager->GetFirstDocTemplatePosition();
				CDocTemplate *temp = AfxGetApp()->m_pDocManager->GetNextDocTemplate(pos);
				pos= temp->GetFirstDocPosition();
				documento= temp->GetNextDoc(pos);
				break;
			}
		}
	}
	if(!documento) {
		CWaitCursor wait;
		AfxGetApp()->m_pDocManager->OnFileNew();
		if(!archivo.IsEmpty())
			return OnAbrir(archivo);
	}
	return documento;
}

void CMainFrame::OnAyudaAyuda()
{
	m_wndHelpBar.ShowWindow(SW_NORMAL);
	CString temp= CMyRegKey::GetProfileString("ayuda", "ruta", "ayuda\\");
	temp+="inicio.html";
	OnQueryNavigate(1, (LPARAM)(const char *) temp );
}

void CMainFrame::OnAyudaIndice()
{
	if( !m_wndHelpBar.IsWindowVisible())  {

		if(m_wndHelpBar.IsHide()) {

			m_wndHelpBar.InitShow();
			return;
		}
	}
	m_wndHelpBar.m_pDlgHelp.m_TabCtrl.ActivateSSLPage(0);
	if( !m_wndHelpBar.IsWindowVisible())
		m_wndHelpBar.ShowWindow((m_wndHelpBar.IsWindowVisible()) == 0);
	RecalcLayout();
}

void CMainFrame::OnAyudaBuscar()
{
	if( !m_wndHelpBar.IsWindowVisible()) {

		if(m_wndHelpBar.IsHide()) {

			m_wndHelpBar.InitShow();
			return;
		}
	}
	m_wndHelpBar.m_pDlgHelp.m_TabCtrl.ActivateSSLPage(1);
	if( !m_wndHelpBar.IsWindowVisible())
		m_wndHelpBar.ShowWindow((m_wndHelpBar.IsWindowVisible()) == 0);
	RecalcLayout();
}

void CMainFrame::OnUpdateAyudaIndice(CCmdUI *pCmdUI)
{
	if(m_wndHelpBar.IsWindowVisible()) 
		pCmdUI->SetRadio(m_wndHelpBar.m_pDlgHelp.m_TabCtrl.GetCurSel()==0);
}

void CMainFrame::OnUpdateAyudaBuscar(CCmdUI *pCmdUI)
{
	if(m_wndHelpBar.IsWindowVisible()) 
		pCmdUI->SetRadio(m_wndHelpBar.m_pDlgHelp.m_TabCtrl.GetCurSel()==1);
}

LRESULT CMainFrame::OnQueryNavigate(WPARAM wParam, LPARAM lParam) {

	if(wParam==0) 
		m_wndHelpBar.m_pDlgHelp.SendMessage(WM_HHELP, wParam, lParam);
	else SendMessageBroadCast(WM_HHELP, wParam, lParam);
	return 0;
}
