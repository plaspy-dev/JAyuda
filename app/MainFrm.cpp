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

// Construcción y destrucción de CMainFrame

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndXP::OnCreate(lpCreateStruct) == -1)
		return -1;
	// barra de herramientas principal
	if (!m_wndHelpBar.Create(_T("Ayuda"), this, CRect(0, 0, 200, 200),TRUE, 514, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create my ODS Bar\n");
		return -1;
	}
	EnableMDITabs(false);
	m_wndHelpBar.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndHelpBar);
	
	SetWindowText("JAyuda - " + CApplicacion::GetAppName());

	LoadDocument(NULL, NULL, "Ayuda", IDR_TemplateTYPE,RUNTIME_CLASS(CHtmlHelpView));
	LoadState();

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
	if( !AfxGetApp()->GetProfileInt("barras", "store",0)) {
		m_wndHelpBar.ShowPane(true, false, false);
	}
	CMDIFrameWndXP::LoadState();
}

void CMainFrame::SaveState() {

	CMDIFrameWndXP::SaveState();
}

void CMainFrame::OnTimer(UINT nIDEvent)
{
	CMDIFrameWndXP::OnTimer(nIDEvent);
}

void CMainFrame::OnUpdateFileNew(CCmdUI *pCmdUI)
{
}

CDocument *CMainFrame::OnAbrir(CString archivo) {
	
	CDocument *documento=NULL;
	bool found=false;

	CWnd *mdi;
	for(mdi= AfxGetMainWnd()->GetTopWindow() ; mdi; mdi= mdi->GetNextWindow()) {
		char wndClass[32];
		::GetClassName(*mdi, wndClass, 32);
		if (strncmp(wndClass, "MDIClient", 32) == 0) 
			break;
	}
	if(mdi) {
		mdi= mdi->GetTopWindow();
		if( mdi&& mdi->IsKindOf(RUNTIME_CLASS(CMDIChildWnd))) {

			CView *vista= ((CMDIChildWnd *) mdi)->GetActiveView();
			((CMDIChildWnd *) mdi)->MDIMaximize();
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
	if(::IsWindow(m_wndHelpBar)) 
		m_wndHelpBar.ShowPane(true, false, true);
	m_wndHelpBar.m_pDlgHelp.m_TabCtrl.ActivateSSLPage(0);
}

void CMainFrame::OnAyudaBuscar()
{
	if(::IsWindow(m_wndHelpBar)) 
		m_wndHelpBar.ShowPane(true, false, true);
	m_wndHelpBar.m_pDlgHelp.m_TabCtrl.ActivateSSLPage(1);
}

void CMainFrame::OnUpdateAyudaIndice(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndHelpBar.IsWindowVisible() && m_wndHelpBar.m_pDlgHelp.m_TabCtrl.GetCurSel()==0);
}

void CMainFrame::OnUpdateAyudaBuscar(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndHelpBar.IsWindowVisible() && m_wndHelpBar.m_pDlgHelp.m_TabCtrl.GetCurSel()==1);
}

LRESULT CMainFrame::OnQueryNavigate(WPARAM wParam, LPARAM lParam) {

	if(wParam==0) 
		m_wndHelpBar.m_pDlgHelp.SendMessage(WM_HHELP, wParam, lParam);
	else {
		CWnd *mdi;
		for(mdi= AfxGetMainWnd()->GetTopWindow() ; mdi; mdi= mdi->GetNextWindow()) {
			char wndClass[32];
			::GetClassName(*mdi, wndClass, 32);
			if (strncmp(wndClass, "MDIClient", 32) == 0) 
				break;
		}
		if(mdi) {
			mdi= mdi->GetTopWindow();
			if( mdi&& mdi->IsKindOf(RUNTIME_CLASS(CMDIChildWnd))) {

				CView *vista= ((CMDIChildWnd *) mdi)->GetActiveView();
				((CMDIChildWnd *) mdi)->MDIMaximize();
				if(vista)
					vista->SendMessage(WM_HHELP, wParam, lParam);
			}
		}
	}
	return 0;
}


void CMainFrame::InitializeRibbon()
{
	// Load panel images:
	m_PanelImages.SetImageSize(CSize(16, 16));
	m_PanelImages.Load(IDB_BUTTONS);

	// Init main button:
	m_MainButton.SetImage(IDB_MAIN);
	m_MainButton.SetText(_T("\nf"));
	m_MainButton.SetToolTipText("Archivo");

	m_wndRibbonBar.SetApplicationButton(&m_MainButton, CSize (45, 45));
	CMFCRibbonMainPanel* pMainPanel = m_wndRibbonBar.AddMainCategory("Archivo", IDB_FILESMALL, IDB_FILELARGE);

	pMainPanel->Add(new CMFCRibbonButton(ID_FILE_SAVE_AS, "Guardar Como", 3, 3));
	CMFCRibbonButton* pBtnPrint = new CMFCRibbonButton(ID_FILE_PRINT, "Imprimir", 6, 6);
	pBtnPrint->SetKeys(_T("p"), _T("w"));
	pBtnPrint->AddSubItem(new CMFCRibbonLabel("Previsualizar e imprimir el documento"));
	pBtnPrint->AddSubItem(new CMFCRibbonButton(ID_FILE_PRINT_DIRECT, "&Impresión rapida", 7, 7, TRUE));
	pBtnPrint->AddSubItem(new CMFCRibbonButton(ID_FILE_PRINT_PREVIEW, "&Vista preliminar", 8, 8, TRUE));
	pBtnPrint->AddSubItem(new CMFCRibbonButton(ID_FILE_PRINT_SETUP, "&Configurar página", 11, 11, TRUE));
	pMainPanel->Add(pBtnPrint);

	pMainPanel->AddRecentFilesList("");
	pMainPanel->AddToBottom(new CMFCRibbonMainPanelButton(ID_APP_EXIT, "Salir", 15));
	// este es el menu principal


	// Add "Home" category with "Clipboard" panel:
	CMFCRibbonCategory* pCategoryHome = m_wndRibbonBar.AddCategory("Inicio", IDB_WRITESMALL, IDB_WRITELARGE);

	// crear panel de impresion
	CMFCRibbonPanel* pPanelImpresion = pCategoryHome->AddPanel("Impresión\ne", m_PanelImages.ExtractIcon(27));
	pPanelImpresion->Add(new CMFCRibbonButton(ID_FILE_PRINT, "Imprimir\nv", 11, 2));

	// Create "Clipboard" panel:
	CMFCRibbonPanel* pPanelClipboard = pCategoryHome->AddPanel("Portapapeles\nd", m_PanelImages.ExtractIcon(27));

	pPanelClipboard->Add(new CMFCRibbonButton(ID_EDIT_COPY, "Copiar\nc", 2));

	// Create and add a "View" panel:
	CMFCRibbonPanel* pPanelView = pCategoryHome->AddPanel("Ver", m_PanelImages.ExtractIcon (7));

	CMFCRibbonButton* pBtnStatusBar = new CMFCRibbonCheckBox(ID_VIEW_STATUS_BAR, "Barra de estado");
	pPanelView->Add(pBtnStatusBar);
	pPanelView->Add(new CMFCRibbonCheckBox(ID_AYUDA_INDICE, "Índice"));
	pPanelView->Add(new CMFCRibbonCheckBox(ID_AYUDA_BUSCAR, "Buscar"));

	// crear panel de impresion
	CMFCRibbonPanel* pPanelAyuda = pCategoryHome->AddPanel("Acerca de\ne", m_PanelImages.ExtractIcon(27));
	pPanelAyuda->Add(new CMFCRibbonButton(ID_APP_ABOUT, "Acerca de\nv", 12, 3));


	// Add quick access toolbar commands:
	CList<UINT, UINT> lstQATCmds;

	lstQATCmds.AddTail(ID_FILE_PRINT);

	m_wndRibbonBar.SetQuickAccessCommands(lstQATCmds);
	m_wndRibbonBar.AddToTabs(new CMFCRibbonButton(ID_APP_ABOUT, _T("\na"), m_PanelImages.ExtractIcon(0)));

	CMDIFrameWndXP::InitializeRibbon();
}
