#include "StdAfx.h"
# include "resource.h"
# include "Componentes.h"
# include <ResourceDLL.h>
#include "newmenu/mdiframewndxp.h"
# include "newmenu/MultiDocTemplateXP.h"
# include "atlbase.h"
# include "ComponentesDoc.h"

# define	TIMER_RESTORE_STATUS_BAR	123
# define	MESSAGE_DELAY				3000
# define	TIMER_UPDATE_TREE			3001
# define	TIMER_UPDATE_TOOLBAR		3002

class CMFCRibbonStatusBarPaneEx : public CMFCRibbonStatusBarPane {

	CMFCRibbonStatusBar *m_pParent;
	COLORREF  m_clrBackground;
	COLORREF  m_clrForeground;
public:
	CMFCRibbonStatusBarPaneEx(
		UINT     nCmdID,                     // Pane command id
		LPCTSTR  lpszText,                   // Pane label
		BOOL     bIsStatic = FALSE,
		CMFCRibbonStatusBar *parent=NULL) 
		: CMFCRibbonStatusBarPane(nCmdID, lpszText, bIsStatic, NULL, "") {// The almost large text in pane

			m_pParent= parent;
			m_clrBackground=-1;
			m_clrForeground=-1;
	}

	void SetFgColor(COLORREF foreground=-1) {

		m_clrForeground= foreground;
	}
	void SetBackground(COLORREF background=-1) {
		m_clrBackground= background;
	}
	virtual void OnCalcTextSize(CDC* pDC) {

		CMFCRibbonStatusBarPane::OnCalcTextSize(pDC);
		if(m_pParent) {
			m_sizeTextRight.cx = m_pParent->GetSpace() - 2*m_szMargin.cx;
		}
	}

	virtual COLORREF OnFillBackground(CDC* pDC) {

		COLORREF temp= CMFCRibbonStatusBarPane::OnFillBackground(pDC);
		if(m_clrBackground!=-1) {

			CRect rect =GetRect();
			pDC->FillSolidRect(rect, m_clrBackground);
		}
		if(m_clrForeground!=-1)
			temp=m_clrForeground;
		return temp;
	}
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CMDIFrameWndXP,CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMDIFrameWndXP, CMDIFrameWndEx)
      ON_WM_TIMER()
	  ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	  ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	  ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	  ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	  ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	  ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	  ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	  ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	  ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	  ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	  ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	  ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	  ON_UPDATE_COMMAND_UI_RANGE(VENTANAS_PREFIJO, VENTANAS_PREFIJO+50, OnUpdateVentanas)
	  ON_COMMAND_RANGE(VENTANAS_PREFIJO, VENTANAS_PREFIJO+50, OnCommandVentanas)
	  ON_MESSAGE(WM_TREENAVEGADOR,OnTreeNavegador)
	  ON_MESSAGE(WM_STATUSBAR, OnStatusBar)
	  ON_MESSAGE(WM_SETSTATUS,OnSetStatus)
	  ON_MESSAGE(WM_TREADRESULT,OnThreadResult)
	  ON_UPDATE_COMMAND_UI_RANGE(ACELERADORES_PREFIJO, ACELERADORES_PREFIJO+50, OnUpdateMenus)
	  ON_COMMAND_RANGE(ACELERADORES_PREFIJO, ACELERADORES_PREFIJO+50, OnCommandMenu)
	  ON_WM_CLOSE()
	  ON_COMMAND(ID_VER_NAVEGADOR, OnVerNavegador)
	  ON_UPDATE_COMMAND_UI(ID_VER_NAVEGADOR, OnUpdateVerNavegador)
	  ON_MESSAGE(WM_QUERYTIME, OnQueryTime)
	  ON_COMMAND(ID_NEXT_PANEEX, OnNextPane)
	  ON_UPDATE_COMMAND_UI(ID_NEXT_PANEEX, OnUpdateNextPane)
	  ON_COMMAND(ID_PREV_PANEEX, OnPrevPane)
	  ON_UPDATE_COMMAND_UI(ID_PREV_PANEEX, OnUpdatePrevPane)
	  ON_COMMAND(ID_WINDOW_CLOSE_ALL, OnCloseAll)
	  ON_COMMAND(ID_HELP, OnHelpIndex)
	  ON_COMMAND(ID_HELP_INDEX, OnHelpIndex)
	  ON_COMMAND(ID_HELP_FINDER, OnHelpIndex)
	  ON_COMMAND(ID_WINDOW_MANAGER, &CMDIFrameWndXP::OnWindowManager)
	  ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMDIFrameWndXP::OnApplicationLook)
	  ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMDIFrameWndXP::OnUpdateApplicationLook)
	  ON_WM_CREATE()
END_MESSAGE_MAP()

CMDIFrameWndXP::CMDIFrameWndXP() {

	m_nAncho=0;
	m_nAlto=0;
	m_dwBkColor=-1;
	m_pFlash=NULL;
	m_ulTotal=0;
	m_ulRealizados=0;
	m_nCurrentHilos=0;
	m_nAnimation=1;
	m_pTLocal= m_pTServidor= CTime::GetCurrentTime();
	m_nInactivo= 0;
	m_bInactivo= false;
	m_bFocus= true;
	m_bConectando= false;
	m_pFadeWnd= NULL;
	((CWinAppXP *)AfxGetApp())->m_nAppLook = ((CWinAppXP *)AfxGetApp())->GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLUE);

	m_LogoFont.CreateFont(36, 0, 0, 0, FW_BOLD, 1, FALSE, FALSE,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		FIXED_PITCH | FF_ROMAN, "Times New Roman");

	CResourceDLL dll;
	m_hBitmap= CEnBitmap::LoadImageResource(IDR_FONDO, "foto",AfxGetResourceHandle());
	ASSERT(m_hBitmap);
	if(m_hBitmap)
		::GetObject(m_hBitmap, sizeof(BITMAP), &m_bmInfo);
}

CMDIFrameWndXP::~CMDIFrameWndXP() {
	
	CMiscelaneosEx *info;
	if(m_pFadeWnd) {
		delete m_pFadeWnd;
		m_pFadeWnd=NULL;
	}
	if(m_hBitmap) 
		::DeleteObject(m_hBitmap);
	m_LogoFont.DeleteObject();
	if(m_pFlash)
		delete m_pFlash;
}

  void CMDIFrameWndXP::OnTimer(UINT_PTR nIDEvent)
  {
    if( nIDEvent==TIMER_RESTORE_STATUS_BAR) {

		KillTimer(TIMER_RESTORE_STATUS_BAR);
		CMFCRibbonStatusBarPaneEx *elemento= (CMFCRibbonStatusBarPaneEx *)m_wndStatusBar.FindElement(ID_STATUSBAR_TEXTO);
		if(elemento) {
			elemento->SetBackground();
			elemento->SetFgColor();
			elemento->Redraw();
		}
	} else if(nIDEvent==TIMER_UPDATE_TREE) {
		KillTimer(TIMER_UPDATE_TREE);
		CMFCTabCtrl* pPrevActiveWnd = m_wndClientArea.FindActiveTabWnd() ? m_wndClientArea.FindActiveTabWnd() : &m_wndClientArea.GetMDITabs();
		int sel= pPrevActiveWnd->GetActiveTab();
		if( sel!=-1) {

			CWnd *tab= pPrevActiveWnd->GetTabWnd(sel);
			if(((CMDIChildWnd *)tab)->GetActiveView())
				m_pNavBar.SetSel(((CMDIChildWnd *)tab)->GetActiveView()->GetRuntimeClass()->m_lpszClassName);
		}
	} else if(nIDEvent==TIMER_UPDATE_TOOLBAR) {

		CMFCRibbonProgressBar *progressbar= (CMFCRibbonProgressBar *)m_wndStatusBar.FindElement(ID_STATUSBAR_BARRADEPROGRESO);
		if(!progressbar) 
			return;
		int pos= progressbar->GetPos();
		if(pos==100)
			pos=0;
		pos++;
		progressbar->SetPos(pos);
	}
	CMDIFrameWndEx::OnTimer(nIDEvent);
  }

 void CMDIFrameWndXP::OnEditPaste()
 {
	 GetFocus()->SendMessage(WM_PASTE);
 }

 void CMDIFrameWndXP::OnEditCopy()
 {
	 GetFocus()->SendMessage(WM_COPY);
 }

 void CMDIFrameWndXP::OnEditCut()
 {
	 GetFocus()->SendMessage(WM_CUT);
 }

 void CMDIFrameWndXP::OnEditUndo()
 {
	 GetFocus()->SendMessage(WM_UNDO);
 }

 void CMDIFrameWndXP::OnEditClear()
 {
	 GetFocus()->SendMessage(WM_CLEAR);
 }

 void CMDIFrameWndXP::OnEditSelectAll()
 {
	 CWnd *focus= GetFocus();
	 if( focus) {
		 if( focus->IsKindOf(RUNTIME_CLASS(CEdit))) 
			 ((CEdit *)focus)->SetSel(0, focus->GetWindowTextLength());
		 else  if( focus->IsKindOf(RUNTIME_CLASS(CRichEditCtrl))) 
			 ((CRichEditCtrl*) focus)->SetSel(0, focus->GetWindowTextLength());
		 else focus->SendMessage(WM_COMMAND, ID_EDIT_SELECT_ALL,0);
	 }
 }

 void CMDIFrameWndXP::OnUpdateEditPaste(CCmdUI *pCmdUI)
 {
	 CWnd *focus=GetFocus();
	 bool enable=false;
	 if( focus && focus->IsKindOf(RUNTIME_CLASS(CEdit))) {
		 if(!(focus->GetStyle()&ES_READONLY)) {
			 COleDataObject	obj;
			 if (obj.AttachClipboard() && obj.IsDataAvailable(CF_TEXT))
				 enable=true;
		 }
	 } else enable=true;
	 pCmdUI->Enable( enable);
 }

 void CMDIFrameWndXP::OnUpdateEditUndo(CCmdUI *pCmdUI)
 {
	 bool enable= false;
	 CWnd *focus= GetFocus();
	 if( focus) 
		 if(focus->IsKindOf(RUNTIME_CLASS(CEdit))) 
			 enable= ((CEdit *)focus)->CanUndo();
		 else if( focus->IsKindOf(RUNTIME_CLASS(CListCtrl)))
			 enable= ((CListCtrl*)focus)->GetItemCount()!=0;
		 else enable=true;
		 pCmdUI->Enable(enable);
 }

 void CMDIFrameWndXP::OnUpdateEditCopy(CCmdUI *pCmdUI)
 {
	 CWnd *focus= GetFocus();
	 int x=0, y=0;
	 if( focus) 
		 if(focus->IsKindOf(RUNTIME_CLASS(CEdit)) || focus->IsKindOf(RUNTIME_CLASS(CRichEditCtrl)))
			 ((CEdit *)focus)->GetSel(x,y);
		 else if( focus->IsKindOf(RUNTIME_CLASS(CListCtrl)))
			 y= ((CListCtrl *)focus)->GetItemCount();
		 else y= 1;
		 pCmdUI->Enable(x!=y);
 }

 void CMDIFrameWndXP::OnUpdateEditCut(CCmdUI *pCmdUI)
 {
	 CWnd *focus= GetFocus();
	 int x=0, y=0;
	 if( focus) {
		 if(focus->IsKindOf(RUNTIME_CLASS(CEdit)) || focus->IsKindOf(RUNTIME_CLASS(CRichEditCtrl))) {
			 if(!(focus->GetStyle()&ES_READONLY)) {
				 ((CEdit *)focus)->GetSel(x,y);
			 }
		 } else if( focus->IsKindOf(RUNTIME_CLASS(CListCtrl))) {
			 y=((CListCtrl *)focus)->GetItemCount();
		 } else y=1;
	 }
	 pCmdUI->Enable(x!=y);
 }

 void CMDIFrameWndXP::OnUpdateEditClear(CCmdUI *pCmdUI)
 {
	 bool enable=false;
	 CWnd *focus= GetFocus();
	 if( focus) {
		 if( focus->IsKindOf(RUNTIME_CLASS(CEdit))) {
			 if(!(focus->GetStyle()&ES_READONLY)) {
				 enable= focus->GetWindowTextLength()>0;
			 }
		 } else if( focus->IsKindOf(RUNTIME_CLASS(CListCtrl))) {
			 enable= ((CListCtrl *)focus)->GetItemCount()!=0;
		 } else enable=true;
	 }
	 pCmdUI->Enable(enable);
 }

 void CMDIFrameWndXP::OnUpdateEditSelectAll(CCmdUI *pCmdUI)
 {
	 bool enable=false;
	 CWnd *focus= GetFocus();
	 if( focus) {
		 if(focus->IsKindOf(RUNTIME_CLASS(CEdit)))
			 enable= focus->GetWindowTextLength()>0;
		 else if (focus->IsKindOf(RUNTIME_CLASS(CListCtrl)))
			 enable= ((CListCtrl *)focus)->GetItemCount()!=0;
		 else if( focus->IsKindOf(RUNTIME_CLASS(CRichEditCtrl)))
			 enable= focus->GetWindowTextLength()>0;
		 else enable=true;
	 }
	 pCmdUI->Enable(enable);
 }

 void CMDIFrameWndXP::DockControlBarLeftOf(CToolBar* Bar, CToolBar* LeftOf)
 {
	 CRect rect;
	 DWORD dw;
	 UINT n;

	 RecalcLayout(TRUE);

	 LeftOf->GetWindowRect(&rect);
	 rect.OffsetRect(1,0);
	 dw=LeftOf->GetBarStyle();
	 n = 0;
	 n = (dw&CBRS_ALIGN_TOP) ? AFX_IDW_DOCKBAR_TOP : n;
	 n = (dw&CBRS_ALIGN_BOTTOM && n==0) ? AFX_IDW_DOCKBAR_BOTTOM : n;
	 n = (dw&CBRS_ALIGN_LEFT && n==0) ? AFX_IDW_DOCKBAR_LEFT : n;
	 n = (dw&CBRS_ALIGN_RIGHT && n==0) ? AFX_IDW_DOCKBAR_RIGHT : n;
	 DockControlBar(Bar,n,&rect);
 }

 void CMDIFrameWndXP::LoadState() {

	 AfxGetApp()->WriteProfileInt("control","close", 0);
	 CWaitCursor wait;
	 if(m_pFlash) 
		 m_pFlash->ShowWindow(SW_HIDE);
	 // cargar el estado de la barra de navegacion.
	 if( !AfxGetApp()->GetProfileInt("barras", "store",0)) {
		 m_pNavBar.ShowPane(true,false,false);
	 } else {
		 ((CWinAppEx *)AfxGetApp())->LoadState(this);
	 }

	 int nfocus;
	 CString szDocumentos=AfxGetApp()->GetProfileString("control", "classname");
	 CStringArray documentos;

	 while( (nfocus=szDocumentos.Find(","))>-1) {

		 documentos.Add(szDocumentos.Mid(0, nfocus));
		 szDocumentos.Delete(0, nfocus+1);
	 }
	 if( !szDocumentos.IsEmpty())
		 documentos.Add(szDocumentos);

	 nfocus= AfxGetApp()->GetProfileInt("control","focus", -1);

	 CDocManager *doc= AfxGetApp()->m_pDocManager;
	 CMultiDocTemplateXP *temp, *focus=NULL;
	 CString temporal;
	 if( !doc)
		 return;

	 for( int i=0;i<documentos.GetCount(); i++) {

		 temporal= documentos.GetAt(i);
		 POSITION pos= doc->GetFirstDocTemplatePosition();
		 while( pos) {

			 temp= (CMultiDocTemplateXP *) doc->GetNextDocTemplate(pos);
			 if( temp && temp->GetViewClassName()==temporal) {

				 PostMessage(WM_TREENAVEGADOR, NM_DBLCLK, (LPARAM) temp);
				 XSleep(100);
				 if( nfocus==i)
					 focus= temp;
				 break;
			 }
		 }
	 }
	 if( focus)
		 PostMessage(WM_TREENAVEGADOR, NM_DBLCLK, (LPARAM) focus);
	 if(m_pFlash) {
		CMFCTabCtrl* pPrevActiveWnd = m_wndClientArea.FindActiveTabWnd() ? m_wndClientArea.FindActiveTabWnd() : &m_wndClientArea.GetMDITabs();
		if(!pPrevActiveWnd->GetTabsNum())
			m_pFlash->ShowWindow(SW_SHOW);
	 }
 }

 bool CMDIFrameWndXP::AddAcelerator(ACCEL nueva) {

	 int cAcelerator= CopyAcceleratorTable(m_hAccelTable, NULL, 0)+1;
	 LPACCEL lpaccelnew= (LPACCEL) LocalAlloc( LPTR, cAcelerator* sizeof(ACCEL));
	 cAcelerator--;
	 if(!lpaccelnew)
		 return false;
	 CopyAcceleratorTable(m_hAccelTable, lpaccelnew, cAcelerator);
	 lpaccelnew[cAcelerator].cmd=nueva.cmd;
	 lpaccelnew[cAcelerator].fVirt= nueva.fVirt;
	 lpaccelnew[cAcelerator].key= nueva.key;
	 DestroyAcceleratorTable(m_hAccelTable);
	 cAcelerator++;
	 m_hAccelTable= CreateAcceleratorTable(lpaccelnew, cAcelerator);
	 ASSERT(CopyAcceleratorTable(m_hAccelTable, NULL, 0)==cAcelerator);
	 return true;
 }

 CString CMDIFrameWndXP::ParseAcelerator(CString label, int id) {

	 ACCEL nueva;
	 nueva.fVirt= FCONTROL | FNOINVERT | FVIRTKEY;
	 nueva.cmd= ACELERADORES_PREFIJO+id;
	 nueva.key= '1'+id;
	 CString temp;

	 if( nueva.key<='9') {
		 AddAcelerator(nueva);
		 temp.Format("%s\tCtrl+%c", label, nueva.key);
	 } else if( nueva.key=='9'+1) {
		 nueva.key='0';
		 AddAcelerator(nueva);
		 temp.Format("%s\tCtrl+%c", label, nueva.key);
	 } else if( nueva.key<='9'+10) {
		 nueva.key-=10;
		 nueva.fVirt= FALT | FNOINVERT | FVIRTKEY;
		 AddAcelerator(nueva);
		 temp.Format("%s\tAlt+%c", label, nueva.key);
	 } else if( nueva.key=='9'+11) {
		 nueva.key='0';
		 nueva.fVirt= FALT | FNOINVERT | FVIRTKEY;
		 AddAcelerator(nueva);
		 temp.Format("%s\tAlt+%c", label, nueva.key);
	 } else if( nueva.key<='9'+20) {
		 nueva.key-=20;
		 nueva.fVirt= FALT | FNOINVERT | FVIRTKEY | FCONTROL;
		 AddAcelerator(nueva);
		 temp.Format("%s\tCtrl+Alt+%c", label, nueva.key);
	 } else if( nueva.key=='9'+21) {
		 nueva.key='0';
		 nueva.fVirt= FALT | FNOINVERT | FVIRTKEY | FCONTROL;
		 AddAcelerator(nueva);
		 temp.Format("%s\tCtrl+Alt+%c", label, nueva.key);
	 } else temp= label;
	 return temp;
 }

 bool CMDIFrameWndXP::LoadDocument( CMenu *menu, HTREEITEM hRoot, CString nombre, UINT nIDResource,CRuntimeClass* pViewClass, const CMiscelaneosEx &permisos, DWORD grupo, CRuntimeClass *pDocClass/*=NULL*/) {

	 if(grupo==0)
		 return LoadDocument(menu, hRoot, nombre, nIDResource, pViewClass, pDocClass);
	 CMiscelaneosEx *info= permisos.Find(pViewClass->m_lpszClassName);
	 if(info && info->GetData())
		return LoadDocument(menu, hRoot, nombre, nIDResource, pViewClass, pDocClass);
	 return false;
 }

 bool CMDIFrameWndXP::LoadDocument( CMenu *menu, HTREEITEM hRoot, CString nombre, UINT nIDResource,CRuntimeClass* pViewClass, CRuntimeClass *pDocClass/*=NULL*/) {

	 HTREEITEM item= ::IsWindow(m_pNavBar) ? m_pNavBar.m_Tree.InsertItem( nombre, hRoot) : NULL;

	 CString temp;

	 CMultiDocTemplateXP* pDocTemplate;
	 pDocTemplate = new CMultiDocTemplateXP(nIDResource,pDocClass ? pDocClass : RUNTIME_CLASS(CComponentesDoc),RUNTIME_CLASS(CMDIChildWndXP), pViewClass);
	 AfxGetApp()->AddDocTemplate( pDocTemplate);

	 if(item) {
		temp= pViewClass->m_lpszClassName;
		m_pNavBar.m_Tree.SetItemData( item, (DWORD) pDocTemplate);
	 }
	 if(menu) {
		 m_pAceleradores.Add(temp, ACELERADORES_PREFIJO+m_pAceleradores.GetCount());
		 temp= ParseAcelerator(nombre, m_pAceleradores.GetCount()-1);
		 menu->AppendMenu( MF_STRING, ACELERADORES_PREFIJO+m_pAceleradores.GetCount()-1, temp);
	 }
	 return true;
 }

 void CMDIFrameWndXP::SaveState() {

	CString temp;
	 CMFCTabCtrl* pPrevActiveWnd = m_wndClientArea.FindActiveTabWnd() ? m_wndClientArea.FindActiveTabWnd() : &m_wndClientArea.GetMDITabs();

	 for(int i=0; i<pPrevActiveWnd->GetTabsNum(); i++) {

		 CWnd *ventana= pPrevActiveWnd->GetTabWnd(i);
		 if( !temp.IsEmpty())
			 temp+=",";
		 temp+=((CMDIChildWnd *)ventana)->GetActiveView()->GetRuntimeClass()->m_lpszClassName;
	 }
	 AfxGetApp()->WriteProfileString("control","classname",temp);
	 AfxGetApp()->WriteProfileInt("control","focus", pPrevActiveWnd->GetActiveTab());
	 AfxGetApp()->WriteProfileInt("barras", "store",1);

	 // guardamos el estado de las ventanas y borramos todas las barras laterales
	 ((CWinAppXP *)AfxGetApp())->SetRegistryBase("Workspace1");
	 ((CWinAppXP *)AfxGetApp())->SaveState(NULL, &m_Impl);
	 CMyRegKey key(((CWinAppXP *)AfxGetApp())->GetRegSectionPath());
	 CStringArray valores;
	 key.EnumRegKey("", valores);
	 for(int i=0; i<valores.GetCount(); i++) {

		 if(valores[i].Find("Pane")!=-1 || valores[i].Find("DockingManager")!=-1) 
			 key.DeleteRegKey(valores[i], "");
	 }
	 ((CWinAppXP *)AfxGetApp())->SetRegistryBase("Workspace");
 }

 BOOL CMDIFrameWndXP::PreCreateWindow(CREATESTRUCT& cs)
 {
	 if( AfxGetApp()->GetProfileInt("control", "loadstate", 0)) {
		 CRegKey key;
		 if(key.Open(HKEY_CURRENT_USER, CApplicacion::GetAppKey() +  "\\control")==ERROR_SUCCESS) {

			 key.DeleteValue("posicion");
			 key.DeleteValue("status");
			 key.DeleteValue("focus");
			 key.DeleteValue("classname");
			 key.Close();
		 }
		 if( key.Open(HKEY_CURRENT_USER, CApplicacion::GetAppKey())==ERROR_SUCCESS) {
			 if(key.RecurseDeleteKey("barras")!=ERROR_SUCCESS)
				 key.DeleteSubKey("barras");
			 if(key.RecurseDeleteKey("ingreso")!=ERROR_SUCCESS)
				 key.DeleteSubKey("ingreso");
			 if(key.RecurseDeleteKey("listas")!=ERROR_SUCCESS)
				 key.DeleteSubKey("listas");
			 if(key.RecurseDeleteKey("consulta")!=ERROR_SUCCESS)
				 key.DeleteSubKey("consulta");
			 key.Close();
		 }
	 }
	 return CMDIFrameWndEx::PreCreateWindow(cs);
 }

 void CMDIFrameWndXP::OnUpdateVentanas(CCmdUI *pCmdUI) {

	 CMFCTabCtrl* pPrevActiveWnd = m_wndClientArea.FindActiveTabWnd() ? m_wndClientArea.FindActiveTabWnd() : &m_wndClientArea.GetMDITabs();
	pCmdUI->Enable((pCmdUI->m_nID-VENTANAS_PREFIJO)<pPrevActiveWnd->GetTabsNum());
 }

 void CMDIFrameWndXP::OnCommandVentanas(UINT id) {

	 CMFCTabCtrl* pPrevActiveWnd = m_wndClientArea.FindActiveTabWnd() ? m_wndClientArea.FindActiveTabWnd() : &m_wndClientArea.GetMDITabs();
	 if((id-VENTANAS_PREFIJO)<pPrevActiveWnd->GetTabsNum())
		 pPrevActiveWnd->SetActiveTab(id-VENTANAS_PREFIJO);
 }

 void CMDIFrameWndXP::OnUpdateFrameTitle(BOOL bAddToTitle)
 {
	 FUNCTION_STACK_TRACE("AAAAAAAC");
	 CMFCTabCtrl* pPrevActiveWnd = m_wndClientArea.FindActiveTabWnd() ? m_wndClientArea.FindActiveTabWnd() : &m_wndClientArea.GetMDITabs();
	 CMDIFrameWndEx::OnUpdateFrameTitle(bAddToTitle);
	 KillTimer(TIMER_UPDATE_TREE);
	 SetTimer(TIMER_UPDATE_TREE,100, NULL);
	 if(m_pFlash)
		m_pFlash->ShowWindow(pPrevActiveWnd->GetTabsNum() ? SW_HIDE : SW_SHOW);
 }

 BOOL CMDIFrameWndXP::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
 {
	 if (CMDIFrameWndEx::OnCreateClient(lpcs, pContext)){  
		 return TRUE ;
	 }
	 return FALSE ;
 }

 LRESULT CMDIFrameWndXP::OnTreeNavegador(WPARAM wParam, LPARAM lParam) {

	 if( wParam== NM_DBLCLK) {

		 if( lParam) {
			 CWaitCursor wait;
			 CMultiDocTemplateXP* temp= (CMultiDocTemplateXP *)lParam;
			 ASSERT(temp);

			 CMFCTabCtrl* pPrevActiveWnd = m_wndClientArea.FindActiveTabWnd() ? m_wndClientArea.FindActiveTabWnd() : &m_wndClientArea.GetMDITabs();

			 for(int i=0; i<pPrevActiveWnd->GetTabsNum(); i++) {
				 CWnd *child= pPrevActiveWnd->GetTabWnd(i);
				 if(child && child->IsKindOf(RUNTIME_CLASS(CMDIChildWnd))) {
					 for(CWnd *view=((CMDIChildWnd *) child)->GetWindow(GW_CHILD); view; view= view->GetNextWindow()) {
						 if( view && view->GetRuntimeClass()->m_lpszClassName==temp->GetViewClassName()) {
							 child->BringWindowToTop();
							 child->SetFocus();
							 return 0;
						 }
					 }
				 }
			 }
			 if(m_pFlash)
				 m_pFlash->ShowWindow(SW_HIDE);
			 temp->OpenDocumentFile(NULL);
		 }
	 } else if( wParam==100 && ::IsWindow(m_pNavBar)) {
		 HTREEITEM item=m_pNavBar.FindItemEx((const char *)lParam);
		 if( item)
			 SendMessage(WM_TREENAVEGADOR, NM_DBLCLK, m_pNavBar.m_Tree.GetItemData(item));
	 }
	 return 0;
 }

 LRESULT CMDIFrameWndXP::OnStatusBar(WPARAM wParam, LPARAM lParam) {

	 CMFCRibbonProgressBar *progressbar= (CMFCRibbonProgressBar *)m_wndStatusBar.FindElement(ID_STATUSBAR_BARRADEPROGRESO);
	 if(!progressbar) 
		 return 0;

	 if( wParam==1 || wParam==4) { // agregamos un nuevo elemento
		 
		 m_ulRealizados= wParam==1 ? m_ulRealizados+1 : lParam;
		 if( m_ulTotal) {
			 int pos= (int) (m_ulRealizados*100)/m_ulTotal;
			 if(pos!=progressbar->GetPos())
				progressbar->SetPos(pos);
		 }
	 } else if( wParam==0) {// iniciamos la busqueda donde lparam es el total

		 m_ulTotal= lParam;
		 CMFCRibbonStatusBarPane *total= (CMFCRibbonStatusBarPane *)m_wndStatusBar.FindElement(ID_STATUSBAR_TOTAL);
		 if(total) {
			 CString temp;
			 temp.Format("%d Resultado", m_ulTotal);
			 if(m_ulTotal!=1)
				 temp+="s";
			 total->SetText(temp);
			 total->Redraw();
		 }
		 progressbar->SetPos(0);
	 } else if( wParam==2) { // terminamos la busqueda

		 progressbar->SetPos(0);
		 m_ulTotal=0;
		 m_ulRealizados=0;
	 } else if(wParam==5) {
		 if(lParam==1) {
			 KillTimer(TIMER_UPDATE_TOOLBAR);
			 progressbar->SetInfiniteMode(true);
			 progressbar->SetPos(0);
			 SetTimer(TIMER_UPDATE_TOOLBAR, 10,NULL);
			 CMFCRibbonStatusBarPane *total= (CMFCRibbonStatusBarPane *)m_wndStatusBar.FindElement(ID_STATUSBAR_TOTAL);
			 if(total) {
				 total->SetText("Cargando...");
				 total->Redraw();
			 }
		 } else {
			 KillTimer(TIMER_UPDATE_TOOLBAR);
			 progressbar->SetInfiniteMode(false);
			 progressbar->SetPos(0);
			 CMFCRibbonStatusBarPane *total= (CMFCRibbonStatusBarPane *)m_wndStatusBar.FindElement(ID_STATUSBAR_TOTAL);
			 if(total) {
				 total->SetText("");
				 total->Redraw();
			 }
		 }
	 } 
	 return 0;
 }

 bool CMDIFrameWndXP::LoadDocumentos(const CMiscelaneosEx &permisos, DWORD grupo) {

	 FUNCTION_STACK_TRACE("AAAAAAAB");

	 CMyRegKey key(HKEY_LOCAL_MACHINE, CApplicacion::GetAppKey() + "\\Addins");
	 CStringArray llaves;
	 key.EnumRegKey("", llaves);
	 for( int i=0; i<llaves.GetCount(); i++) {

		 CString path= key.GetString(llaves[i], "FullPath");
		 if(!path.IsEmpty()) {
			 LoadDocumentosEx(path, NULL, permisos, grupo);
		 }
	 }

	 /*CDocManager *doc= AfxGetApp()->m_pDocManager;
	 if( doc) {
		 POSITION pos= doc->GetFirstDocTemplatePosition();
		 CMultiDocTemplate *temp;
		 while( pos) {
			 temp= (CMultiDocTemplate *)doc->GetNextDocTemplate(pos);
			 if( temp)
				 OnCreateChild(temp->m_hMenuShared);
		 }
	 }*/
	 return true;
 }

 bool CMDIFrameWndXP::LoadDocumentos(DWORD parametro) {

	 FUNCTION_STACK_TRACE("AAAAAAAB");

	 CMyRegKey key(HKEY_LOCAL_MACHINE, CApplicacion::GetAppKey() + "\\Addins");
	 CStringArray llaves;
	 key.EnumRegKey("", llaves);
	 for( int i=0; i<llaves.GetCount(); i++) {
		 
		 CString path= key.GetString(llaves[i], "FullPath");
		 if(!path.IsEmpty()) {
			LoadDocumentosEx(path, NULL, parametro);
		 }
	 }

	 /*CDocManager *doc= AfxGetApp()->m_pDocManager;
	 if( doc) {
		 POSITION pos= doc->GetFirstDocTemplatePosition();
		 CMultiDocTemplate *temp;
		 while( pos) {
			 temp= (CMultiDocTemplate *)doc->GetNextDocTemplate(pos);
			 if( temp)
				 OnCreateChild(temp->m_hMenuShared);
		 }
	 }*/
	 return true;
 }

 LRESULT CMDIFrameWndXP::OnSetStatus(WPARAM wParam, LPARAM lParam) {

	 ASSERT_VALID(this);
	 CString temp= (const char *) lParam;
	 CMFCRibbonStatusBarPaneEx *elemento= (CMFCRibbonStatusBarPaneEx *)m_wndStatusBar.FindElement(ID_STATUSBAR_TEXTO);
	 if(elemento) {
		 elemento->SetBackground();
		 elemento->SetFgColor();
	 }
	 if( wParam>1) {
		 if(elemento) {
			 elemento->SetBackground(wParam==2 ? GetSysColor(COLOR_ACTIVECAPTION) : RGB(255,0,0));
			 elemento->SetFgColor(GetSysColor(COLOR_CAPTIONTEXT));
		 }
		 MessageBeep(MB_OK);
		 SetTimer(TIMER_RESTORE_STATUS_BAR, MESSAGE_DELAY, NULL);
	 }
	 if(elemento) {
		 elemento->SetText(temp);
		 elemento->Redraw();
	 }
	 return 0;
 } 

 LRESULT CMDIFrameWndXP::OnThreadResult(WPARAM wParam, LPARAM lParam) {

	switch( wParam) {
		case 0:
			m_nCurrentHilos--;
			if( m_nAnimation) {
				if(m_nCurrentHilos==0) {
					CMFCRibbonStatusBarPane *elemento= (CMFCRibbonStatusBarPane *)m_wndStatusBar.FindElement(ID_STATUSBAR_ANIMACIONPROGRESO);
					if(elemento) 
						elemento->StopAnimation();	
				}
			} else {
				CString temp;
				temp.Format("%d", m_nCurrentHilos);
				CMFCRibbonStatusBarPane *elemento= (CMFCRibbonStatusBarPane *)m_wndStatusBar.FindElement(ID_STATUSBAR_ANIMACIONPROGRESO);
				if(elemento) 
					elemento->SetText(temp);
			}
			PostMessage( WM_STATUSBAR, 2);
			break;
		case 1:
			m_nCurrentHilos++;
			if( m_nAnimation) {	
				if(m_nCurrentHilos==1) {
					CMFCRibbonStatusBarPane *elemento= (CMFCRibbonStatusBarPane *)m_wndStatusBar.FindElement(ID_STATUSBAR_ANIMACIONPROGRESO);
					if(elemento) 
						elemento->StartAnimation();
				}
			} else {
				CString temp;
				temp.Format("%d", m_nCurrentHilos);
				CMFCRibbonStatusBarPane *elemento= (CMFCRibbonStatusBarPane *)m_wndStatusBar.FindElement(ID_STATUSBAR_ANIMACIONPROGRESO);
				if(elemento) 
					elemento->SetText(temp);
			}
			break;
		default:
			{
				/*CString temp;
				temp.Format("conector\\servidor%d", (UINT)lParam);
				CMFCRibbonStatusBarPane *elemento= (CMFCRibbonStatusBarPane *)m_wndStatusBar.FindElement(ID_STATUSBAR_SERVIDOR);
				if(elemento) 
					elemento->SetText(AfxGetApp()->GetProfileString(temp, "servidor"));*/
			}
			break;
	 }
	 return 0;
 } 

 void CMDIFrameWndXP::OnUpdateMenus(CCmdUI *pCmdUI) {

	 pCmdUI->Enable(m_pAceleradores.Find(pCmdUI->m_nID)!=NULL);
 }

 void CMDIFrameWndXP::OnCommandMenu(UINT id) {

	 CMiscelaneosEx *info=m_pAceleradores.Find(id);
	 if( info && ::IsWindow(m_pNavBar))
		 PostMessage(WM_TREENAVEGADOR, NM_DBLCLK, m_pNavBar.m_Tree.GetItemData(m_pNavBar.FindItemEx(info->GetNombre())));
 }

 void CMDIFrameWndXP::SendMessageBroadCast(UINT message,WPARAM wParam, LPARAM lParam) {

	 CMFCTabCtrl* pPrevActiveWnd = m_wndClientArea.FindActiveTabWnd() ? m_wndClientArea.FindActiveTabWnd() : &m_wndClientArea.GetMDITabs();
	 for( int i=0 ;i< pPrevActiveWnd->GetTabsNum(); i++) {
		 CWnd *ventana= pPrevActiveWnd->GetTabWnd(i);
		 ventana->GetNextWindow(GW_CHILD)->SendMessage(message, wParam, lParam);
	 }
 }

 void CMDIFrameWndXP::OnClose()
 {
	 SaveState();
	 CMDIFrameWndEx::OnClose();
	 AfxGetApp()->WriteProfileInt("control","close", 1);
 }

 void CMDIFrameWndXP::OnVerNavegador()
 {
	if(::IsWindow(m_pNavBar)) 
		m_pNavBar.ShowPane(!m_pNavBar.IsWindowVisible(), false, true);
 }

 void CMDIFrameWndXP::OnUpdateVerNavegador(CCmdUI *pCmdUI)
 {
	 pCmdUI->SetCheck( ::IsWindow(m_pNavBar) && m_pNavBar.IsWindowVisible());
 }

 LRESULT CMDIFrameWndXP::OnQueryTime(WPARAM wParam, LPARAM lParam) {

	 if( lParam) 
		 ((CWnd *) lParam)->SendMessage(WM_QUERYTIME, 0, (LPARAM)&m_pTServidor);
	 return (LRESULT) &m_pTServidor;
 }

 void CMDIFrameWndXP::OnNextPane()
 {
	 CMFCTabCtrl* pPrevActiveWnd = m_wndClientArea.FindActiveTabWnd() ? m_wndClientArea.FindActiveTabWnd() : &m_wndClientArea.GetMDITabs();
	 pPrevActiveWnd->SetActiveTab(pPrevActiveWnd->GetActiveTab()+1);
 }

 void CMDIFrameWndXP::OnUpdateNextPane(CCmdUI *pCmdUI)
 {
	 CMFCTabCtrl* pPrevActiveWnd = m_wndClientArea.FindActiveTabWnd() ? m_wndClientArea.FindActiveTabWnd() : &m_wndClientArea.GetMDITabs();
	 pCmdUI->Enable((pPrevActiveWnd->GetActiveTab()<(pPrevActiveWnd->GetTabsNum()-1)));
 }

 void CMDIFrameWndXP::OnPrevPane()
 {
	 CMFCTabCtrl* pPrevActiveWnd = m_wndClientArea.FindActiveTabWnd() ? m_wndClientArea.FindActiveTabWnd() : &m_wndClientArea.GetMDITabs();
	 pPrevActiveWnd->SetActiveTab(pPrevActiveWnd->GetActiveTab()-1);
 }

 void CMDIFrameWndXP::OnUpdatePrevPane(CCmdUI *pCmdUI)
 {
	 CMFCTabCtrl* pPrevActiveWnd = m_wndClientArea.FindActiveTabWnd() ? m_wndClientArea.FindActiveTabWnd() : &m_wndClientArea.GetMDITabs();
	 pCmdUI->Enable(pPrevActiveWnd->GetActiveTab()>0);
 }

 void CMDIFrameWndXP::OnCloseAll() {

	 CMFCTabCtrl* pPrevActiveWnd = m_wndClientArea.FindActiveTabWnd() ? m_wndClientArea.FindActiveTabWnd() : &m_wndClientArea.GetMDITabs();
	 int pos= pPrevActiveWnd->GetActiveTab();
	 if( pos>=0) {
		 CWaitCursor wait;
		 CWnd *actual= pPrevActiveWnd->GetTabWnd(pos);
		 CWnd *del;
		 ASSERT(actual);
		 for(int i=0; i<pPrevActiveWnd->GetTabsNum(); i++) {

			 del= pPrevActiveWnd->GetTabWnd(i);
			 if( del!=actual) {
				 del->SendMessage(WM_CLOSE);
				 i--;
			 }
		 }
	 }
 }

 int CMDIFrameWndXP::LoadDocumentosEx(CString nombreDLL, HTREEITEM hRoot, const CMiscelaneosEx &permisos, DWORD grupo) {

	 // cargamos mas documentos desde dll
	 HINSTANCE hDLL;
	 int i=0;
	 LOADFUNCIONES lpfnInicial;
	 CMiscelaneosEx *acel;
	 CMenu *menu;
	 CString temp;

	 // cargamos todos los nombres de las funciones
	 hDLL = LoadLibrary( nombreDLL);
	 lpfnInicial= (LOADFUNCIONES)::GetProcAddress( hDLL, "LoadFunciones3");
	 if( !lpfnInicial) {

		 FreeLibrary(hDLL);
		 return false;
	 }
	 else {

		 LPDLLFUNC3 lpfnDllFunc;
		 CStringArray funciones;
		 CMultiDocTemplateXP *pDocTemplate;

		 lpfnInicial(&funciones);
		 for( i=0; i<funciones.GetCount(); i++) {

			 lpfnDllFunc = (LPDLLFUNC3)::GetProcAddress(hDLL,funciones.GetAt(i));
			 if (lpfnDllFunc) {
				 CString strLabel;
				 CString strGrupo;
				 pDocTemplate = lpfnDllFunc(&strLabel, permisos, grupo, &strGrupo);
				 if( pDocTemplate) {

					 if( strLabel.IsEmpty())
						 continue;
					 HTREEITEM item= ::IsWindow(m_pNavBar) ? m_pNavBar.FindItem(strGrupo) : NULL;
					 if( item==NULL && ::IsWindow(m_pNavBar)) {
						 if(strGrupo.IsEmpty()) {
							 item= TVI_ROOT;
						 } else {
							 item= m_pNavBar.m_Tree.InsertItem(strGrupo);
							 m_pNavBar.m_Tree.SetItemState(item, TVIS_BOLD,TVIS_BOLD);
						 }
					 }
					 if( item && ::IsWindow(m_pNavBar))
						 item= m_pNavBar.m_Tree.InsertItem( strLabel,item);
					 if( item && ::IsWindow(m_pNavBar))
						 m_pNavBar.m_Tree.SetItemData( item, (DWORD) pDocTemplate);

					 temp= ParseAcelerator(strLabel, m_pAceleradores.GetCount());
					 acel= m_pAceleradores.Add(pDocTemplate->GetViewClassName(), m_pAceleradores.GetCount()+ACELERADORES_PREFIJO);
				}
			 }
		 }
	 }
	 if(::IsWindow(m_pNavBar))
		 m_pNavBar.m_Tree.Expand( hRoot, TVE_EXPAND);
	 return i;	 
 }

 int CMDIFrameWndXP::LoadDocumentosEx(CString nombreDLL, HTREEITEM hRoot, DWORD parametro) {

	 // cargamos mas documentos desde dll
	 HINSTANCE hDLL;
	 int i=0;
	 LOADFUNCIONES lpfnInicial;
	 CMiscelaneosEx *acel;
	 CMenu *menu;
	 CString temp;

	 // cargamos todos los nombres de las funciones
	 hDLL = LoadLibrary( nombreDLL);
	 lpfnInicial= (LOADFUNCIONES)::GetProcAddress( hDLL, "LoadFunciones");
	 if( !lpfnInicial) {

		 FreeLibrary(hDLL);
		 return false;
	 }
	 else {

		 LPDLLFUNC lpfnDllFunc;
		 CStringArray funciones;
		 CMultiDocTemplateXP *pDocTemplate;

		 lpfnInicial(&funciones);
		 for( i=0; i<funciones.GetCount(); i++) {

			 lpfnDllFunc = (LPDLLFUNC)::GetProcAddress(hDLL,funciones.GetAt(i));
			 if (lpfnDllFunc) {
				 CString strLabel;
				 CString grupo;
				 pDocTemplate = lpfnDllFunc(&strLabel, parametro, &grupo);
				 if( pDocTemplate) {

					 if( strLabel.IsEmpty())
						 continue;
					 HTREEITEM item= ::IsWindow(m_pNavBar) ? m_pNavBar.FindItem( grupo) : NULL;
					 if( item==NULL && ::IsWindow(m_pNavBar)) {
						 if(grupo.IsEmpty()) {
							 item= TVI_ROOT;
						 } else {
							 item= m_pNavBar.m_Tree.InsertItem(grupo);
							 m_pNavBar.m_Tree.SetItemState(item, TVIS_BOLD,TVIS_BOLD);
						 }
					 }
					 if( item && ::IsWindow(m_pNavBar))
						 item= m_pNavBar.m_Tree.InsertItem( strLabel,item);
					 if( item && ::IsWindow(m_pNavBar))
						 m_pNavBar.m_Tree.SetItemData( item, (DWORD) pDocTemplate);

					 temp= ParseAcelerator(strLabel, m_pAceleradores.GetCount());

					 acel= m_pAceleradores.Add(pDocTemplate->GetViewClassName(), m_pAceleradores.GetCount()+ACELERADORES_PREFIJO);
				}
			}
		 }
	 }
	 if(::IsWindow(m_pNavBar))
		m_pNavBar.m_Tree.Expand( hRoot, TVE_EXPAND);
	 return i;
 }

 void CMDIFrameWndXP::OnHelpIndex() {

	 CString temp;
	 temp.Format( "\"%s\"",CApplicacion::GetAppPath("jayuda.exe"));
	 CWnd *child= m_wndClientArea.GetTopWindow();
	 if( child && child->IsKindOf(RUNTIME_CLASS(CMDIChildWnd))) {
		 
		 CView *view= ((CMDIChildWnd *) child)->GetActiveView();
		 if(view) {
			 CDocument *documento= view->GetDocument();
			 if(documento)
				 temp+=" /t \"" + documento->GetTitle() + "\"";
		 }
	 }
	 WinExec( temp, SW_NORMAL);
 }

CTreeNavBar	*CMDIFrameWndXP::GetNavigateBar() {

	return &m_pNavBar;
}

void CMDIFrameWndXP::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	((CWinAppXP *)AfxGetApp())->m_nAppLook = id;

	switch (((CWinAppXP *)AfxGetApp())->m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (((CWinAppXP *)AfxGetApp())->m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);
	((CWinAppXP *)AfxGetApp())->WriteInt(_T("ApplicationLook"), ((CWinAppXP *)AfxGetApp())->m_nAppLook);
}

void CMDIFrameWndXP::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(((CWinAppXP *)AfxGetApp())->m_nAppLook == pCmdUI->m_nID);
}

void CMDIFrameWndXP::OnWindowManager()
{
	ShowWindowsDialog();
}

int CMDIFrameWndXP::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	OnApplicationLook( ((CWinAppXP *)AfxGetApp())->m_nAppLook);
	
	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // other styles available...
	mdiTabParams.m_bActiveTabCloseButton = TRUE;      // set to FALSE to place close button at right of tab area
	mdiTabParams.m_bTabIcons = FALSE;    // set to TRUE to enable document icons on MDI taba
	mdiTabParams.m_bAutoColor = FALSE;    // set to FALSE to disable auto-coloring of MDI tabs
	mdiTabParams.m_bDocumentMenu = TRUE; // enable the document menu at the right edge of the tab area
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	m_wndRibbonBar.Create(this);
	
	InitializeRibbon();

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("No se pudo crear la barra de estado\n");
		return -1;      // No se pudo crear
	}

	CResourceDLL *dll=new CResourceDLL();
	CMFCRibbonStatusBarPaneEx *texto= new CMFCRibbonStatusBarPaneEx(ID_STATUSBAR_TEXTO, "Listo", TRUE, &m_wndStatusBar);
	
	m_wndStatusBar.AddElement(texto, "Estado");
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonProgressBar(ID_STATUSBAR_BARRADEPROGRESO), "Progreso");
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_TOTAL, "Sin resultados", TRUE) , "Total");
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_ANIMACIONPROGRESO, "", IDB_BLOADING, 16, RGB(255,0,255)), "Progreso");
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_USUARIO, "Usuario", TRUE), "Usuario");
	// m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_SERVIDOR, "Servidor", TRUE), "Servidor");
	delete dll;
	
	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	if (!m_pNavBar.Create("Navegador", this, CRect(0, 0, 200, 200), TRUE, 512, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create navigate window\n");
		return FALSE; // failed to create
	}

	m_pNavBar.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_pNavBar);

	// Navigation pane will be created at left, so temporary disable docking at the left side:
	EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM | CBRS_ALIGN_RIGHT);

	// Outlook bar is created and docking on the left side should be allowed.
	EnableDocking(CBRS_ALIGN_LEFT);
	EnableAutoHidePanes(CBRS_ALIGN_RIGHT);

	// Enable enhanced windows management dialog
	EnableWindowsDialog(ID_WINDOW_MANAGER, "Ventanas", TRUE);
	return 0;
}

void CMDIFrameWndXP::InitializeRibbon() {

	CResourceDLL dll;

	// Add elements to the right side of tabs:
	CMFCRibbonButton* pVisualStyleButton = new CMFCRibbonButton(-1, "Estilo", -1, -1);

	pVisualStyleButton->SetMenu(IDR_THEME_MENU, FALSE /* No default command */, TRUE /* Right align */);
	pVisualStyleButton->SetToolTipText("Modificar estilo visual");
	pVisualStyleButton->SetDescription("Cambiar entre temas");
	m_wndRibbonBar.AddToTabs(pVisualStyleButton);
}

BOOL CMDIFrameWndXP::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) {

	if(!AfxGetApp()->GetProfileInt("control","close", 1)) {

		CMyRegKey keytemp(((CWinAppXP *)AfxGetApp())->GetRegSectionPath());
		((CWinAppXP *)AfxGetApp())->SetRegistryBase("Workspace1");
		CMyRegKey keytemp1(((CWinAppXP *)AfxGetApp())->GetRegSectionPath());
		keytemp.DeleteRegKey("", "");
		keytemp1.DeleteRegKey("", "");
		AfxGetApp()->WriteProfileInt("barras", "store",0);
		AfxGetApp()->WriteProfileString("control", "classname","");
	}
	((CWinAppXP *)AfxGetApp())->SetRegistryBase("Workspace1");
	CMyRegKey key(((CWinAppXP *)AfxGetApp())->GetRegSectionPath());
	CStringArray valores;
	key.EnumRegKey("", valores);
	for(int i=0; i<valores.GetCount(); i++) {

		if(valores[i].Find("Pane")!=-1 || valores[i].Find("DockingManager")!=-1) 
			key.DeleteRegKey(valores[i], "");
	}
	BOOL retorno= CMDIFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext);
	if(m_pNavBar.IsAutoHideMode())	
		m_pNavBar.UnSetAutoHideMode(NULL);
	m_pNavBar.ShowPane(false,false,false);
	((CWinAppXP *)AfxGetApp())->SetRegistryBase("Workspace");
	return retorno;
}

BOOL CMDIFrameWndXP::OnShowMDITabContextMenu(CPoint point, DWORD dwAllowedItems, BOOL bDrop) {

	CMenu menu;
	CMenu submenu,submenu1;
	menu.CreatePopupMenu();
	submenu.CreatePopupMenu();

	CMFCTabCtrl* pPrevActiveWnd = m_wndClientArea.FindActiveTabWnd() ? m_wndClientArea.FindActiveTabWnd() : &m_wndClientArea.GetMDITabs();
	int i=pPrevActiveWnd->GetActiveTab();
	int pos=0;

	menu.InsertMenu(pos++, MF_BYPOSITION , ID_FILE_CLOSE, "Cerrar\tCtrl+F4");
	if( pPrevActiveWnd->GetTabsNum()>1) {
		menu.InsertMenu(pos++, MF_BYPOSITION | MF_STRING, ID_WINDOW_CLOSE_ALL, pPrevActiveWnd->GetTabsNum()==2 ? "C&errar Otra" : "C&errar Otras");
	}
	menu.InsertMenu(pos++, MF_BYPOSITION|MF_SEPARATOR);
	if( pPrevActiveWnd->GetTabsNum()>1) {

		menu.InsertMenu(pos++, MF_BYPOSITION|MF_SEPARATOR);
		if( i!=pPrevActiveWnd->GetTabsNum()-1) {
			menu.InsertMenu(pos, MF_BYPOSITION, ID_NEXT_PANEEX, "Siguiente\tAlt+Flecha Derecha");
			pos++;
		}
		if( i!=0)
			menu.InsertMenu(pos, MF_BYPOSITION, ID_PREV_PANEEX, "Anterior\tAlt+Flecha Izquierda");

		for( int j=0; j<pPrevActiveWnd->GetTabsNum(); j++) {
			if( i==j)
				continue;
			CWnd *child= pPrevActiveWnd->GetTabWnd(j);
			CString temp= ((CMDIChildWndXP *)child)->GetActiveView()->GetDocument()->GetTitle();
			submenu.AppendMenu(MF_STRING,j+VENTANAS_PREFIJO,temp);
		}
		menu.AppendMenu(MF_SEPARATOR);
		menu.AppendMenu(MF_POPUP, (UINT) submenu.GetSafeHmenu(), "&Ventanas");
	}

	CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu();
	pPopupMenu->Create(AfxGetMainWnd(), point.x, point.y, menu, true, true);
	// menu.TrackPopupMenu( TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
	return TRUE;
}

BOOL CMDIFrameWndXP::OnEraseMDIClientBackground(CDC* pDC) {

	static bool invalidate=true;
	CRect rect;
	m_wndClientArea.GetClientRect(rect);

	if(m_sizeClient.cx!= rect.Width() || m_sizeClient.cy!=rect.Height()) {

		m_sizeClient.cx= rect.Width();
		m_sizeClient.cy= rect.Height();
		BuildFlash();
	}

	CString sLogoString=CApplicacion::GetCompany() + "\r" + CApplicacion::GetLongName();
	int nLineas= 1;
	for( int i=0; i<sLogoString.GetLength(); i++) {
		if(sLogoString[i]=='\r')
			nLineas++;
	}
	CRect rDataBox= rect;

	if(m_hBitmap) {
		CDC dcMem;
		VERIFY( dcMem.CreateCompatibleDC(pDC));

		HBITMAP* pBmpOld = (HBITMAP*) ::SelectObject(dcMem.m_hDC, m_hBitmap);
		pDC->SetStretchBltMode(COLORONCOLOR);

		pDC->StretchBlt(rect.left, rect.top, rect.Width(), rect.Height(), 
			&dcMem, 0, 0, m_bmInfo.bmWidth-1, m_bmInfo.bmHeight-1,
			SRCCOPY);
		::SelectObject(dcMem.m_hDC, pBmpOld);
	} else {
		CBrush NewBrush(GetSysColor(COLOR_DESKTOP));
		pDC->SetBrushOrg(0,0) ;
		CBrush* pOldBrush = (CBrush*)pDC->SelectObject(&NewBrush);   
		pDC->PatBlt(rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top ,PATCOPY);
		pDC->SelectObject(pOldBrush);
	}

	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	CFont* oldFont = pDC->SelectObject(&m_LogoFont);
	CSize sz = pDC->GetTextExtent(sLogoString, sLogoString.GetLength());
	// GetTextExtent calculates the size of the displayed logo
	// which depends on the device context....
	// Calculate the box size by subtracting the text width and height from the
	// window size.  Also subtract 20% of the average character size to keep the
	// logo from printing into the borders...
	rDataBox.left = rDataBox.right  - sz.cx - tm.tmAveCharWidth/2;
	rDataBox.left-=10;
	rDataBox.right-=10;
	rDataBox.top  = rDataBox.bottom - nLineas*(sz.cy - tm.tmHeight/5);
	rDataBox.top-=20;
	rDataBox.bottom-=10;
	pDC->SetBkMode(TRANSPARENT);
	// shift logo box right, and print black...
	COLORREF oldColor = pDC->SetTextColor(RGB(0,0,0));
	pDC->DrawText(sLogoString, sLogoString.GetLength(), &rDataBox, DT_VCENTER | /*DT_SINGLELINE | */DT_RIGHT);
	// shift logo box left and print white
	rDataBox.left -= (nLineas+1)*tm.tmAveCharWidth/5;
	pDC->SetTextColor(RGB(255,255,255));
	pDC->DrawText(sLogoString, sLogoString.GetLength(), &rDataBox, DT_VCENTER | /*DT_SINGLELINE | */DT_RIGHT);
	// Restore original location and print in the button face color
	rDataBox.left += tm.tmAveCharWidth/5;
	pDC->SetTextColor(GetSysColor(COLOR_BTNFACE));
	pDC->DrawText(sLogoString, sLogoString.GetLength(), &rDataBox, DT_VCENTER | /*DT_SINGLELINE | */DT_RIGHT);
	// restore the original properties and release resources...
	pDC->SelectObject(oldFont);
	pDC->SetTextColor(oldColor);

	if(invalidate) {
		invalidate=false;
		Invalidate();
	} else invalidate=true;
	return TRUE;
}


void CMDIFrameWndXP::BuildFlash(bool load/*=false*/) {

	CWaitCursor wait;
	if(m_szArchivo.IsEmpty())
		return;
	int mancho=m_sizeClient.cx/2;
	int malto= m_sizeClient.cy/2;

	CRect rect(mancho-m_nAncho/2,
		malto-m_nAlto/2,
		m_nAncho, m_nAlto);
	rect.right= rect.left+m_nAncho;
	rect.bottom= rect.top+m_nAlto;

	m_wndClientArea.ClientToScreen(rect);
	ScreenToClient(rect);

	if(!m_pFlash) {
		m_pFlash=new CShockwaveflash1();
		if(!m_pFlash->Create("", "", WS_CHILD | WS_VISIBLE, rect, this, 1050)) {
			delete m_pFlash;
			m_pFlash=NULL;
			return;
		}
	} else {
		m_pFlash->MoveWindow(rect);
	}
	if(m_dwBkColor!=-1) {
		m_pFlash->put_BackgroundColor(RGB(
			GetBValue(m_dwBkColor),
			GetGValue(m_dwBkColor),
			GetRValue(m_dwBkColor))
			);
	}
	if(load)
		m_pFlash->put_Movie(m_szArchivo);
}

void CMDIFrameWndXP::SetFlash(CString archivo, int ancho/*=0*/, int alto/*=0*/, DWORD bkColor) {

	m_szArchivo= archivo;
	m_nAlto= alto;
	m_nAncho= ancho;
	m_dwBkColor= bkColor;
	BuildFlash(true);
}
