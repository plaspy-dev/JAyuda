// Template.cpp : define los comportamientos de las clases para la aplicación.
//

#include "stdafx.h"
#include "Template.h"
#include "MainFrm.h"

#include "TemplateView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTemplateApp

BEGIN_MESSAGE_MAP(CTemplateApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
END_MESSAGE_MAP()


// Construcción de CTemplateApp

CTemplateApp::CTemplateApp()
{
}

CTemplateApp theApp;


BOOL CTemplateApp::InitInstance()
{
	CString myid;
	myid.Format("{6A97833B-7441-41%d-B96A-B9D28D4B4DF9}",CApplicacion::GetID());
	if(!CWinAppEx::InitInstance(myid))
		return false;

	AfxEnableControlContainer();

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Inicializar bibliotecas OLE
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	
#ifdef ENABLE_LICENSE_CHECK
	SetRegistryKey(_T("SDG"));
#else
	SetRegistryKey(_T("JAyuda\\1.1"));
#endif

	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;

	m_pMainWnd = pMainFrame;

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	// quitar de aqui
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	return TRUE;
}

void CTemplateApp::OnFileNew() {

	if(m_pMainWnd)
		((CMainFrame *) m_pMainWnd)->OnAbrir("");
	PRINTDLG pd;
	pd.lStructSize=(DWORD)sizeof(PRINTDLG);
	BOOL bRet=GetPrinterDeviceDefaults(&pd);
	if(bRet)
	{
		// protect memory handle with ::GlobalLock and ::GlobalUnlock
		DEVMODE FAR *pDevMode=(DEVMODE FAR *)::GlobalLock(m_hDevMode);
		// set orientation to landscape
		pDevMode->dmOrientation= GetProfileInt("printer", "orientacion", DMORIENT_PORTRAIT);
		pDevMode->dmPaperSize= GetProfileInt("printer", "papel", DMPAPER_LETTER);
		::GlobalUnlock(m_hDevMode);
	}
}

void CTemplateApp::OnFileOpen() {

}

CDocument* CTemplateApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
	if(m_pMainWnd)
		return ((CMainFrame *) m_pMainWnd)->OnAbrir(lpszFileName);
	return NULL;
}

void CTemplateApp::OnFileSave()
{
}

// Cuadro de diálogo CAboutDlg utilizado para el comando Acerca de

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Datos del cuadro de diálogo
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Compatibilidad con DDX/DDV

// Implementación
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CHyperLink m_SLink;
	CStatic		m_SVersion;
	CKCSideBannerWnd m_banner;
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SVERSION, m_SVersion);
	DDX_Control(pDX, IDC_SLINKEX, m_SLink);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// Comando de la aplicación para ejecutar el cuadro de diálogo
void CTemplateApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// Controladores de mensaje de CTemplateApp


BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_SLink.SetURL("http://equipo-2.example.invalid");

	m_banner.Attach(this, KCSB_ATTACH_TOP);
	m_banner.SetFillFlag(KCSB_FILL_TEXTURE);
	m_banner.SetSize(80);

	CEnBitmap bitmap;
	bitmap.CopyImage(CApplicacion::GetBitmap());
	CRect rect;
	m_banner.GetWindowRect(rect);
	bitmap.ResizeImage(rect.Width(),rect.Height());
	m_banner.SetTexture((HBITMAP)bitmap.Detach());

	return TRUE;
}
