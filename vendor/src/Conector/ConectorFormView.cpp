#include "StdAfx.h"
#include "ConectorFormView.h"
# include "Query.h"
# include "PropiedadesBar.h"

IMPLEMENT_DYNAMIC(CConectorFormView, CResizableFormView)

CConectorFormView::CConectorFormView(LPCTSTR lpszTemplateName) : CResizableFormView(lpszTemplateName)
{
	m_pConector=NULL;
	m_pUsuario=NULL;
	m_pUsuario2= NULL;
	m_pUsuario3= NULL;
}

CConectorFormView::CConectorFormView(UINT nIDTemplate) : CResizableFormView(nIDTemplate)
{
}

CConectorFormView::~CConectorFormView(void)
{
}

void CConectorFormView::OnInitialUpdate()
{
	CResizableFormView::OnInitialUpdate();
	m_pConector=(CConector *)AfxGetMainWnd()->SendMessage(WM_QUERYCONECTOR);
	m_pUsuario=(CUsuario *)AfxGetMainWnd()->SendMessage(WM_QUERYUSUARIO);
	m_pUsuario2=(CUsuario2 *) m_pUsuario;
	m_pUsuario3=(CUsuario3 *) m_pUsuario;
}

BOOL CConectorFormView::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN || pMsg->message==WM_KEYUP) {
		if( pMsg->wParam== VK_RETURN  || pMsg->wParam==VK_DOWN) {

			CWnd *foco= GetFocus();
			if( foco) {
				if( !foco->IsKindOf( RUNTIME_CLASS(CButton)) || pMsg->wParam==VK_DOWN) {
					if(foco->IsKindOf(RUNTIME_CLASS(CEdit))) {

						if((((CEdit*)foco)->GetStyle() & ES_MULTILINE || foco->GetParent()->IsKindOf(RUNTIME_CLASS(CComboBox))) && pMsg->wParam==VK_DOWN) { // no se hace nada para los que son multilineas
						} else {
							pMsg->wParam= VK_TAB;
						}
					} else if(pMsg->wParam!=VK_DOWN) {
						pMsg->wParam= VK_TAB;
					}
				} else pMsg->wParam=VK_SPACE;
			}
		} else if(pMsg->wParam==VK_ESCAPE || pMsg->wParam==VK_UP) {
			if(pMsg->message==WM_KEYDOWN) {
				CWnd *foco= GetFocus();
				if( foco) {
					if( foco->IsKindOf( RUNTIME_CLASS(CEdit))) {
						if((((CEdit*)foco)->GetStyle() & ES_MULTILINE || foco->GetParent()->IsKindOf(RUNTIME_CLASS(CComboBox))) && pMsg->wParam==VK_UP) {
							// no se hace nada si es una linea multiple y esta subiendo.
						} else {
							CWnd *elemento = GetNextDlgTabItem(foco->GetParent()->IsKindOf(RUNTIME_CLASS(CComboBox)) ? foco->GetParent() : foco, true);
							elemento->SetFocus();
							if( elemento->IsKindOf(RUNTIME_CLASS(CEdit)))
								((CEdit *)elemento)->SetSel(0, ((CEdit *)elemento)->GetWindowTextLength());
							return TRUE;
						}
					}
				}
			}
		}
		if(pMsg->wParam==VK_ESCAPE)
			return TRUE; // no dejamos pasar el escape
	}
	return CResizableFormView::PreTranslateMessage(pMsg);
}

CString CConectorFormView::InsertModification(const CString &id) {

	CQuery query("modificaciones");

	query["id"]= id;
	query["fecha"]="SYSDATE()";
	CString temp;
	temp.Format("%d", m_pUsuario->GetID());
	query["usuario"]= temp;
	query["clase"]= GetRuntimeClass()->m_lpszClassName;
	
	return query.GetInsert();
}

void CConectorFormView::SetError(const CString &error, CWnd *elemento/*=NULL*/,int tipo/*=0*/) {

	if(tipo)
		::SetErrorEx(elemento, error, tipo, GetRuntimeClass()->m_lpszClassName);
	else SetErrorEx(elemento, error, tipo);
}

void CConectorFormView::SetModificacion(const CString &id, bool force/*=true*/) {
	
	m_szModificacionesID= id;
	CPropiedades info(GetRuntimeClass()->m_lpszClassName, GetDocument()->GetTitle(), m_szModificacionesID);
	AfxGetMainWnd()->SendMessage(WM_UPDATEMODIFICATION, force, (LPARAM)&info);
}

void CConectorFormView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	if(bActivate && !m_szModificacionesID.IsEmpty()) 
		SetModificacion(m_szModificacionesID, false);
	CResizableFormView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}
