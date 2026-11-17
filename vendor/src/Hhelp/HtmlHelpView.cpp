// HtmlHelp.cpp: archivo de implementación
//

#include "stdafx.h"
#include "Hhelp.h"
#include "HtmlHelpview.h"
# include <resourcedll.h>
// CHtmlHelpView

# define ID_HTML_AYUDA 1342
# define ID_ATRAS		1231
# define ID_CANCELAR	1232
# define ID_ADELANTE	1233
# define ID_RELOAD		1234
# define ID_HOME		1235

IMPLEMENT_DYNCREATE(CHtmlHelpView, CFixHtmlView)

CHtmlHelpView::CHtmlHelpView()
{
	m_bVisibleToolbar=false;
	m_bCanBack= m_bCanForward=false;

	CString ruta= CMyRegKey::GetProfileString("ayuda", "ruta", "ayuda\\");
	m_szAppPath= CApplicacion::GetAppPath(ruta);

	m_szInitialURL= AfxGetApp()->GetProfileString("search", "last",m_szAppPath + ruta + "inicio.html");
}

CHtmlHelpView::~CHtmlHelpView()
{
}

void CHtmlHelpView::DoDataExchange(CDataExchange* pDX)
{
	CFixHtmlView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CHtmlHelpView, CFixHtmlView)
	ON_MESSAGE(WM_HHELP, OnQueryNavigate)
	ON_COMMAND(ID_ATRAS, OnAtras)
	ON_COMMAND(ID_ADELANTE, OnAdelante)
	ON_COMMAND(ID_CANCELAR, OnCancelar)
	ON_COMMAND(ID_RELOAD, OnReload)
	ON_COMMAND(ID_HOME, OnHome)
	ON_UPDATE_COMMAND_UI(ID_ADELANTE, OnUpdateAdelante)
	ON_UPDATE_COMMAND_UI(ID_ATRAS, OnUpdateAtras)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// Diagnósticos de CHtmlHelpView

#ifdef _DEBUG
void CHtmlHelpView::AssertValid() const
{
	CFixHtmlView::AssertValid();
}

void CHtmlHelpView::Dump(CDumpContext& dc) const
{
	CFixHtmlView::Dump(dc);
}
#endif //_DEBUG


// Controladores de mensajes de CHtmlHelpView

void CHtmlHelpView::OnInitialUpdate()
{
	CWaitCursor wait;
	CFixHtmlView::OnInitialUpdate();

	CMDIChildWndXP *temp= (CMDIChildWndXP *)GetParent();
	if(temp->IsKindOf(RUNTIME_CLASS(CMDIChildWndXP))) 
		temp->MDIMaximize();
	GetDocument()->SetTitle("Ayuda");

	if(!m_szInitialURL.IsEmpty())
		Navigate2(m_szInitialURL);

	CMDIFrameWndXP *main= (CMDIFrameWndXP *)AfxGetMainWnd();
	ASSERT(main);
	
	CResourceDLL *res= new CResourceDLL();

	CMFCRibbonCategory* pCategory = main->m_wndRibbonBar.AddContextCategory("Navegación", "Navegación", ID_HTML_AYUDA, AFX_CategoryColor_Orange, IDB_BSMALL,IDB_BGRANDE);
	CMFCRibbonPanel* pPanel = pCategory->AddPanel(_T("Navegación"));
	pPanel->Add(new CMFCRibbonButton(ID_ATRAS, _T("Atras"), 0, 0));
	pPanel->Add(new CMFCRibbonButton(ID_ADELANTE, _T("Adelante"), 1, 1));
	pPanel->Add(new CMFCRibbonButton(ID_HOME, _T("Inicio"), 4, 4));
	pPanel->Add(new CMFCRibbonButton(ID_RELOAD, _T("Actualizar"), 3, 3));
	pPanel->Add(new CMFCRibbonButton(ID_CANCELAR, _T("Detener"), 2, 2));

	delete res;
}

LRESULT CHtmlHelpView::OnQueryNavigate(WPARAM wParam, LPARAM lParam) {

	if( wParam==1) {
        CString pagina= (const char *) lParam;
		Navigate2(m_szAppPath + pagina);
	}
	return 0;
}

void CHtmlHelpView::OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags, LPCTSTR lpszTargetFrameName, CByteArray& baPostedData, LPCTSTR lpszHeaders, BOOL* pbCancel)
{
	CWnd *main= AfxGetMainWnd();
	if(main && ::IsWindow(*main)) {
		main->SendMessage(WM_STATUSBAR, 5, 1);
		main->SendMessage(WM_HHELP, 0, (LPARAM) (const char *)lpszURL);
	}
	CFixHtmlView::OnBeforeNavigate2(lpszURL, nFlags, lpszTargetFrameName, baPostedData, lpszHeaders, pbCancel);
}


void CHtmlHelpView::OnAtras() {

	GoBack();
}

void CHtmlHelpView::OnAdelante() {

	GoForward();
}

void CHtmlHelpView::OnCancelar() {

	Stop();
}

void CHtmlHelpView::OnReload() {

	Refresh();
}

void CHtmlHelpView::OnHome() {

	CString ruta= CMyRegKey::GetProfileString("ayuda", "ruta", "ayuda\\");
	Navigate2( m_szAppPath + ruta + "inicio.html");
}

void CHtmlHelpView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	CMDIFrameWndXP *main= (CMDIFrameWndXP *)AfxGetMainWnd();
	
	bool recal= main->m_wndRibbonBar.HideAllContextCategories();
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

			CView *view= ((CMDIChildWnd *) mdi)->GetActiveView();
			if(view==this) {
				main->m_wndRibbonBar.ShowContextCategories(ID_HTML_AYUDA);
				if(main->m_wndRibbonBar.GetHideFlags()==0)
					main->m_wndRibbonBar.ActivateContextCategory(ID_HTML_AYUDA);
				recal=true;
			}
		}
	}
	if(recal) {
		main->m_wndRibbonBar.RecalcLayout();
		main->m_wndRibbonBar.RedrawWindow();
	}
	CFixHtmlView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CHtmlHelpView::OnTitleChange(LPCTSTR lpszText)
{
	CWnd *main= AfxGetMainWnd();
	if(main) {
		GetDocument()->SetTitle(CApplicacion::GetAppName());
		main->SetWindowText(lpszText);
	} else {
		CString titulo= lpszText;
		titulo+=" - ";
		titulo+=CApplicacion::GetAppName(); 
		GetDocument()->SetTitle(titulo);
	}
	CFixHtmlView::OnTitleChange(lpszText);
}

void CHtmlHelpView::OnDestroy()
{
	CMDIFrameWndXP *main= (CMDIFrameWndXP *)AfxGetMainWnd();
	if(::IsWindow(main->m_wndRibbonBar)) {
		for(int i=0; i<main->m_wndRibbonBar.GetCategoryCount(); i++) {
			
			CMFCRibbonCategory *cat= main->m_wndRibbonBar.GetCategory(i);
			if(cat->GetContextID()==ID_HTML_AYUDA) {

				main->m_wndRibbonBar.RemoveCategory(i);
				i--;
			}
		}
		main->m_wndRibbonBar.RecalcLayout();
		main->m_wndRibbonBar.RedrawWindow();
	}
	if(main && ::IsWindow(*main)) 
		main->SendMessage(WM_STATUSBAR, 5, 0);
	CFixHtmlView::OnDestroy();
}

void CHtmlHelpView::OnUpdateAdelante(CCmdUI* pCmdUI) {

	pCmdUI->Enable(m_bCanForward);
}

void CHtmlHelpView::OnUpdateAtras(CCmdUI* pCmdUI) {

	pCmdUI->Enable(m_bCanBack);
}

void CHtmlHelpView::OnDocumentComplete(LPCTSTR lpszURL)
{
	CWnd *main= AfxGetMainWnd();
	if(main && ::IsWindow(*main)) 
		main->SendMessage(WM_STATUSBAR, 5, 0);
	CFixHtmlView::OnDocumentComplete(lpszURL);
}

void CHtmlHelpView::OnCommandStateChange(long nCommand, BOOL bEnable) {

	switch(nCommand) {
		case CSC_NAVIGATEFORWARD:
			m_bCanForward=bEnable;
			break;
		case CSC_NAVIGATEBACK:
			m_bCanBack=bEnable;
			break;
	}
}
