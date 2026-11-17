// DlgWebContrasena.cpp : implementation file
//

#include "stdafx.h"
#include "Conector.h"
#include "DlgWebContrasena.h"


// CDlgWebContrasena dialog

IMPLEMENT_DYNAMIC(CDlgWebContrasena, CDialog)

CDlgWebContrasena::CDlgWebContrasena(const CString &url, CWnd* pParent, int ancho, int alto)
	: CDialog(CDlgWebContrasena::IDD, pParent)
{
	m_szURL= url;
	m_pAfxMainWnd=pParent;
	m_nAncho= ancho;
	m_nAlto= alto;
}

CDlgWebContrasena::~CDlgWebContrasena()
{
}

void CDlgWebContrasena::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgWebContrasena, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgWebContrasena::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgWebContrasena::OnBnClickedCancel)
	ON_WM_DESTROY()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgWebContrasena message handlers

void CDlgWebContrasena::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	// OnOK();
}

void CDlgWebContrasena::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

BOOL CDlgWebContrasena::OnInitDialog()
{
	AfxEnableControlContainer();

	CDialog::OnInitDialog();
	SetWindowText("Cargando...");

	VERIFY(m_EWebBrowser.CreateFromStatic(IDC_EWEBBROWSER, this));

	m_EWebBrowser.Navigate(m_szURL);

	CRect rect;
	GetWindowRect(rect);
	if(m_nAlto!=-1) {
		rect.bottom=m_nAlto;
	}
	if(m_nAncho!=-1) {
		rect.right=m_nAncho;
	}
	MoveWindow(rect);
	
	return TRUE;
}

BOOL CDlgWebContrasena::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if(wParam==m_EWebBrowser.GetDlgCtrlID()) {
		Notification *not= (Notification *)lParam;
		if(not->hdr.code==CHTMLCtrl::DocumentComplete) {
			CString resultado= GetLeftString(GetRightString(not->URL, "close="), "&");
			if(resultado=="true" || resultado=="http:///") {
				OnOK();
			} else if(resultado=="false") {
				OnCancel();
			}
			SetWindowText(m_EWebBrowser.GetLocationName());

			if(m_pAfxMainWnd && ::IsWindow(*m_pAfxMainWnd)) 
				m_pAfxMainWnd->SendMessage(WM_STATUSBAR, 5, 0);
		} else if(not->hdr.code==CHTMLCtrl::BeforeNavigate2) {

			SetWindowText("Cargando...");
			if(m_pAfxMainWnd && ::IsWindow(*m_pAfxMainWnd)) 
				m_pAfxMainWnd->SendMessage(WM_STATUSBAR, 5, 1);
		} else if(not->hdr.code==CHTMLCtrl::OnHomeRequested) {
			
			not->URL="";
			if(m_pAfxMainWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWndXP)))
				((CMDIFrameWndXP*)m_pAfxMainWnd)->SendMessageBroadCast(WM_NOTIFY, -1, lParam);
			if(!not->URL.IsEmpty())
				m_EWebBrowser.Navigate(not->URL);
			else OnCancel();
		}
	}
	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CDlgWebContrasena::OnDestroy()
{
	CMDIFrameWndXP *main= (CMDIFrameWndXP *)AfxGetMainWnd();
	if(main && ::IsWindow(*main)) 
		main->SendMessage(WM_STATUSBAR, 5, 0);
	CDialog::OnDestroy();
}

void CDlgWebContrasena::OnSize(UINT nType, int cx, int cy) {

	m_EWebBrowser.MoveWindow(5,5,cx-10, cy-10);
	CDialog::OnSize(nType, cx, cy);
}
