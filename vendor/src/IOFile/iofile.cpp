// IOFile.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "IOFile.h"
#include "DlgElementos.h"
# include "export.h"
# include "LicenceDlg.h"
# include "DlgProgreso.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(CIOFileApp, CWinApp)
END_MESSAGE_MAP()


// CIOFileApp construction

CIOFileApp::CIOFileApp()
{
}


// The one and only CIOFileApp object

CIOFileApp theApp;


// CIOFileApp initialization

BOOL CIOFileApp::InitInstance()
{
	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}
#if defined(ENABLE_LICENSE_CHECK) && !defined(_DEBUG)
	if(!RegisterFile(CApplicacion::GetAppPath("iofile.dll"))) {

		CString texto;
		texto.Format("%s Esta corrupto.\nPor favor ReInstale %s o Informe al Administrador del sistema.", CApplicacion::GetAppName(), CApplicacion::GetAppName());
		AfxMessageBox(texto, MB_ICONERROR);
		return false;
	}
#endif
	return TRUE;
}

DWORD GetLicenceDlg() {
#ifdef ENABLE_LICENSE_CHECK

	CXWinVersion version;
	if(version.IsVistaorLater()) {

		BOOL bElevated = FALSE;
		if ( FAILED( CVistaTools::IsElevated( &bElevated ) ) )
		{
			ASSERT( FALSE );
			return IDCANCEL;
		}
		if(!bElevated) {

			TCHAR szPath[ MAX_PATH ];
			::GetModuleFileName( NULL, szPath, MAX_PATH);
			CVistaTools::RunElevated(NULL, szPath);
			return IDCANCEL;
		}
	}

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CLicenceDlg dlg;
	return dlg.DoModal();
#else
	return IDOK;
#endif
}

DWORD GetProgressDlg(CStringArray *querys, CStringArray *mensajes, const CString titulo,bool silent/*=false*/) {

	CWnd *main= AfxGetMainWnd();
	CConector *conector=NULL;
	if(silent) {
		conector= (CConector *)main->SendMessage(WM_QUERYCONECTOR);
		ASSERT(conector);
		conector->SetMainFrame(NULL);
	}
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CDlgProgreso dlg;
	dlg.m_pConector= (CConector *)main->SendMessage(WM_QUERYCONECTOR);
	dlg.m_pQuerys= querys;
	dlg.m_pMensajes= mensajes;
	dlg.m_szTitulo= titulo;
	dlg.m_bSilent= silent;
	DWORD resultado= dlg.DoModal();
	if(conector && silent) 
		conector->SetMainFrame(main);
	return resultado;
}

int DlgListado(CString titulo,CStringArray *elementos, CString *elemento, CStringArray *descripciones) {

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CDlgElementos dlg;
	dlg.m_szFinal= *elemento;
	dlg.m_szValores= elementos;
	dlg.m_szDescripciones= descripciones;
	dlg.m_szTitulo= titulo;
	int	retorno= dlg.DoModal();
	if( retorno ==IDOK) 
		(*elemento)= dlg.m_szFinal;
	return retorno;
}

CString	MarcasToNumeros( CStringArray &nombres, CUIntArray  &id, CString valor) {

	CStringArray nuevos;
	CString temp1;
	int pos;
	while( (pos= valor.Find(","))!=-1) {

		nuevos.Add( valor.Left(pos));
		valor.Delete(0, pos+1);
	}
	if( !valor.IsEmpty())
		nuevos.Add( valor);
	valor.Empty();
	for( int i=0; i< nuevos.GetCount(); i++) {
							
		if(i) 
			valor+=",";
		temp1= nuevos.GetAt(i);
		for( int j=0; j< nombres.GetCount(); j++) {
								
			if( nombres.GetAt(j).CompareNoCase( temp1)==0) {

				temp1.Format("%d", id.GetAt(j));
				break;
			}
		}
		valor+= temp1;
	}
	return valor;
}

CString	MarcasToNumerosEx( CStringArray &nombres, CUIntArray  &id, CString valor, CString &salida) {

	CStringArray nuevos;
	CString temp1;
	int pos;
	int j=0;
	while( (pos= valor.Find(","))!=-1) {

		nuevos.Add( valor.Left(pos));
		valor.Delete(0, pos+1);
	}
	if( !valor.IsEmpty())
		nuevos.Add( valor);
	valor.Empty();
	for( int i=0; i< nuevos.GetCount(); i++) {

		if(i) 
			valor+=",";
		temp1= nuevos.GetAt(i);
		for( j=0; j< nombres.GetCount(); j++) {
								
			if( nombres.GetAt(j).CompareNoCase( temp1)==0) {

				temp1.Format("%d", id.GetAt(j));
				break;
			}
		}
		if( j==nombres.GetCount()) // no lo encontro
			return temp1;
		valor+= temp1;
	}
	salida= valor;
	return NULL;
}

CString	NumerosToMarcas(CStringArray &nombres, CUIntArray &id, CString valor) {

	CStringArray nuevos;
	CString temp1;
	int pos;
	while( (pos= valor.Find(","))!=-1) {

		nuevos.Add( valor.Left(pos));
		valor.Delete(0, pos+1);
	}
	if( !valor.IsEmpty())
		nuevos.Add( valor);
	valor.Empty();
	for( int i=0; i< nuevos.GetCount(); i++) {
							
		if(i) 
			valor+=",";
		temp1= nuevos.GetAt(i);
		for( int j=0; j< id.GetCount(); j++) {
									
			if( atol(temp1)== id.GetAt(j)) {

				temp1= nombres.GetAt(j);
				break;
			}
		}
		valor+= temp1;
	}
	return valor;
}

bool Exportar(CMyListCtrl *pLista,CString Titulo, bool showfile) {

	CExport pExport(NULL);
	CFileDialog dlg( false, pExport.GetDefaultType(true), Titulo, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER, pExport.GetFileTypes(true));
	if( dlg.DoModal()==IDOK) {

			CWaitCursor wait;

			pExport.SetRows(pLista->GetItemCount());

			CStringArray result;
			if(!pExport.CustomExportLocal( dlg.GetPathName(), Titulo)) {
				AfxMessageBox("No se puede obtener acceso a \"" + dlg.GetPathName() + "\".\nCompruebe que el archivo no este siendo utilizado por otro proceso.", MB_OK | MB_ICONERROR);
				return false;
			}
			CMFCHeaderCtrl *header= &pLista->GetHeaderCtrl();
			ASSERT(header);
			int len= header->GetItemCount();

			HDITEM hdi;
			TCHAR  lpBuffer[256];
			hdi.mask = HDI_TEXT;

			for( int i=0; i<len; i++) {

				hdi.pszText = lpBuffer;
				hdi.cchTextMax = 256;
				header->GetItem(i, &hdi);
				result.Add(hdi.pszText);
				pExport.SetTipoColumna(i, pLista->GetColumnType(i));
			}
			pExport.Inicio(result);
			ULONG filas= pLista->GetItemCount();
			AfxGetMainWnd()->PostMessage(WM_STATUSBAR, 0, (LPARAM)filas);
			for( int i=0; i<filas; i++) {
				
				result.RemoveAll();
				for( int j=0; j<len; j++) 

					result.Add(pLista->GetItemText(i,j));
				pExport.Agregar( result);
				AfxGetMainWnd()->PostMessage(WM_STATUSBAR, 1);
			}
			pExport.Terminar();
			AfxGetMainWnd()->PostMessage(WM_STATUSBAR, 2);
			if(showfile) {
				if( MyMessageBox("¿Desea ver el archivo?", MB_ICONQUESTION | MB_YESNOCANCEL, "showfile")==IDYES) 
					ShellExecute(NULL, "open", dlg.GetPathName(), NULL,NULL,SW_SHOWNORMAL);
			}
			return true;
	}
	return false;
}


bool Exportar(CTreeListCtrl *pTree,CString Titulo, bool showfile) {

	CExport pExport(NULL);
	CFileDialog dlg( false, pExport.GetDefaultType(true), Titulo, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER, pExport.GetFileTypes(true));
	if( dlg.DoModal()==IDOK) {

			CWaitCursor wait;

			pExport.SetRows(pTree->GetItemCount());

			CStringArray result;
			if(!pExport.CustomExportLocal( dlg.GetPathName(), Titulo)) {
				AfxMessageBox("No se puede obtener acceso a \"" + dlg.GetPathName() + "\".\nCompruebe que el archivo no este siendo utilizado por otro proceso.", MB_OK | MB_ICONERROR);
				return false;
			}

			int len= pTree->GetColumnCount();

			for( int i=0; i<len; i++) {
				result.Add(pTree->GetColumnText(i));
				pExport.SetTipoColumna(i, pTree->GetColumnType(i));
			}

			pExport.Inicio(result);
			ULONG filas= pTree->GetItemCount();
			AfxGetMainWnd()->PostMessage(WM_STATUSBAR, 0, (LPARAM)filas);
			
			HTREELISTITEM item= NULL;
			HTREELISTITEM subitem= NULL;

			item= pTree->GetRootItem();
			while( item) {

				result.RemoveAll();
				for( int j=0; j<len; j++) 
					result.Add(pTree->GetItemText(item,j));
				pExport.Agregar( result);
				AfxGetMainWnd()->PostMessage(WM_STATUSBAR, 1);

				subitem= pTree->GetChildItem(item);
				while( subitem) {

					result.RemoveAll();
					for( int j=0; j<len; j++) 
						result.Add(pTree->GetItemText(subitem,j));
					pExport.Agregar( result);
					AfxGetMainWnd()->PostMessage(WM_STATUSBAR, 1);
					subitem= pTree->GetNextSiblingItem(subitem);
				}
                item= pTree->GetNextSiblingItem(item);
			}
			pExport.Terminar();
			AfxGetMainWnd()->PostMessage(WM_STATUSBAR, 2);
			if(showfile) {
				if( MyMessageBox("¿Desea ver el archivo?", MB_ICONQUESTION | MB_YESNOCANCEL, "showfile")==IDYES) 
					ShellExecute(NULL, "open", dlg.GetPathName(), NULL,NULL,SW_SHOWNORMAL);
			}
			return true;
	}
	return false;
}

CPtrArray *SortComponentes(CFormView *view, int &horizontales) {

	CPtrArray *pHorizontales= new CPtrArray();
	CWnd *item= view->GetWindow(GW_CHILD);
	CRect rect;
	horizontales=0;

	// organizamos los elementos
	while(item) {

		item->GetWindowRect(rect);
		TCHAR clase[MAX_PATH];
		if(GetClassName(*item, clase, MAX_PATH)) {
			CString szClase= clase;
			if(szClase=="Button") { // es un group box o boton
				if((item->GetStyle()&BS_AUTOCHECKBOX)==BS_AUTOCHECKBOX) { // es un checkbox o groupbox
					if(item->GetStyle()&BS_RADIOBUTTON) { // es un group box
						item= item->GetWindow(GW_HWNDNEXT);
						continue;
					}
				} else {
					item= item->GetWindow(GW_HWNDNEXT);
					continue;
				}
			}
		}
		if(item->IsKindOf(RUNTIME_CLASS(CStaticSeparator))) {
			item= item->GetWindow(GW_HWNDNEXT);
			continue;
		}
		CPtrArray *ptr=NULL;
		for(int i=0; i<pHorizontales->GetCount(); i++) {
			CPtrArray *temp=(CPtrArray *)pHorizontales->GetAt(i);
			if(temp) {
				for(int j=0; j<temp->GetCount(); j++) {
					CRect rect1;
					CWnd *itemrect= (CWnd *)temp->GetAt(j);
					itemrect->GetWindowRect(rect1);
					if(rect1.top<rect.CenterPoint().y && rect1.bottom>rect.CenterPoint().y) { // esta en el punto

						ptr= temp;
						break;
					}
				}
			}
			if(ptr)
				break;
		}
		if(!ptr) {
			ptr= new CPtrArray();
			pHorizontales->Add(ptr);
		}
		ASSERT(ptr);
		int col=-1;
		for(int i=0; i<ptr->GetCount(); i++) {
			CRect rect1;
			CWnd *itemrect= (CWnd *)ptr->GetAt(i);
			itemrect->GetWindowRect(rect1);
			if(rect.CenterPoint().x<rect1.CenterPoint().x) { // es menor
				col= i;
				ptr->InsertAt(i, item);
				break;
			}
		}
		if(col==-1)
			ptr->Add(item);
		if(horizontales<ptr->GetCount()) 
			horizontales= ptr->GetCount();
		item= item->GetWindow(GW_HWNDNEXT);
	}
	return pHorizontales;
}

CString GetCommonPath(const CString &ruta) {
#ifndef _DEBUG
	TCHAR szPath[MAX_PATH];
	if(SUCCEEDED(SHGetFolderPath(NULL, 
								 CSIDL_COMMON_APPDATA, 
								 NULL, 
								 0, 
								 szPath))) 
	{
		PathAppend(szPath, "SDG");
		PathAppend(szPath, CApplicacion::GetAppName());
		PathAppend(szPath, ruta);
		return szPath;
	}
#endif
	return CApplicacion::GetAppPath(ruta);
}
